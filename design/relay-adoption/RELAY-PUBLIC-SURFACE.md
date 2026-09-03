# Relay public surface at the audit pin

This is a static, public-artifact description of `modestimpala/Relay@e5d3d59c42e8aca7f38e79b77145d6d46de770d8`. Nothing in this assessment executed Relay.

## Distribution and dependency shape

**CLAIM `[SOURCE_PROVEN]`.** The manifest declares package version `0.8.0` and a dependency on `Thunderstore-unreal_shimloader-1.1.7`; the documentation names `main.dll` and `Relay.pak` as the two parts of the player build. Evidence: `Relay@e5d3d59/manifest.json:1-8`, `README.md:97-114`.

**CLAIM `[ASSET_OBSERVED]`.** A complete static census of the pinned public tree contains exactly 75 files and 12,917,877 bytes. The measured player payload is `main.dll`, `libsodium.dll`, and `Relay.pak`, totalling 3,883,723 bytes. The optional operator payload contains rendezvous and relay executables plus a byte-identical libsodium DLL.

**FALSIFIER.** A complete census of the same pin has a different path set, size, manifest version, dependency declaration, player payload or service payload.

**PROBE.** Enumerate, hash and classify every path in the exact export; parse the manifest; sum the three player components and compare the two libsodium files.

**GAP.** File presence does not establish that a package installs, loads, or interoperates.

**CLAIM `[README_REPORTED]`.** Relay documents that every participating peer needs the same Relay DLL/pak, aligned gameplay mod, rule profile, API version and network version. It describes optional UDP rendezvous and relay-fallback services rather than mandatory services for every session. Evidence: `Relay@e5d3d59/README.md:43-61,97-114,147-183,305-319,400-411`.

**FALSIFIER.** Readable implementation or controlled runtime evidence at this pin establishes a different peer/package or service contract.

**PROBE.** Read the named documentation ranges and operator service documentation.

**GAP.** The package was not installed or run; actual load order, dependency resolution, mismatch behavior, and fallback need remain unqualified.

## Public API and rule surface

**CLAIM `[README_REPORTED]`.** Documentation separates discovery, stable network identity, presence, authority, durable fields, directed events, transitions, join state and targeted resynchronization. It reports core API `3`, network protocol `19`, and rules schema `relay.rules.v1`. Evidence: `Relay@e5d3d59/README.md:109-111`, `Relay@e5d3d59/README.md:305-567`, `Relay@e5d3d59/README.md:405`.

**FALSIFIER.** Source or independent black-box evidence shows that one of those documented surfaces is absent or materially different.

**PROBE.** Extract documented API/rule/version declarations and compare them to the static asset and binary claim ledger.

**GAP.** The native implementation is not readable, and bounded asset/binary observations do not verify the documented semantics. Agreement between documented API/network constants and the live native return/handshake behavior is `UNKNOWN`.

**CLAIM `[ASSET_OBSERVED]`.** Fifty-four editor assets expose names consistent with rule/schema, authority, lease, presence, field, event, resync, world-fence, save-transfer, item-replacement, inventory, hook and grab concepts. Their package headers identify UE 4.27-era serialization, and the pak footer/index is structurally valid. Evidence: the complete `Relay@e5d3d59/editor` tree and `pak/Relay.pak`.

**FALSIFIER.** A corrected parser shows those names are absent or the package structures do not validate.

**PROBE.** Parse bounded asset name tables and package headers; validate the pak index hash without loading an asset.

**GAP.** Names and structures do not prove control flow, ordering, atomicity, rejection, recovery, or runtime safety.

## Source, licence, maturity and authenticity

**CLAIM `[SOURCE_PROVEN]`.** The public tree contains documentation, manifests, cooked/editor artifacts, binaries, service configuration and third-party licence texts, but its positive-controlled native-source extension census contains zero Relay native implementation files. Evidence: the complete tree at `Relay@e5d3d59`.

**FALSIFIER.** Corresponding native source/build instructions are identified at the pin.

**PROBE.** Demonstrate the source-extension census on a known source tree, then apply it to the full Relay pin.

**GAP.** Absence of readable source is not evidence that a documented mechanism is false.

**CLAIM `[BINARY_OBSERVED]`.** `mod/dlls/main.dll` and `pak/Relay.pak` have no entries in the published service checksum list. The player `mod/dlls/libsodium.dll` is byte-identical to `services/libsodium.dll` at SHA-256 `F656AEB789BFC3A2AC587FAE1D7CFE278BBE8CE73F28DA73B4D08282EA8F9FE3`, which matches the published `libsodium.dll` entry. Evidence: `Relay@e5d3d59/services/SHA256SUMS.txt:1-6`; bounded SHA-256 comparison of the two pinned libsodium files.

**FALSIFIER.** Either unlisted player filename appears in the published checksum list, the two libsodium byte streams differ, or their computed digest differs from the published entry.

**PROBE.** Parse every published checksum filename, positively confirm the `libsodium.dll` entry, hash both pinned libsodium files, and compare the computed digests and bytes.

**GAP.** A checksum match does not authenticate the distribution channel or establish the project's licence. Authenticity for `main.dll`, `Relay.pak`, and the package as a whole remains `UNKNOWN`.

**CLAIM `[UNKNOWN]`.** No Relay project licence was established for modification, redistribution, or derived use of Relay's own DLL, pak, assets or services. The included ENet and libsodium notices cover those third-party components, not the project as a whole.

**FALSIFIER.** An applicable grant covering the intended dependency and redistribution use is produced and accepted by legal review.

**PROBE.** Inventory conventional licence/notice paths and read all licence references in the exact export and repository tree.

**GAP.** A grant may exist outside the pinned public material; this is not legal advice.

**CLAIM `[SOURCE_PROVEN]`.** The pinned public history has one commit and exposes no repository test suite or CI result in the audited tree. Evidence: the reachable history and complete tracked path set at `Relay@e5d3d59`.

**FALSIFIER.** Additional reachable history or a test/CI artifact exists at the same pin.

**PROBE.** Enumerate reachable commits, tracked paths, conventional CI locations and test-bearing paths.

**GAP.** This is a maturity/reproducibility observation only. It is not an argument based on novelty or number of releases, and it does not show the software is defective.

## Reconciled byte measurements

**CLAIM `[BINARY_OBSERVED]`.** The published list names exactly six service payloads. Its three binary payloads match their listed SHA-256 values byte-for-byte; the three checksum-listed text payloads (`libsodium-LICENSE.txt`, `ENet-LICENSE.txt`, and `README.md`) match their published Git-blob checksums after the export's CRLF line endings are normalized to LF. This six-entry checksum reconciliation is not a conflicting-package observation. Evidence: `Relay@e5d3d59/services/SHA256SUMS.txt:1-6` and the six corresponding pinned service payloads.

**FALSIFIER.** The checksum list has other than six entries, any of its three binary payloads fails a raw-byte checksum, or any of its three text payloads fails its checksum after CRLF-to-LF normalization.

**PROBE.** Parse only the six checksum-list entries; hash the three listed binaries as raw bytes and the three listed text payloads after CRLF-to-LF normalization.

**GAP.** The checksum file is not one of its own entries. This reconciliation does not authenticate `main.dll` or `Relay.pak`; the matching libsodium checksum alone also does not authenticate its distribution channel or the package as a whole.

**CLAIM `[BINARY_OBSERVED]`.** Separately, a complete comparison of all 75 export paths with the corresponding Git blobs has eleven raw-byte mismatches, all of which normalize to equality through CRLF-to-LF conversion: four under `services` and seven elsewhere.

**FALSIFIER.** The exact 75-path comparison yields a different raw-mismatch count or any mismatch remains after CRLF-to-LF normalization.

**PROBE.** Enumerate all 75 paths at the pin, compare each export byte stream with its corresponding Git blob, then normalize every raw mismatch and count the remaining differences.

**GAP.** Line-ending equality does not establish runtime behavior, distribution authenticity, or package identity beyond the exact compared bytes.
