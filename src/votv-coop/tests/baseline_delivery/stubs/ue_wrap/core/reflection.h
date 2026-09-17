#pragma once

#include "qualification_types.h"

#include <cstdint>
#include <string>

namespace ue_wrap::reflection {

inline std::wstring NameOf(void* object) {
    return static_cast<QualificationActor*>(object)->name;
}

inline std::wstring ToString(const std::wstring& value) { return value; }
inline bool IsLive(void* object) {
    return object && static_cast<QualificationActor*>(object)->live;
}

inline std::int32_t InternalIndexOf(void* object) {
    return static_cast<QualificationActor*>(object)->index;
}

inline bool IsLiveByIndex(void* object, std::int32_t index) {
    return object && static_cast<QualificationActor*>(object)->live &&
        static_cast<QualificationActor*>(object)->index == index;
}

// The identity probe's two reads. The probe is compiled but not run here: this harness leaves
// coop::config::ResolveFlag false, so ProbeLog() never opens the block. A QualificationActor is a
// bare struct with no Outer, and its class is itself.
inline void* OuterOf(void* /*object*/) { return nullptr; }

inline std::wstring ClassNameOf(void* object) {
    return object ? std::wstring(L"QualificationActor") : std::wstring();
}

}  // namespace ue_wrap::reflection
