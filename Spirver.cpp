#include "Spirver.h"
#include <fstream>
#include <iostream>
#include <istream>
#include <glslang/SPIRV/SpvTools.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <string>
#include <spirv_glsl.hpp>

using namespace Spirver;
using namespace Spirver::detail;
using namespace Spirver::proc;

Spirver::GlslShader::GlslShader(GlslShader& o) : ShaderCode(o.stage)
{
	*this = o;
}

Spirver::GlslShader::GlslShader(GlslShader&& o) : ShaderCode(o.stage)
{
	*this = std::move(o);
}

GlslShader& Spirver::GlslShader::operator=(GlslShader& o)
{
	this->code = o.code;
	this->stage = o.stage;
	this->errors = std::stringstream(o.errors.str());
	this->uniformProperties = o.uniformProperties;
	return *this;
}

GlslShader& Spirver::GlslShader::operator=(GlslShader&& o)
{
	this->code = std::move(o.code);
	this->stage = o.stage;
	this->errors = std::move(std::stringstream(o.errors.str()));
	this->uniformProperties = std::move(o.uniformProperties);
	return *this;
}

GlslShader GlslShader::FromFile(const std::string& path, Spirver::Stage stage)
{
	return GlslShader(fileToString(path.c_str()), stage);
}

GlslShader GlslShader::FromMemory(const std::string& code, Spirver::Stage stage)
{
	return GlslShader(code, stage);
}

std::string ShaderCode::GetErrors()
{	
	std::string ret = errors.str();
	errors = std::stringstream();
	return ret;
}

bool GlslShader::Optimize()
{
	std::string codeLegacy, codeLegacyOpt;
	modernGlslToLegacyGlsl(code, codeLegacy); // so that glsl-opt can handle it
	bool success = optimizeGlsl(codeLegacy.c_str(), codeLegacyOpt, stage);
	success &= legacyGlslToModernGlsl(codeLegacyOpt, code, uniformProperties); // and back to 4.6
	if (!success) errors << Spirver::proc::GetErrors();
	return success;
}

bool GlslShader::Compile(GLuint shader)
{
	const char* source = code.c_str();
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	bool success = printLog(shader, LogType::Shader);
	if (!success) errors << Spirver::proc::GetErrors();
	return success;
}

ShaderStat GlslShader::Analyze()
{
	return AnalyzeShader(code, stage);
}

bool GlslShader::ToFile(std::string&& path)
{
	return stringToFile(code, path);
}

SpirvShader GlslShader::ToSpirv()
{
	std::vector<GLuint> spirv;
	bool success = glslToSpirv(code, stage, spirv);
	SpirvShader spirvShader = SpirvShader::FromMemory(spirv, stage);
	if (!success) spirvShader.errors << Spirver::proc::GetErrors();
	return spirvShader;
}

Spirver::GlslShader::GlslShader(const std::string& code, Stage stage) : ShaderCode(stage)
{
	this->code = code;
	this->stage = stage;
	getUniformLocations(this->code, this->uniformProperties); // code is immutable, so we can do this here
}


Spirver::SpirvShader::SpirvShader(SpirvShader& o) : ShaderCode(o.stage)
{
	*this = o;
}

Spirver::SpirvShader::SpirvShader(SpirvShader&& o) : ShaderCode(o.stage)
{
	*this = std::move(o);
}

SpirvShader& Spirver::SpirvShader::operator=(SpirvShader& o)
{
	this->spirv = o.spirv;
	this->stage = o.stage;
	this->errors = std::stringstream(o.errors.str());
	return *this;
}

SpirvShader& Spirver::SpirvShader::operator=(SpirvShader&& o)
{
	this->spirv = std::move(o.spirv);
	this->stage = o.stage;
	this->errors = std::move(o.errors);
	return *this;
}

SpirvShader SpirvShader::FromFile(const std::string& path, Spirver::Stage stage)
{
	std::vector<char> data;
	fileToSpirv(path.c_str(), data);
	std::vector<GLuint> datauint(data.begin(), data.end());
	return SpirvShader(datauint, stage);
}

SpirvShader SpirvShader::FromMemory(const std::vector<GLuint>& spirv, Spirver::Stage stage)
{
	return SpirvShader(spirv, stage);
}

bool SpirvShader::Optimize()
{
	bool success = optimizeSpirv(spirv);
	if (!success) errors << Spirver::proc::GetErrors();
	return success;
}

bool SpirvShader::Compile(GLuint id)
{
	glShaderBinary(1, &id, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(GLuint)); // load binary
	bool success = printLog(id, LogType::PrespecShader);
	if (!success) errors << Spirver::proc::GetErrors();
	glSpecializeShader(id, "main", 0, nullptr, nullptr); // no constants to specialize
	success = printLog(id, LogType::Shader);
	if (!success) errors << Spirver::proc::GetErrors();
	return success;
}

ShaderStat SpirvShader::Analyze()
{
	return AnalyzeShader(spirv, stage);
}

bool SpirvShader::ToFile(std::string&& path)
{
	return spirvToFile(spirv, path);
}

GlslShader SpirvShader::ToGlsl()
{
	std::string code;
	spirvToGlsl(spirv, code);
	return GlslShader::FromMemory(code, stage);
}

inline Spirver::SpirvShader::SpirvShader(const std::vector<GLuint>& spirv, Stage stage) : ShaderCode(stage)
{
	this->spirv = spirv;
	this->stage = stage;
}


#pragma region Lifecycle

void Spirver::Init()
{
	InitGlslOpt();
	InitGlslang();
	InitSpirvOpt();
}

void Spirver::Clean()
{
	CleanGlslOpt();
	CleanGlslang();
	CleanSpirvOpt();
}

#pragma endregion

#pragma region Stage enum

const char* Spirver::stageNames[] = { "Vertex", "Fragment", "Geometry", "Compute" };

GLuint Spirver::StageToGlsl(Stage stage)
{
	switch (stage)
	{
	case Stage::Vertex: return GL_VERTEX_SHADER;
	case Stage::Fragment: return GL_FRAGMENT_SHADER;
	case Stage::Geometry: return GL_GEOMETRY_SHADER;
	case Stage::Compute: return GL_COMPUTE_SHADER;
	default: return GL_VERTEX_SHADER;
	}
}

EShLanguage Spirver::StageToGlslang(Stage stage)
{
	switch (stage)
	{
	case Stage::Vertex: return EShLanguage::EShLangVertex;
	case Stage::Fragment: return EShLanguage::EShLangFragment;
	case Stage::Geometry: return EShLanguage::EShLangGeometry;
	case Stage::Compute: return EShLanguage::EShLangCompute;
	default: return EShLanguage::EShLangVertex;
	}
}

glslopt_shader_type Spirver::StageToGlslopt(Stage stage)
{
	switch (stage)
	{
	case Stage::Vertex: return kGlslOptShaderVertex;
	case Stage::Fragment: return kGlslOptShaderFragment;
	default: return kGlslOptShaderVertex;
	}

	//glslopt cant handle geometry or compute shaders!
}

int Spirver::StageToInt(Stage stage)
{
	switch (stage)
	{
	case Stage::Vertex: return 0;
	case Stage::Fragment: return 1;
	case Stage::Geometry: return 2;
	case Stage::Compute: return 3;
	default: return 0;
	}
}

Spirver::Stage Spirver::StageToSpirver(GLenum stage)
{
	switch (stage)
	{
	case GL_VERTEX_SHADER: return Stage::Vertex;
	case GL_FRAGMENT_SHADER: return Stage::Fragment;
	case GL_GEOMETRY_SHADER: return Stage::Geometry;
	case GL_COMPUTE_SHADER: return Stage::Compute;
	default: return Spirver::Stage::Vertex;
	}
}

Spirver::Stage Spirver::StageToSpirver(EShLanguage stage)
{
	switch (stage)
	{
	case EShLanguage::EShLangVertex: return Stage::Vertex;
	case EShLanguage::EShLangFragment: return Stage::Fragment;
	case EShLanguage::EShLangGeometry: return Stage::Geometry;
	case EShLanguage::EShLangCompute: return Stage::Compute;
	default: return Spirver::Stage::Vertex;
	}
}
Spirver::Stage Spirver::StageToSpirver(int i)
{
	switch (i)
	{
	case 0: return Stage::Vertex;
	case 1: return Stage::Fragment;
	case 2: return Stage::Geometry;
	case 3: return Stage::Compute;
	default: return Stage::Vertex;
	}
}
Spirver::Stage Spirver::StageToSpirver(const std::string& s)
{
	if (s == ".vert" || s == "vert") return Stage::Vertex;
	else if (s == ".frag" || s == "frag") return Stage::Fragment;
	else if (s == ".geom" || s == "geom") return Stage::Geometry;
	else if (s == ".comp" || s == "comp") return Stage::Compute;
	else return Stage::Vertex;
}

#pragma endregion

#pragma region Logging

std::string Spirver::proc::GetErrors()
{
	std::string ret = errors.str();
	errors = std::stringstream();
	return ret;
}

#pragma endregion

#pragma region IO

std::string Spirver::proc::fileToString(const char* filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) return "";

	std::string shaderCode = "";
	std::string line = "";
	while (std::getline(file, line)) // load file line by line
		shaderCode += line + "\n";
	file.close();

	return shaderCode;
}

bool Spirver::proc::fileToSpirv(const char* filename, std::vector<char>& data)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) return false;

	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	//data = std::vector<GLuint>(size / sizeof(GLuint));
	//file.read((char*)data.data(), size);
	data = std::vector<char>(std::istreambuf_iterator<char>(file), {});
	file.close();
	return true;
}

bool Spirver::proc::stringToFile(const std::string& str, const std::string& path)
{
	std::ofstream file(path, std::ios::binary);
	if (!file.is_open()) return false;
	
	file << str;
	file.close();
	return true;
}

#pragma endregion

#pragma region Compilation

bool Spirver::proc::glslToSpirv(const std::string& glsl, Stage stage, int& uniformBase, std::vector<GLuint>& spirv)
{
	InitGlslang();
	
	// to AST shader
	glslang::TShader* astshader = new glslang::TShader(StageToGlslang(stage));
	if (uniformBase >= 0) { if (!glslToAstShader(glsl, astshader, uniformBase)) return false; }
	else { if (!glslToAstShader(glsl, astshader)) return false; }

	// to AST program
	glslang::TProgram* astprogram = new glslang::TProgram();
	if (!astShaderToAstProgram(astshader, astprogram)) return false;

	// to binary
	bool ret = astProgramToSpirv(astprogram, spirv, stage);
	delete astprogram; delete astshader;
	return ret;
}

bool Spirver::proc::legacyGlslToModernGlsl(const std::string& source, std::string& output, const std::map<std::string, UniformProperties>& uniformLocations)
{
	std::istringstream s(source);
	output = std::string();
	output.reserve(source.size());
	for (std::string line; std::getline(s, line); )
	{
		// version
		if (std::regex_match(line, regVersion))
		{
			output += "#version 460\n";
			continue;
		}

		// if no uniform variable, continue
		std::smatch matchNameNoLayout;
		if (!std::regex_match(line, matchNameNoLayout, regNameNoLayout))
		{
			output += line + "\n";
			continue;
		}

		// if no stored properties, continue
		std::string name = matchNameNoLayout.str(1);
		UniformProperties uprops = (*uniformLocations.find(name)).second;
		if (uprops.isEmpty())
		{
			output += line + "\n";
			continue;
		}

		// write stored properties
		output += "layout(";
		bool firstcomma = true;
		
		if (uprops.location != -1)
		{
			if (firstcomma) firstcomma = false;
			else output += ',';
			output += "location=" + std::to_string(uprops.location);
		}

		if (uprops.binding != -1)
		{
			if (firstcomma) firstcomma = false;
			else output += ',';
			output += "binding=" + std::to_string(uprops.binding);
		}

		output += ") " + line + "\n";

	}

	return true;
}


bool Spirver::proc::modernGlslToLegacyGlsl(const std::string& source, std::string& output)
{
	std::istringstream s(source);
	output = std::string();
	output.reserve(source.size());
	for (std::string line; std::getline(s, line); )
	{
		// version
		if (std::regex_match(line, regVersion))
		{
			output += "#version 150\n";
			continue;
		}
		
		// if no layout block, add line
		std::smatch matchLayout;
		if (!std::regex_match(line, matchLayout, regLayoutAndName))
		{
			output += line + "\n";
			continue;
		}

		// otherwise remove layout block
		output += std::string(line.begin() + matchLayout.length(1), line.end()) + "\n";
	}

	return true;
}

void Spirver::proc::getUniformLocations(const std::string& source, std::map<std::string, UniformProperties>& uniformLocations)
{
	std::istringstream s(source);
	for (std::string line; std::getline(s, line); )
	{
		// if no layout block, continue
		std::smatch matchLayoutAndName;
		if (!std::regex_match(line, matchLayoutAndName, regLayoutAndName)) continue;

		std::string name = matchLayoutAndName.str(2);
		std::string layoutStr = matchLayoutAndName.str(1);
	
		std::smatch match_loc;
		int loc = -1;
		if (std::regex_match(line, match_loc, regLocation)) loc = std::stoi(match_loc.str(1));
		
		std::smatch match_bind;
		int bind = -1;
		if (std::regex_match(line, match_bind, regBinding)) bind = std::stoi(match_bind.str(1));

		uniformLocations[name] = UniformProperties{ loc, bind };
	}
}

#pragma endregion

#pragma region Optimization

bool Spirver::proc::optimizeGlsl(const char* source, std::string& optimized, Stage stage)
{
	InitGlslOpt();

	if (stage != Stage::Vertex && stage != Stage::Fragment) return false; // glsl-opt can't handle other stages

	glslopt_shader* shader = glslopt_optimize(glslOptCtx, StageToGlslopt(stage), source, kGlslOptionNotFullShader);
	if (glslopt_get_status(shader)) // if successful
	{
		optimized = std::string(glslopt_get_output(shader));
		glslopt_shader_delete(shader);
		return true;
	}
	else
	{
		glslopt_shader_delete(shader);
		return printLog(shader);
	}
}

#pragma endregion

#pragma region Analysis

ShaderStat Spirver::proc::AnalyzeShader(const std::string& glsl, Spirver::Stage stage)
{
	InitGlslang();
	
	// to AST
	glslang::TShader* astshader = new glslang::TShader(StageToGlslang(stage));
	glslToAstShader(glsl, astshader);

	// analysis
	ShaderStat ret = AnalyzeAstShader(astshader);
	ret.stats[glslLines] = std::count(glsl.begin(), glsl.end(), '\n');
	
	delete astshader;
	return ret;
}

#pragma endregion



#pragma region detail

#pragma region Lifecycle

glslopt_ctx* Spirver::detail::glslOptCtx = nullptr;
bool Spirver::detail::isGlslangInitialized = false;
spvtools::Optimizer* Spirver::detail::spirvOpt = nullptr;
spvtools::SpirvTools* Spirver::detail::spirvTools = nullptr;

void Spirver::detail::InitGlslOpt()
{
	if (IsGlslOptInitialized()) return;
	
	glslOptCtx = glslopt_initialize(kGlslTargetOpenGL);
}

void Spirver::detail::CleanGlslOpt()
{
	if (!IsGlslOptInitialized()) return;

	glslopt_cleanup(glslOptCtx);
	glslOptCtx = nullptr;
}

void Spirver::detail::InitGlslang()
{
	if (IsGlslangInitialized()) return;

	glslang::InitializeProcess();
	isGlslangInitialized = true;
}

void Spirver::detail::CleanGlslang()
{
	if (!IsGlslangInitialized()) return
	
	glslang::FinalizeProcess();
	isGlslangInitialized = false;
}

void Spirver::detail::InitSpirvOpt()
{
	// SpirvOpt needs to be reinitialized every time
	if (spirvOpt != nullptr) delete spirvOpt;
	
	spirvOpt = new spvtools::Optimizer(SPV_ENV_OPENGL_4_5);
	spirvOpt->SetMessageConsumer(printSpirvOptLog);
	//spirvOpt.RegisterPerformancePasses(); // assign passes automatically
	spirvOpt->RegisterPass(spvtools::CreateWrapOpKillPass())
		.RegisterPass(spvtools::CreateDeadBranchElimPass())
		.RegisterPass(spvtools::CreateMergeReturnPass())
		.RegisterPass(spvtools::CreateInlineExhaustivePass())
		.RegisterPass(spvtools::CreateEliminateDeadFunctionsPass())
		.RegisterPass(spvtools::CreateAggressiveDCEPass())
		.RegisterPass(spvtools::CreatePrivateToLocalPass())
		.RegisterPass(spvtools::CreateLocalSingleBlockLoadStoreElimPass())
		.RegisterPass(spvtools::CreateLocalSingleStoreElimPass())
		.RegisterPass(spvtools::CreateAggressiveDCEPass())
		.RegisterPass(spvtools::CreateScalarReplacementPass())
		.RegisterPass(spvtools::CreateLocalAccessChainConvertPass())
		.RegisterPass(spvtools::CreateLocalSingleBlockLoadStoreElimPass())
		.RegisterPass(spvtools::CreateLocalSingleStoreElimPass())
		.RegisterPass(spvtools::CreateAggressiveDCEPass())
		.RegisterPass(spvtools::CreateLocalMultiStoreElimPass())
		.RegisterPass(spvtools::CreateAggressiveDCEPass())
		.RegisterPass(spvtools::CreateCCPPass())
		.RegisterPass(spvtools::CreateAggressiveDCEPass())
		.RegisterPass(spvtools::CreateLoopUnrollPass(true))
		.RegisterPass(spvtools::CreateDeadBranchElimPass())
		.RegisterPass(spvtools::CreateRedundancyEliminationPass())
		.RegisterPass(spvtools::CreateCombineAccessChainsPass())
		.RegisterPass(spvtools::CreateSimplificationPass())
		.RegisterPass(spvtools::CreateScalarReplacementPass())
		.RegisterPass(spvtools::CreateLocalAccessChainConvertPass())
		.RegisterPass(spvtools::CreateLocalSingleBlockLoadStoreElimPass())
		.RegisterPass(spvtools::CreateLocalSingleStoreElimPass())
		.RegisterPass(spvtools::CreateAggressiveDCEPass())
		.RegisterPass(spvtools::CreateSSARewritePass())
		.RegisterPass(spvtools::CreateAggressiveDCEPass())
		.RegisterPass(spvtools::CreateVectorDCEPass())
		.RegisterPass(spvtools::CreateDeadInsertElimPass())
		.RegisterPass(spvtools::CreateDeadBranchElimPass())
		.RegisterPass(spvtools::CreateSimplificationPass())
		.RegisterPass(spvtools::CreateIfConversionPass())
		.RegisterPass(spvtools::CreateCopyPropagateArraysPass())
		.RegisterPass(spvtools::CreateReduceLoadSizePass())
		.RegisterPass(spvtools::CreateAggressiveDCEPass())
		.RegisterPass(spvtools::CreateBlockMergePass())
		.RegisterPass(spvtools::CreateRedundancyEliminationPass())
		.RegisterPass(spvtools::CreateDeadBranchElimPass())
		.RegisterPass(spvtools::CreateBlockMergePass())
		.RegisterPass(spvtools::CreateSimplificationPass());
}

void Spirver::detail::CleanSpirvOpt()
{
	if (!IsSpirvOptInitialized()) return;

	delete spirvOpt;
	spirvOpt = nullptr;
}

void Spirver::detail::InitSpirvTools()
{
	if (IsSpirvToolsInitialized()) return;

	spirvTools = new spvtools::SpirvTools(SPV_ENV_OPENGL_4_5);
}

void Spirver::detail::CleanSpirvTools()
{
	if (!IsSpirvToolsInitialized()) return;

	delete spirvTools;
	spirvTools = nullptr;
}

#pragma endregion

#pragma region Logging

std::stringstream Spirver::detail::errors = std::stringstream();
const char* Spirver::detail::logTypeStr[] = { "Program", "Shader", "PrespecShader" };

bool Spirver::detail::printLog(GLuint object, LogType logType)
{
	GLint success = GL_FALSE;
	int logLength = 100;

	switch (logType)
	{
	case LogType::Program:
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logLength);
		break;
	case LogType::Shader:
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
		break;
	case LogType::PrespecShader:
		glGetShaderiv(object, GL_SPIR_V_BINARY, &success);
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
		break;
	}

	if (!success)
	{
		if (logLength > 0)
		{
			char* strInfoLog = new char[logLength];
			switch (logType)
			{
			case LogType::Program:
				glGetProgramInfoLog(object, logLength, NULL, strInfoLog);
				break;
			case LogType::Shader:
			case LogType::PrespecShader:
				glGetShaderInfoLog(object, logLength, NULL, strInfoLog);
				break;
			}

			std::cout << logTypeStr[(int)logType] << " error:" << std::endl << strInfoLog << std::endl;
			errors << logTypeStr[(int)logType] << " error:" << std::endl << strInfoLog << std::endl;

			delete[] strInfoLog;
		}
		else
		{
			std::cout << logTypeStr[(int)logType] << " error!" << std::endl;
			errors << logTypeStr[(int)logType] << " error!" << std::endl;
		}

		return false;
	}
	else return true;
}

bool Spirver::detail::printLog(glslang::TShader* object)
{
	std::string log(object->getInfoLog());
	if (log.size() > 0)
	{
		std::cout << "AST shader error: " << std::endl << log << std::endl;
		errors << "AST shader error: " << std::endl << log << std::endl;
		return false;
	}
	return true;
}

bool Spirver::detail::printLog(glslang::TProgram* object)
{
	std::string log(object->getInfoLog());
	if (log.size() > 0)
	{
		std::cout << "AST program error: " << std::endl << log << std::endl;
		errors << "AST program error: " << std::endl << log << std::endl;
		return false;
	}
	return true;
}

bool Spirver::detail::printLog(spv::SpvBuildLogger& object)
{
	std::string log = object.getAllMessages();
	if (log.length() > 0)
	{
		std::cout << "SPIR-V build error: " << std::endl << log;
		errors << "SPIR-V build error: " << std::endl << log;
		return false;
	}
	return true;
}

bool Spirver::detail::printLog(glslopt_shader* object)
{
	std::cout << "GLSL-Optimizer error: " << std::endl << glslopt_get_log(object);
	errors << "GLSL-Optimizer error: " << std::endl << glslopt_get_log(object);
	return false;
}

void Spirver::detail::printSpirvOptLog(spv_message_level_t level, const char* source,
	const spv_position_t& position, const char* msg)
{
	switch (level) {
	case SPV_MSG_FATAL:
	case SPV_MSG_INTERNAL_ERROR:
	case SPV_MSG_ERROR:
		std::cerr << "Spir-V optimizer error at line " << position.index << ": " << msg
			<< std::endl;
		break;
	case SPV_MSG_WARNING:
		std::cout << "Spir-V optimizer warning at line " << position.index << ": " << msg
			<< std::endl;
		break;
	case SPV_MSG_INFO:
		std::cout << "Spir-V optimizer info at line " << position.index << ": " << msg
			<< std::endl;
		break;
	default:
		break;
	}
}

#pragma endregion

#pragma region Analysis

ShaderStat Spirver::detail::AnalyzeAstShader(glslang::TShader* shader)
{
	SpirverAstAnalyzerTraverser t;
#ifdef _WIN32
	shader->getIntermediate()->getTreeRoot()->traverse(&t);
#endif
	return t.GetShaderStat();
}

#pragma endregion

#pragma region Compilation

bool Spirver::detail::glslToAstShader(const std::string& source, glslang::TShader* shader, int uniformBase)
{
	const char* const s[] = { source.c_str() };
	shader->setStrings(s, 1);
	shader->setEnvInput(glslang::EShSource::EShSourceGlsl, shader->getStage(), glslang::EShClient::EShClientOpenGL, 460);
	shader->setEnvClient(glslang::EShClient::EShClientOpenGL, glslang::EShTargetClientVersion::EShTargetOpenGL_450);
	shader->setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);
	if (uniformBase > -1)
	{
		shader->setUniformLocationBase(uniformBase);
		shader->setAutoMapLocations(true);
	}
	
	shader->parse(&DefaultTBuiltInResource, 110, false, EShMessages::EShMsgSpvRules);
	return printLog(shader);
}

bool Spirver::detail::astShaderToAstProgram(glslang::TShader* shader, glslang::TProgram* program)
{
	program->addShader(shader);
	program->link(EShMessages::EShMsgDefault);
	return printLog(program);
}

bool Spirver::detail::astProgramToSpirv(glslang::TProgram* program, std::vector<GLuint>& spirv, Stage stage)
{
	spv::SpvBuildLogger logger;
	glslang::SpvOptions spvOptions;
	spvOptions.disableOptimizer = true; // we want to optimize separately
	glslang::GlslangToSpv(*(program->getIntermediate(StageToGlslang(stage))), spirv, &logger, &spvOptions);
	return printLog(logger);
}

#pragma endregion

const TBuiltInResource Spirver::detail::DefaultTBuiltInResource = {
	/* .MaxLights = */ 32,
	/* .MaxClipPlanes = */ 6,
	/* .MaxTextureUnits = */ 32,
	/* .MaxTextureCoords = */ 32,
	/* .MaxVertexAttribs = */ 64,
	/* .MaxVertexUniformComponents = */ 4096,
	/* .MaxVaryingFloats = */ 64,
	/* .MaxVertexTextureImageUnits = */ 32,
	/* .MaxCombinedTextureImageUnits = */ 80,
	/* .MaxTextureImageUnits = */ 32,
	/* .MaxFragmentUniformComponents = */ 4096,
	/* .MaxDrawBuffers = */ 32,
	/* .MaxVertexUniformVectors = */ 128,
	/* .MaxVaryingVectors = */ 8,
	/* .MaxFragmentUniformVectors = */ 16,
	/* .MaxVertexOutputVectors = */ 16,
	/* .MaxFragmentInputVectors = */ 15,
	/* .MinProgramTexelOffset = */ -8,
	/* .MaxProgramTexelOffset = */ 7,
	/* .MaxClipDistances = */ 8,
	/* .MaxComputeWorkGroupCountX = */ 65535,
	/* .MaxComputeWorkGroupCountY = */ 65535,
	/* .MaxComputeWorkGroupCountZ = */ 65535,
	/* .MaxComputeWorkGroupSizeX = */ 1024,
	/* .MaxComputeWorkGroupSizeY = */ 1024,
	/* .MaxComputeWorkGroupSizeZ = */ 64,
	/* .MaxComputeUniformComponents = */ 1024,
	/* .MaxComputeTextureImageUnits = */ 16,
	/* .MaxComputeImageUniforms = */ 8,
	/* .MaxComputeAtomicCounters = */ 8,
	/* .MaxComputeAtomicCounterBuffers = */ 1,
	/* .MaxVaryingComponents = */ 60,
	/* .MaxVertexOutputComponents = */ 64,
	/* .MaxGeometryInputComponents = */ 64,
	/* .MaxGeometryOutputComponents = */ 128,
	/* .MaxFragmentInputComponents = */ 128,
	/* .MaxImageUnits = */ 8,
	/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
	/* .MaxCombinedShaderOutputResources = */ 8,
	/* .MaxImageSamples = */ 0,
	/* .MaxVertexImageUniforms = */ 0,
	/* .MaxTessControlImageUniforms = */ 0,
	/* .MaxTessEvaluationImageUniforms = */ 0,
	/* .MaxGeometryImageUniforms = */ 0,
	/* .MaxFragmentImageUniforms = */ 8,
	/* .MaxCombinedImageUniforms = */ 8,
	/* .MaxGeometryTextureImageUnits = */ 16,
	/* .MaxGeometryOutputVertices = */ 256,
	/* .MaxGeometryTotalOutputComponents = */ 1024,
	/* .MaxGeometryUniformComponents = */ 1024,
	/* .MaxGeometryVaryingComponents = */ 64,
	/* .MaxTessControlInputComponents = */ 128,
	/* .MaxTessControlOutputComponents = */ 128,
	/* .MaxTessControlTextureImageUnits = */ 16,
	/* .MaxTessControlUniformComponents = */ 1024,
	/* .MaxTessControlTotalOutputComponents = */ 4096,
	/* .MaxTessEvaluationInputComponents = */ 128,
	/* .MaxTessEvaluationOutputComponents = */ 128,
	/* .MaxTessEvaluationTextureImageUnits = */ 16,
	/* .MaxTessEvaluationUniformComponents = */ 1024,
	/* .MaxTessPatchComponents = */ 120,
	/* .MaxPatchVertices = */ 32,
	/* .MaxTessGenLevel = */ 64,
	/* .MaxViewports = */ 16,
	/* .MaxVertexAtomicCounters = */ 0,
	/* .MaxTessControlAtomicCounters = */ 0,
	/* .MaxTessEvaluationAtomicCounters = */ 0,
	/* .MaxGeometryAtomicCounters = */ 0,
	/* .MaxFragmentAtomicCounters = */ 8,
	/* .MaxCombinedAtomicCounters = */ 8,
	/* .MaxAtomicCounterBindings = */ 1,
	/* .MaxVertexAtomicCounterBuffers = */ 0,
	/* .MaxTessControlAtomicCounterBuffers = */ 0,
	/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
	/* .MaxGeometryAtomicCounterBuffers = */ 0,
	/* .MaxFragmentAtomicCounterBuffers = */ 1,
	/* .MaxCombinedAtomicCounterBuffers = */ 1,
	/* .MaxAtomicCounterBufferSize = */ 16384,
	/* .MaxTransformFeedbackBuffers = */ 4,
	/* .MaxTransformFeedbackInterleavedComponents = */ 64,
	/* .MaxCullDistances = */ 8,
	/* .MaxCombinedClipAndCullDistances = */ 8,
	/* .MaxSamples = */ 4,
	/* .maxMeshOutputVerticesNV = */ 256,
	/* .maxMeshOutputPrimitivesNV = */ 512,
	/* .maxMeshWorkGroupSizeX_NV = */ 32,
	/* .maxMeshWorkGroupSizeY_NV = */ 1,
	/* .maxMeshWorkGroupSizeZ_NV = */ 1,
	/* .maxTaskWorkGroupSizeX_NV = */ 32,
	/* .maxTaskWorkGroupSizeY_NV = */ 1,
	/* .maxTaskWorkGroupSizeZ_NV = */ 1,
	/* .maxMeshViewCountNV = */ 4,
	/* .maxDualSourceDrawBuffersEXT = */ 1,

	/* .limits = */ {
		/* .nonInductiveForLoops = */ 1,
		/* .whileLoops = */ 1,
		/* .doWhileLoops = */ 1,
		/* .generalUniformIndexing = */ 1,
		/* .generalAttributeMatrixVectorIndexing = */ 1,
		/* .generalVaryingIndexing = */ 1,
		/* .generalSamplerIndexing = */ 1,
		/* .generalVariableIndexing = */ 1,
		/* .generalConstantMatrixVectorIndexing = */ 1,
	} };

#pragma endregion