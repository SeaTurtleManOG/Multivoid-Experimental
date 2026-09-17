#pragma once

// Shadow stub for coop/element/portable_identity.h.
//
// The real module derives a cross-peer name from an actor's UE reflection state (child-actor
// chain, RF_WasLoaded, the game Key). A QualificationActor has none of that, so it has NO
// portable identity, and "" is what the real contract returns in exactly that case: "an actor
// which is itself top-level, runtime-created and keyless". The channel therefore indexes these
// actors by their adapter key, which is what the harness's cases address them by.

#include <cstdint>
#include <string>

namespace coop::element {

inline std::wstring PortableIdentity(void* /*actor*/) { return std::wstring(); }
inline std::wstring PortableWireKey(void* /*actor*/) { return std::wstring(); }

}  // namespace coop::element
