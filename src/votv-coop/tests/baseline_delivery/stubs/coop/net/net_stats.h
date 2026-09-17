#pragma once

#include <cstdint>

namespace coop::net::net_stats {
inline std::uint64_t qualificationSentBytes = 0;
inline void AddSent(std::uint32_t bytes) { qualificationSentBytes += bytes; }
}  // namespace coop::net::net_stats
