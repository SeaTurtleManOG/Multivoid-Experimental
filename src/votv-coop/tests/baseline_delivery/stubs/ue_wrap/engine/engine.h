#pragma once

// Shadow stub for ue_wrap/engine/engine.h.
//
// The real header is the umbrella over every engine domain and reaches ue_wrap/core/types.h ->
// cached_obj_ref.h, which needs the full reflection surface. The channel header uses three names
// from it, all inside the identity probe, which coop::config::ResolveFlag keeps off in this
// harness: FVector, TryGetActorLocation and ParentActorOf. FVector's layout is reproduced from
// ue_wrap/core/types.h and the same static_assert is kept, so a layout change upstream is caught
// here rather than silently diverging.

#include <string>

namespace ue_wrap {

struct FVector {
    float X = 0.f, Y = 0.f, Z = 0.f;
};
static_assert(sizeof(FVector) == 12, "FVector layout");

}  // namespace ue_wrap

namespace ue_wrap::engine {

// No engine world behind a QualificationActor: the location read fails and reports it, which is
// the real function's contract for an actor whose root component cannot be read.
inline bool TryGetActorLocation(void* /*actor*/, FVector& out) {
    out = FVector{};
    return false;
}

// A QualificationActor is never a child actor, so it has no parent and no owning component name.
inline void* ParentActorOf(void* /*actor*/, std::wstring* outComponentName = nullptr) {
    if (outComponentName) outComponentName->clear();
    return nullptr;
}

}  // namespace ue_wrap::engine
