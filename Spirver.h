#pragma once

#include <string>
#include <GL/glew.h>
#include <glslang/Include/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <ostream>
#include <glslang/Public/ShaderLang.h>
#include <spirv-tools/optimizer.hpp>
#include <glslang/SPIRV/SpvTools.h>
#include <glsl_optimizer.h>
#include <sstream>
#include <SpirverAstAnalyzer.h>
#include <regex>


/// Compile, convert and analyze shaders with static functions
namespace Spirver
{

#pragma region Lifecycle

/// Initialize everything now instead of lazily
void Init();
/// Free up memory used by Spirver
void Clean();

#pragma endregion

#pragma region Stage enum

/// GLSL shader stage 
enum class Stage { Vertex = 0, Fragment = 1, Geometry = 2, Compute = 3, StageCount = 4};
/// Spirver::Stage to string
extern const char* stageNames[];

// enum conversion
GLuint StageToGlsl(Stage stage);
EShLanguage StageToGlslang(Stage stage);
glslopt_shader_type StageToGlslopt(Stage stage);
int StageToInt(Stage stage);
Stage StageToSpirver(GLenum stage);
Stage StageToSpirver(EShLanguage stage);
Stage StageToSpirver(int i);
Stage StageToSpirver(const std::string& s);
inline GLuint StageToGlsl(EShLanguage stage) { return StageToGlsl(StageToSpirver(stage)); }
inline GLuint StageToGlsl(int i) { return StageToGlsl(StageToSpirver(i)); }

#pragma endregion

#pragma region UniformProperties

/// Describes the contents of layout() for a variable
struct UniformProperties
{
	int location = -1;
	int binding = -1;
	bool isEmpty() { return location == -1 && binding == -1; }
};

#pragma endregion

#pragma region ShaderInterface

/// Abstract class for shader source code
class ShaderCode
{
public:
	ShaderCode() = delete;
	virtual ~ShaderCode() {};

	virtual bool Optimize() = 0;
	virtual bool Compile(GLuint shader) = 0;
	virtual ShaderStat Analyze() = 0;
	virtual bool ToFile(std::string&& path) = 0;

	Stage GetStage() { return stage; }
	bool HasErrors() { return errors.str().length() > 0; }
	std::string GetErrors();

protected:
	Stage stage;
	std::stringstream errors;

	ShaderCode(Stage stage) : stage(stage) {}
};

class SpirvShader;

/// GLSL shader source code
class GlslShader : public ShaderCode
{
public:
	GlslShader() : ShaderCode(Stage::Vertex) {};
	GlslShader(GlslShader& o);
	GlslShader(GlslShader&& o);
	GlslShader& operator=(GlslShader& o);
	GlslShader& operator=(GlslShader&& o);

	static GlslShader FromFile(const std::string& path, Stage stage);
	static GlslShader FromMemory(const std::string& code, Stage stage);

	bool Optimize() override;
	bool Compile(GLuint shader) override;
	ShaderStat Analyze() override;
	bool ToFile(std::string&& path) override;

	SpirvShader ToSpirv();

private:
	GlslShader(const std::string& code, Stage stage);

	std::string code;
	std::map<std::string, UniformProperties> uniformProperties;

	friend class SpirvShader;
};

/// SPIR-V shader source code
class SpirvShader : public ShaderCode
{
public:
	SpirvShader() : ShaderCode(Stage::Vertex) {};
	SpirvShader(SpirvShader& o);
	SpirvShader(SpirvShader&& o);
	SpirvShader& operator=(SpirvShader& o);
	SpirvShader& operator=(SpirvShader&& o);

	static SpirvShader FromFile(const std::string& path, Stage stage);
	static SpirvShader FromMemory(const std::vector<GLuint>& spirv, Stage stage);

	bool Optimize() override;
	bool Compile(GLuint shader) override;
	ShaderStat Analyze() override;
	bool ToFile(std::string&& path) override;

	GlslShader ToGlsl();

private:
	SpirvShader(const std::vector<GLuint>& spirv, Stage stage);

	std::vector<GLuint> spirv;

	friend class GlslShader;
};

#pragma endregion

}; // Spirver





namespace Spirver::proc {

#pragma region Logging

/// Get all error messages and clear stored errors
std::string GetErrors();

#pragma endregion

#pragma region IO

std::string fileToString(const char* filename);
bool fileToSpirv(const char* filename, std::vector<char>& data);
template<typename T>
bool spirvToFile(const std::vector<T>& spirv, const std::string filename);
bool stringToFile(const std::string& str, const std::string& path);

#pragma endregion

#pragma region Compilation

/// Convert GLSL with automatic uniform locations to SPIR-V
bool glslToSpirv(const std::string& glsl, Stage stage, int& uniformBase, std::vector<GLuint>& spirv);
/// Convert GLSL with explicit uniform locations to SPIR-V
inline bool glslToSpirv(const std::string& glsl, Stage stage, std::vector<GLuint>& spirv)
{
	int temp = -1;
	return glslToSpirv(glsl, stage, temp, spirv);
}

template<typename T>
static bool spirvToShader(const std::vector<T>& spirv, Stage stage, GLuint id);

template<typename T>
bool spirvToGlsl(const std::vector<T>& spirv, std::string& glsl);


/// Store the contents of layout() qualifiers of uniform variables
void getUniformLocations(const std::string& source, std::map<std::string, UniformProperties>& uniformLocations);

/// Transform shaders optimized with glsl-optimizer to ones glslang can consume
bool legacyGlslToModernGlsl(const std::string& source, std::string& output, const std::map<std::string, UniformProperties>& uniformLocations);
/// Transform shaders with layout() qualifiers to ones glsl-optimizer can consume
bool modernGlslToLegacyGlsl(const std::string& source, std::string& output);


#pragma endregion

#pragma region Optimization

/// Optimize using SPIRV-Tools
template<typename T>
bool optimizeSpirv(std::vector<T>& spirv);

/// Optimize using GLSL-Optimizer
bool optimizeGlsl(const char* source, std::string& optimized, Stage stage);
inline bool optimizeGlslFile(const char* filename, std::string& optimized, Stage stage)
{
	return optimizeGlsl(fileToString(filename).c_str(), optimized, stage);
};

#pragma endregion

#pragma region Analysis

ShaderStat AnalyzeShader(const std::string& glsl, Spirver::Stage stage);
template<typename T>
ShaderStat AnalyzeShader(const std::vector<T>& spirv, Spirver::Stage stage);

#pragma endregion

} // Spirver::proc




namespace Spirver::detail {
extern const TBuiltInResource DefaultTBuiltInResource;

#pragma region Lifecycle

extern glslopt_ctx* glslOptCtx;
extern bool isGlslangInitialized;
extern spvtools::Optimizer* spirvOpt;
extern spvtools::SpirvTools* spirvTools;
inline bool IsGlslangInitialized() { return isGlslangInitialized; }
inline bool IsGlslOptInitialized() { return glslOptCtx != nullptr; }
inline bool IsSpirvOptInitialized() { return spirvOpt != nullptr; }
inline bool IsSpirvToolsInitialized() { return spirvTools != nullptr; }

void InitGlslOpt();
void CleanGlslOpt();
void InitGlslang();
void CleanGlslang();
void InitSpirvOpt();
void CleanSpirvOpt();
void InitSpirvTools();
void CleanSpirvTools();

#pragma endregion

#pragma region Logging

// culmulative error messages
extern std::stringstream errors;
enum class LogType { Program = 0, Shader = 1, PrespecShader = 2, LogTypeCount = 3 };
extern const char* logTypeStr[(int)LogType::LogTypeCount];
// returns false on error, true on no error
bool printLog(GLuint program, LogType logType);
bool printLog(glslang::TShader* object);
bool printLog(glslang::TProgram* object);
bool printLog(spv::SpvBuildLogger& object);
bool printLog(glslopt_shader* object);
void printSpirvOptLog(spv_message_level_t level, const char* source, const spv_position_t& position, const char* msg);

#pragma endregion

#pragma region Analysis

ShaderStat AnalyzeAstShader(glslang::TShader* shader);

#pragma endregion

#pragma region Compilation

bool glslToAstShader(const std::string& source, glslang::TShader* shader, int uniformBase = -1);
bool astShaderToAstProgram(glslang::TShader* shader, glslang::TProgram* program);
bool astProgramToSpirv(glslang::TProgram* program, std::vector<GLuint>& spirv, Stage stage);

#pragma endregion

#pragma region Regex

// wl: whole line, 1: group 1...
inline const std::regex regLayoutAndName = std::regex(R"(\s*(layout.*\)\s+)(?:in|out|uniform)\s+\w+\s+(\w+)[\[\]\d ]*;.*)"); //wl, 1: layout(..), 2: name
inline const std::regex regLocation = std::regex(R"(.*location\s*=\s*(\d*).*)"); // wl, 1: location integer
inline const std::regex regBinding = std::regex(R"(.*binding\s*=\s*(\d*).*)"); // wl, 1: binding integer
inline const std::regex regStd140 = std::regex(R"(layout *.*(std140))");
inline const std::regex regNameNoLayout = std::regex(R"(\s*(?:in|out|uniform)\s+\w+\s+(\w+)[\[\]\d ]*;.*)"); // wl, s1: name
inline const std::regex regVersion = std::regex(R"(\s*#version\s+\d+\s*(?:core|compatibility)?\s*)"); // wl

#pragma endregion

} //Spirver::detail

#include <Spirver.inl>