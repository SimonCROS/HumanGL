#include "ShaderProgram.h"

#include "Shader.h"
#include "MicroGLTF/Struct.h"

ShaderProgram::ShaderProgram(std::string&& vertCode, std::string&& fragCode)
    : m_vertCode(std::move(vertCode)), m_fragCode(std::move(fragCode))
{
}

auto ShaderProgram::Create(const std::string& vertPath,
                                   const std::string& fragPath) -> Expected<ShaderProgram, std::string>
{
    std::string vertCode;
    std::string fragCode;

    if (!Shader::tryGetShaderCode(vertPath, vertCode))
        return Unexpected(std::string("Error while reading shader file ") + vertPath);

    if (!Shader::tryGetShaderCode(fragPath, fragCode))
        return Unexpected(std::string("Error while reading shader file ") + fragPath);

    return Expected<ShaderProgram, std::string>{std::in_place, std::move(vertCode), std::move(fragCode)};
}

void ShaderProgram::destroy()
{
    for (auto& program : programs)
    {
        program.second.destroy();
    }
    programs.clear();
}

ShaderProgramInstance& ShaderProgram::getProgram(const ShaderFlags flags)
{
    auto it = programs.find(flags);
    if (it == programs.end())
    {
        throw std::exception(); // TODO exception
    }

    return it->second;
}

auto ShaderProgram::enableVariant(ShaderFlags flags)
    -> std::expected<std::reference_wrapper<ShaderProgramInstance>, std::string>
{
    const auto it = programs.find(flags);
    if (it != programs.end())
        return it->second;

    const std::string modifiedVertCode = getCodeWithFlags(m_vertCode, flags);
    const std::string modifiedFragCode = getCodeWithFlags(m_fragCode, flags);

    ShaderProgramInstance program(modifiedVertCode, modifiedFragCode);
    if (program.id != 0)

        programs.emplace(flags, program);
    return program.id != 0;
}

bool ShaderProgram::enableVariants(const std::unordered_set<ShaderFlags>& variants)
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

auto ShaderProgram::getCodeWithFlags(const std::string_view& code, const ShaderFlags flags) -> std::string
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
    if (flags & ShaderHasVec3Colors)
        defines += "#define HAS_VEC3_COLORS\n";

    auto copy = std::string(code);
    if (defines.empty())
        return copy;

    copy.insert(afterVersionIndex, defines);
    return copy;
}

auto GetPrimitiveShaderFlags(const microgltf::Model& model, const microgltf::Primitive& primitive) -> ShaderFlags
{
    ShaderFlags primitiveShaderFlags = ShaderHasNone; // TODO store flags in primitive when loading to avoid recalculate
    for (const auto& [attribute, accessorId] : primitive.attributes)
    {
        if (attribute == "NORMAL")
            primitiveShaderFlags |= ShaderHasNormals;
        else if (attribute == "TANGENT")
            primitiveShaderFlags |= ShaderHasTangents;
        else if (attribute == "COLOR_0" && model.accessors[accessorId].type == microgltf::Vec3)
            primitiveShaderFlags |= ShaderHasVec3Colors;
        else if (attribute == "COLOR_0" && model.accessors[accessorId].type == microgltf::Vec4)
            primitiveShaderFlags |= ShaderHasVec4Colors;
    }
    if (primitive.material >= 0)
    {
        const auto& material = model.materials[primitive.material];
        if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
            primitiveShaderFlags |= ShaderHasBaseColorMap;
        if (material.normalTexture.index >= 0)
            primitiveShaderFlags |= ShaderHasNormalMap;
    }

    return primitiveShaderFlags;
}
