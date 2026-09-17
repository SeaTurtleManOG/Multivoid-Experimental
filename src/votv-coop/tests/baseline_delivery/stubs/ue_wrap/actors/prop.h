#pragma once

// Shadow stub for ue_wrap/actors/prop.h.
//
// The real header declares the prop wrapper over reflection::FName and pulls the reflection
// surface with it. The channel header uses two names from it, both inside the identity probe that
// coop::config::ResolveFlag keeps off in this harness: the parent prop's interactable key and its
// save key. A QualificationActor is not a prop, so it carries neither.

#include <string>

namespace ue_wrap::prop {

inline std::wstring GetInteractableKeyString(void* /*obj*/) { return std::wstring(); }
inline std::wstring GetActorSaveKeyString(void* /*obj*/) { return std::wstring(); }

}  // namespace ue_wrap::prop
