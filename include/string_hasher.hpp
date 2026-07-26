#pragma once
#include <string>

namespace Hasher{

constexpr uint64_t stringview(std::string_view str) {
    uint64_t hash = 14695981039346656037ULL;
    for (char c : str) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
    }

};
