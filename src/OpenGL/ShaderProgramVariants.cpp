#include "ShaderProgramVariants.h"

#include "Shader.h"

ShaderProgramVariants::ShaderProgramVariants(const std::string& vertPath, const std::string& fragPath)
{
    if (!Shader::tryGetShaderCode(vertPath, &m_vertCode))
    {
        // std::string("Error while reading shader file ") + vertPath
        throw std::exception(); // TODO exception
    }

    if (!Shader::tryGetShaderCode(fragPath, &m_fragCode))
    {
        // std::string("Error while reading shader file ") + vertPath
        throw std::exception(); // TODO exception
    }
}

void ShaderProgramVariants::destroy()
{
    for (auto& program : programs)
    {
        program.second.destroy();
    }
    programs.clear();
}

ShaderProgram& ShaderProgramVariants::getProgram(const ShaderFlags flags)
{
    auto it = programs.find(flags);
    if (it == programs.end())
    {
        throw std::exception(); // TODO exception
    }

    return it->second;
}

bool ShaderProgramVariants::enableVariant(ShaderFlags flags)
{
    if (programs.contains(flags))
    {
        return true;
    }

    std::string modifiedVertCode = getCodeWithFlags(m_vertCode, flags);
    std::string modifiedFragCode = getCodeWithFlags(m_fragCode, flags);

    ShaderProgram program(modifiedVertCode, modifiedFragCode);

    programs.emplace(flags, program);
    return program.id != 0;
}

bool ShaderProgramVariants::enableVariants(const std::unordered_set<ShaderFlags>& variants)
{
    for (const auto flags : variants)
    {
        if (!enableVariant(flags))
        {
            return false;
        }
    }
    return true;
}

auto ShaderProgramVariants::getCodeWithFlags(const std::string_view& code, const ShaderFlags flags) -> std::string
{
    constexpr std::size_t afterVersionIndex = 13;

    std::string defines;
    if (flags & ShaderHasNormals)
        defines += "#define HAS_NORMALS\n";
    if (flags & ShaderHasTangents)
        defines += "#define HAS_TANGENTS\n";
    if (flags & ShaderHasBaseColorMap)
        defines += "#define HAS_BASECOLORMAP\n";
    if (flags & ShaderHasMetalRoughnessMap)
        defines += "#define HAS_METALROUGHNESSMAP\n";
    if (flags & ShaderHasNormalMap)
        defines += "#define HAS_NORMALMAP\n";
    if (flags & ShaderHasEmissiveMap)
        defines += "#define HAS_EMISSIVEMAP\n";

    auto copy = std::string(code);
    if (defines.empty())
        return copy;

    copy.insert(afterVersionIndex, defines);
    return copy;
}
