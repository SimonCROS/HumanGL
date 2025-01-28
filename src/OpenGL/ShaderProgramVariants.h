#ifndef SHADER_PROGRAM_VARIANTS_H
#define SHADER_PROGRAM_VARIANTS_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <string_view>

#include "Expected.h"
#include "ShaderProgram.h"

enum ShaderFlags : unsigned char
{
    ShaderHasNone = 0,
    ShaderHasNormals = 1 << 0,
    ShaderHasTangents = 1 << 1,
    ShaderHasBaseColorMap = 1 << 2,
    ShaderHasMetalRoughnessMap = 1 << 3,
    ShaderHasNormalMap = 1 << 4,
    ShaderHasEmissiveMap = 1 << 5,
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

class ShaderProgramVariants
{
public:
    std::unordered_map<ShaderFlags, ShaderProgram> programs;

    ShaderProgramVariants() = delete;
    ShaderProgramVariants(const ShaderProgramVariants&) = delete;
    ShaderProgramVariants(std::string&& vertCode, std::string&& fragCode);

    static auto Create(const std::string& vertPath, const std::string& fragPath) -> Expected<ShaderProgramVariants, std::string>;

    void destroy();

    ShaderProgram& getProgram(ShaderFlags flags);
    bool enableVariant(ShaderFlags flags);
    bool enableVariants(const std::unordered_set<ShaderFlags>& flags);

private:
    std::string m_vertCode;
    std::string m_fragCode;

    static std::string getCodeWithFlags(const std::string_view& code, ShaderFlags flags);
};

#endif
