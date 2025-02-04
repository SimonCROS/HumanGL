#include <fstream>
#include <sstream>

#include "ShaderProgram.h"
#include "MicroGLTF/Struct.h"

ShaderProgram::ShaderProgram(std::string&& vertCode, std::string&& fragCode)
    : m_vertCode(std::move(vertCode)), m_fragCode(std::move(fragCode))
{
}

auto ShaderProgram::Create(const std::string& vertPath, const std::string& fragPath)
    -> Expected<ShaderProgram, std::string>
{
    auto e_vertCode = tryGetShaderCode(vertPath);
    if (!e_vertCode)
        return Unexpected(std::move(e_vertCode).error());

    auto e_fragCode = tryGetShaderCode(fragPath);
    if (!e_fragCode)
        return Unexpected(std::move(e_fragCode).error());

    return Expected<ShaderProgram, std::string>{std::in_place, *std::move(e_vertCode), *std::move(e_fragCode)};
}

auto ShaderProgram::getProgram(const ShaderFlags flags) -> ShaderProgramInstance&
{
    auto it = programs.find(flags);
    if (it == programs.end())
    {
        throw std::exception(); // TODO exception
    }

    return it->second;
}

auto ShaderProgram::getProgram(const ShaderFlags flags) const -> const ShaderProgramInstance&
{
    auto it = programs.find(flags);
    if (it == programs.end())
    {
        throw std::exception(); // TODO exception
    }

    return it->second;
}

auto ShaderProgram::enableVariant(const ShaderFlags flags)
    -> Expected<std::reference_wrapper<ShaderProgramInstance>, std::string>
{
    {
        const auto it = programs.find(flags);
        if (it != programs.end())
            return it->second;
    }

    const std::string modifiedVertCode = getCodeWithFlags(m_vertCode, flags);
    const std::string modifiedFragCode = getCodeWithFlags(m_fragCode, flags);

    auto e_program = ShaderProgramInstance::Create(modifiedVertCode, modifiedFragCode);
    if (!e_program)
        return Unexpected(std::move(e_program).error());

    auto [it, inserted] = programs.try_emplace(flags, *std::move(e_program));

    if (!inserted)
        return Unexpected("A shader with the same id already exist");
    return it->second;
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

auto ShaderProgram::tryGetShaderCode(const std::string& path) -> Expected<std::string, std::string>
{
    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        std::stringstream stream;
        file.open(path.c_str());
        stream << file.rdbuf();
        file.close();

        return std::move(stream).str();
    }
    catch (std::ifstream::failure& e)
    {
        return Unexpected("Failed to get shader code at `" + path + "`: " + e.what());
    }
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
