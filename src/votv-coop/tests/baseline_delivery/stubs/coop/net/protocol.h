#pragma once

#include <cstdint>

namespace coop::net {

enum class ReliableKind : std::uint8_t {
    QualificationToggle = 1,
};

struct KeyedTogglePayload {
    wchar_t key[64]{};
    std::uint8_t action = 0;
};

}  // namespace coop::net
