# libvotv adoption matrix

Audit pin: `modestimpala/libvotv@04bc63b3f98ba32edb064342b034c74d24fe2c81`.

## Decision boundary

**CLAIM `[UNKNOWN]`.** No project-level libvotv licence grant was established at the pin. No source may be copied or redistributed until legal review identifies an applicable grant.

**FALSIFIER.** A licence covering the intended use is produced and accepted.

**PROBE.** Inventory and read conventional licence/notice paths and all licence references in the complete seven-file tree.

**GAP.** A grant may exist outside the pin; this is not legal advice.

| ID | Component | Disposition | Reason and evidence strength |
|---|---|---|---|
| C01 | CMake interface target | REIMPLEMENT_CLEANLY | `[INFERRED]` Trivial include/link propagation adds UE4SS coupling and no unique behavior. |
| C02 | `FIntVector` / `FIntVector2D` PODs | REIMPLEMENT_CLEANLY | `[INFERRED]` Trivial tuples; validate ABI locally if needed. |
| C03 | generated property accessor macros | REJECT | `[SOURCE_PROVEN]` Getters default and setters no-op on reflection miss without a required error channel; `libvotv@04bc63b/include/StructUtil.hpp:31-159`. |
| C04 | game enum catalog | EXTRACT_SCHEMA_ONLY | `[INFERRED]` Useful candidate names/ordinals, but every ordinal needs independent target-cook validation. |
| C05 | fixed VotV struct layouts | EXTRACT_SCHEMA_ONLY | `[INFERRED]` Candidate field/offset schema only; literal padding/layout is build-coupled. |
| C06 | typed VotV UObject wrapper catalog | EXTRACT_SCHEMA_ONLY | `[INFERRED]` Candidate class/property names inherit C03 failure behavior if code is reused. |
| C07 | aggregate game snapshots | REJECT | `[INFERRED]` Aggregation can launder property misses into plausible values without validity bits. |
| C08 | function lookup | REFERENCE_ONLY | `[INFERRED]` Similar lookup already exists in Multivoid; no distinct safety contract. |
| C09 | reflected function invocation | REJECT | `[SOURCE_PROVEN]` Name existence is checked, but property class/size/flags are not validated before typed writes/reads; `libvotv@04bc63b/include/FunctionUtil.hpp:39-109`. |
| C10 | property get/set helpers | REFERENCE_ONLY | `[INFERRED]` Detectable null/false on miss but no lifetime/thread contract; existing ownership overlaps. |
| C11 | `FColor` helper | REIMPLEMENT_CLEANLY | `[SOURCE_PROVEN]` `FromHex` indexes input before checking length; `libvotv@04bc63b/include/CommonUtil.hpp:13-25`. |
| C12 | string conversion | REJECT | `[SOURCE_PROVEN]` Iterator widening/narrowing does not implement UTF-8 encoding/decoding; `libvotv@04bc63b/include/CommonUtil.hpp:36-45`. |
| C13 | background worker | REJECT | `[INFERRED]` Arbitrary closures lack a game-thread route and post-shutdown tasks can be accepted but not run. |
| C14 | exception wrapper | REFERENCE_ONLY | `[INFERRED]` Logs C++ exceptions but does not replace Multivoid's explicit game-thread/SEH policy. |
| C15 | `_malloca` parameter guard | REJECT | `[INFERRED]` Buffer lifetime depends on construction-frame/toolchain behavior and does not prove ownership. |
| C16 | hook/string parameter helper | REJECT | `[SOURCE_PROVEN]` Same-named properties are cast as `FName` without class validation; `libvotv@04bc63b/include/CommonUtil.hpp:162-186`. |
| C17 | serial-number mutator | REJECT | `[INFERRED]` Mutates GUObjectArray serial state without re-verifying the item still identifies the original object. |
| C18 | object lifetime tracker | REJECT | `[INFERRED]` The pin fixes dropped-deletion UAF paths, but address-only valid entries, notification loss, lock/UObject access and post-check races remain. |

**CLAIM `[SOURCE_PROVEN]`.** The package advertises C++17 but uses `std::jthread`, `std::stop_token`, and `std::atomic_ref`, while its interface target sets no C++20 feature requirement. Evidence: `libvotv@04bc63b/README.md:3-4`, `include/CommonUtil.hpp:47-87`, `include/ObjectLifetimeTracker.hpp:63-68`, `CMakeLists.txt:10-25`.

**FALSIFIER.** Those facilities are removed, polyfilled under C++17, or the target declares the necessary standard.

**PROBE.** Read the complete build file and search all standard-library facility uses.

**GAP.** No consumer build was run; exact toolchain acceptance is `UNKNOWN`.

**CLAIM `[INFERRED]`.** libvotv and Relay have related project provenance but no install/runtime dependency on each other at the pins. Relay's static package contains a same-named lifetime-tracker RTTI string, but exact code provenance/version is `UNKNOWN`.

**FALSIFIER.** A readable dependency declaration or implementation comparison establishes a direct packaged dependency or exact provenance.

**PROBE.** Compare complete manifests, trees and bounded binary strings.

**GAP.** Shared names and publisher identity are not source equivalence.

## Use in the chosen direction

Only schema candidates C04–C06 may inform independently validated names/ordinals/layout hypotheses. C01, C02 and C11 may be cleanly reimplemented only if a concrete need exists. No libvotv runtime dependency, copied implementation, silent accessor, universal wrapper generator or lifetime tracker is part of the contract-layer decision.

