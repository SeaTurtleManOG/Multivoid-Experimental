#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace coop::net {

template <std::size_t N>
inline void WireKeyFromString(const std::wstring& source, wchar_t (&dest)[N]) {
    std::size_t i = 0;
    for (; i + 1 < N && i < source.size(); ++i) dest[i] = source[i];
    dest[i] = L'\0';
}

template <std::size_t N>
inline std::wstring StringFromWireKey(const wchar_t (&source)[N]) {
    std::size_t n = 0;
    while (n < N && source[n] != L'\0') ++n;
    return std::wstring(source, source + n);
}

inline std::uint64_t FnvKey(const std::wstring& value) {
    std::uint64_t hash = 1469598103934665603ull;
    for (wchar_t ch : value) {
        hash ^= static_cast<std::uint64_t>(ch);
        hash *= 1099511628211ull;
    }
    return hash;
}

}  // namespace coop::net
