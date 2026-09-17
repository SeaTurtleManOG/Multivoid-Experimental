#pragma once

#include <cstddef>
#include <cstdint>

namespace coop::element::scan_hub {

struct Consumer {
    const char* name;
    void* context;
    bool (*ensureResolved)();
    bool (*isInstance)(void*);
    void (*passBegin)(void*, bool);
    void (*match)(void*, void*);
    std::size_t (*passComplete)(void*, bool, std::uint32_t);
    int settleScans;
};

inline void Register(const Consumer&) {}

}  // namespace coop::element::scan_hub
