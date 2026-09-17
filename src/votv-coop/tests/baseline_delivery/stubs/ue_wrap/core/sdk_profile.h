#pragma once

// Shadow stub for ue_wrap/core/sdk_profile.h.
//
// The real header is the whole offset table for the game's SDK. The channel header reads one
// entry, and only inside the identity probe that coop::config::ResolveFlag keeps off in this
// harness: the UObject object-flags offset. The value is reproduced from the real table so the
// stub names the same field rather than an invented one.

#include <cstddef>

namespace ue_wrap::profile::off {

inline constexpr size_t UObject_ObjectFlags = 0x08;  // int32 EObjectFlags

}  // namespace ue_wrap::profile::off
