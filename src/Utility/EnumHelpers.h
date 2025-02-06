//
// Created by Simon Cros on 2/6/25.
//

#ifndef ENUMHELPERS_H
#define ENUMHELPERS_H

#include <utility>

#define MAKE_FLAG_ENUM(Enum) \
inline Enum operator~(const Enum a) { return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(a)); } \
inline Enum operator|(const Enum a, const Enum b) { return static_cast<Enum>(static_cast<int>(a) | static_cast<int>(b)); } \
inline Enum operator&(const Enum a, const Enum b) { return static_cast<Enum>(static_cast<int>(a) & static_cast<int>(b)); } \
inline Enum operator^(const Enum a, const Enum b) { return static_cast<Enum>(static_cast<int>(a) ^ static_cast<int>(b)); } \
inline Enum& operator|=(Enum& a, const Enum b) { return reinterpret_cast<Enum&>(reinterpret_cast<int&>(a) |= static_cast<int>(b)); } \
inline Enum& operator&=(Enum& a, const Enum b) { return reinterpret_cast<Enum&>(reinterpret_cast<int&>(a) &= static_cast<int>(b)); } \
inline Enum& operator^=(Enum& a, const Enum b) { return reinterpret_cast<Enum&>(reinterpret_cast<int&>(a) ^= static_cast<int>(b)); }

#endif //ENUMHELPERS_H
