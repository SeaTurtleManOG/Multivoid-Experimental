# Relay implementation confidence

## Bottom line

**CLAIM `[INFERRED]`.** Relay has a substantial documented and statically visible contract surface, but none of the behavior that would carry a Multivoid authority, save, inventory, transition or resync invariant is implementation-qualified at this pin. Broad migration must therefore treat those behaviors as `UNKNOWN`; selective evaluation may only promote the exact behavior exercised by an independently reviewed gate.

**FALSIFIER.** Corresponding readable implementation source or reproducible, independent black-box results establish the named invariants at the exact package/API/wire versions.

**PROBE.** Classify the published documentation and bounded static asset/binary observations by evidence strength; check whether any item source-proves or runtime-observes a core behavioral invariant.

**GAP.** Static artifacts were not executed, decompiled into publishable source, or treated as full behavior verification.

## Confidence by surface

| Surface | Strongest evidence | What is established | What remains unknown |
|---|---|---|---|
| Package and version labels | `SOURCE_PROVEN` | Manifest and inventory: `Relay@e5d3d59/manifest.json:1-8`, `RELAY-PUBLIC-SURFACE.md:5-13` | Native constants, future compatibility, resolver behavior |
| Blueprint/rule vocabulary | `ASSET_OBSERVED` | Names and serialized structures exist | Execution semantics, ordering, failure behavior |
| Native entry surface | `BINARY_OBSERVED` | PE/import/export/resource characteristics; a very small named export surface | Internal authority, state, security and recovery behavior |
| Player/session contract | `README_REPORTED` | What the documentation promises | Handshake, mismatch, authentication and lifecycle correctness |
| Identity/presence/authority | `README_REPORTED` | Separate concepts are documented | Collision, lease expiry, split-brain and fail-closed enforcement |
| Fields/events/transitions | `README_REPORTED` | Declarative usage is documented | Atomicity, exactly/at-most-once boundaries, replay and loss behavior |
| Join/resync/world fences | `README_REPORTED` | Staged concepts are documented | Native-save reconciliation, travel/rejoin ordering, convergence |
| Inventory/save/item replacement | `ASSET_OBSERVED` | Related assets and names exist | Ownership, loss/duplication prevention, persistence and rollback |
| Services documentation/config | `SOURCE_PROVEN` | Operator topology and configuration: `Relay@e5d3d59/services/README.md:42-81` | Runtime capacity, exposure, abuse resistance and operational reliability |
| Service binaries | `BINARY_OBSERVED` | Bounded PE/CLI/protocol strings in the exact artifacts | Reachability, correctness and operational reliability |

**CLAIM `[BINARY_OBSERVED]`.** Static inspection found only `start_mod` and `uninstall_mod` as named exports from the player `main.dll`; it did not expose a callable native C++ API sufficient to integrate Multivoid directly. Evidence: the PE export directory of `Relay@e5d3d59/mod/dlls/main.dll`.

**FALSIFIER.** A corrected export analysis or supported integration specification identifies a stable native API.

**PROBE.** Parse the PE export directory of the exact DLL and cross-check documented consumer surfaces.

**GAP.** A Blueprint or loader-mediated interface can exist without named PE exports. Whether Multivoid needs a gameplay pak or editable assets for such a bridge is `UNKNOWN`.

## Qualification boundary

The following are gates, not claims that Relay fails:

1. Prove exact package hashes and a supported install/upgrade path on all peers.
2. Resolve the project licence before redistribution, bundling, or derived use.
3. Qualify version mismatch and schema rejection as fail-closed.
4. Qualify player identity binding and authorization independently of displayed identity.
5. Qualify identity collision, presence ownership, lease expiry and rejoin.
6. Qualify authority transfer and split-brain rejection.
7. Qualify field/event/transition ordering, loss, duplication and replay.
8. Qualify native-save boot, native-actor bind/adopt, join seed and targeted resync.
9. Qualify inventory ownership, persistence and rollback with non-destructive fixtures.
10. Qualify world travel, teardown, reconnect and repeated-cycle stability.
11. Qualify native-C++ to Blueprint/rule integration without dual materializers.
12. Qualify two-mod dependency resolution and version coexistence.

**CLAIM `[UNKNOWN]`.** Until the relevant subset passes, neither the prospective pilot gates nor the existence of Relay artifacts establishes safe co-ownership of a Multivoid subsystem.

**FALSIFIER.** An independently reviewed gate packet passes with exact versions, role logs, positive/negative controls and rollback evidence.

**PROBE.** Apply the black-box qualification plan only to an authorized isolated environment; no such result is part of this audit.

**GAP.** This assessment cannot predict results of a future authorized run.
