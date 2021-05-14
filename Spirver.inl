#pragma once
#include <spirv-tools/optimizer.hpp>
#include <spirv-tools/libspirv.h>
#include <fstream>
#include <spirv_glsl.hpp>

namespace Spirver::proc {
using namespace Spirver::detail;

#pragma region IO

template<typename T>
bool spirvToFile(const std::vector<T>& spirv, const std::string filename)
{
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;

    out.write((char*)spirv.data(), spirv.size() * sizeof(T));
    out.close();
    return true;
}

#pragma endregion

#pragma region Compilation

template<typename T>
bool spirvToShader(const std::vector<T>& spirv, Stage stage, GLuint id)
{
    glShaderBinary(1, &id, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(T));
    printLog(id, LogType::PrespecShader);
    glSpecializeShader(id, "main", 0, nullptr, nullptr);
    return printLog(id, LogType::Shader);
}

template<typename T>
bool spirvToGlsl(const std::vector<T>& spirv, std::string& glsl)
{
    spirv_cross::CompilerGLSL glslComp(spirv);
    //spirv_cross::ShaderResources res = glslComp.get_shader_resources(); // reflection stuff
    spirv_cross::CompilerGLSL::Options options;
    options.version = 460;
    glslComp.set_common_options(options);
    glsl = glslComp.compile();
    return true;
}

#pragma endregion

#pragma region Optimization

template<typename T>
bool optimizeSpirv(std::vector<T>& spirv)
{
    InitSpirvOpt();
    return spirvOpt->Run(spirv.data(), spirv.size(), &spirv);
}

#pragma endregion

#pragma region Analysis

template<typename T>
ShaderStat AnalyzeShader(const std::vector<T>& spirv, Spirver::Stage stage)
{
    std::string glsl;
    spirvToGlsl(spirv, glsl);
    ShaderStat stat = AnalyzeShader(glsl, stage);

    spvtools::SpirvTools spvt(SPV_ENV_OPENGL_4_5);
    std::string spirvAsm;
    spvt.Disassemble(spirv, &spirvAsm);
    stat.stats[spirvLines] = std::count(spirvAsm.c_str(), spirvAsm.c_str() + spirvAsm.length(), '\n');
    return stat;
}

#pragma endregion

} //Spirver