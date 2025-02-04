#ifndef SHADER_PROGRAM_VARIANTS_H
#define SHADER_PROGRAM_VARIANTS_H

#include <memory>
#include <unordered_map>
#include <string>
#include <string_view>

#include "Expected.h"
#include "ShaderProgramInstance.h"

enum ShaderFlags : unsigned char
{
    ShaderHasNone = 0,
    ShaderHasNormals = 1 << 0,
    ShaderHasTangents = 1 << 1,
    ShaderHasBaseColorMap = 1 << 2,
    ShaderHasMetalRoughnessMap = 1 << 3,
    ShaderHasNormalMap = 1 << 4,
    ShaderHasEmissiveMap = 1 << 5,
    ShaderHasVec3Colors = 1 << 6,
    ShaderHasVec4Colors = 1 << 7,
};

inline ShaderFlags operator~(const ShaderFlags a) { return static_cast<ShaderFlags>(~static_cast<int>(a)); }

inline ShaderFlags operator|(const ShaderFlags a, const ShaderFlags b)
{
    return static_cast<ShaderFlags>(static_cast<int>(a) | static_cast<int>(b));
}

inline ShaderFlags operator&(const ShaderFlags a, const ShaderFlags b)
{
    return static_cast<ShaderFlags>(static_cast<int>(a) & static_cast<int>(b));
}

inline ShaderFlags operator^(const ShaderFlags a, const ShaderFlags b)
{
    return static_cast<ShaderFlags>(static_cast<int>(a) ^ static_cast<int>(b));
}

inline ShaderFlags& operator|=(ShaderFlags& a, const ShaderFlags b)
{
    return reinterpret_cast<ShaderFlags&>(reinterpret_cast<int&>(a) |= static_cast<int>(b));
}

inline ShaderFlags& operator&=(ShaderFlags& a, const ShaderFlags b)
{
    return reinterpret_cast<ShaderFlags&>(reinterpret_cast<int&>(a) &= static_cast<int>(b));
}

inline ShaderFlags& operator^=(ShaderFlags& a, const ShaderFlags b)
{
    return reinterpret_cast<ShaderFlags&>(reinterpret_cast<int&>(a) ^= static_cast<int>(b));
}

class ShaderProgram
{
public:
    std::unordered_map<ShaderFlags, std::unique_ptr<ShaderProgramInstance>> programs;

    ShaderProgram() = delete;
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) = default; // TODO add destructor, so mark other as destroyed
    ShaderProgram(std::string&& vertCode, std::string&& fragCode);

    static auto Create(const std::string& vertPath, const std::string& fragPath) -> Expected<ShaderProgram, std::string>;

    auto getProgram(ShaderFlags flags) -> ShaderProgramInstance&;
    auto getProgram(ShaderFlags flags) const -> const ShaderProgramInstance&;
    auto enableVariant(ShaderFlags flags) -> Expected<std::reference_wrapper<ShaderProgramInstance>, std::string>;

private:
    std::string m_vertCode;
    std::string m_fragCode;

    static auto getCodeWithFlags(const std::string_view& code, ShaderFlags flags) -> std::string;
    static auto tryGetShaderCode(const std::string& path) -> Expected<std::string, std::string>;
};

#endif
