#pragma once

#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>

namespace qualification_log {

inline std::vector<std::string>& Lines() {
    static std::vector<std::string> lines;
    return lines;
}

inline void Clear() { Lines().clear(); }

inline void Emit(const char* format, ...) {
    char buffer[4096]{};
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Lines().emplace_back(buffer);
}

inline bool Contains(const std::string& needle) {
    for (const auto& line : Lines()) {
        if (line.find(needle) != std::string::npos) return true;
    }
    return false;
}

}  // namespace qualification_log

#define UE_LOGI(...) ::qualification_log::Emit(__VA_ARGS__)
#define UE_LOGW(...) ::qualification_log::Emit(__VA_ARGS__)
#define UE_LOGE(...) ::qualification_log::Emit(__VA_ARGS__)
