#ifndef STRING_UNORDERED_MAP
#define STRING_UNORDERED_MAP

#include <string>
#include <string_view>
#include <unordered_map>

struct StringHash
{
    using hash_type = std::hash<std::string_view>;
    using is_transparent = void;

    std::size_t operator()(const char* str) const { return hash_type{}(str); }
    std::size_t operator()(std::string_view str) const { return hash_type{}(str); }
    std::size_t operator()(const std::string& str) const { return hash_type{}(str); }
};

/**
 * Allow for searching with a std::string_view in a std::unordered_map<std::string, T>
 */
template <
    typename Value,
    typename Allocator = std::allocator<std::pair<const std::string, Value>>>
using StringUnorderedMap = std::unordered_map<std::string, Value, StringHash, std::equal_to<>, Allocator>;

#endif
