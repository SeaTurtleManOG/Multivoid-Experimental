#pragma once

namespace coop::config_registry::rows {
inline constexpr bool interactable_log = false;
}

namespace coop::config {
inline bool ResolveFlag(bool) { return false; }
}
