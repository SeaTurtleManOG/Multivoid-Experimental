// coop/props/drive_place_authorship.h -- does a client's place NOTE drive-payload authorship?
//
// A prop_drive_C keeps its recorded signal in `data_0` (ue_wrap/desk/drive_chain.h:61-62). The intent
// (PropDropIntentPayload, closed by a static_assert) has no field for it and the save record that
// follows (coop/props/prop_save_data.h) skips the drive lineage, which drive_sync owns, so the row
// rides only the DrivePayload lane, whose birth branch in coop/interactables/drive_sync.cpp
// emits for a client's FIRST-SIGHTED eid only after NoteLocalDriveBirth; unnoted, the host's respawn
// broadcasts a CDO-default (blank) row as truth. Gating the note on `freshBirth` (`!parked && (reel
// || module || drive)`) missed the pickup-then-place players perform (a hand pickup PARKS the key)
// and the container extract. Class alone decides because this runs after the drain's authoring gate:
// every entry is already an admitted client place with that same hazard. It cannot let a JOINER
// re-author the host's save-loaded drives (drive_sync.cpp's birth invariant): g_pending holds only this
// client's own FinishSpawn placements, the drain first drops eid-bound entries (every save-loaded or
// host-mirrored drive) and echo-marked ones, the note is one-shot and TTL-bounded (30 s), and the
// emit stays gated on `!RowIsDefault(row)`, so a blank disc still sends nothing.

#pragma once

namespace coop::prop_drop_intent {

// True when this client's admitted place should note local drive-payload authorship.
//
// PRECONDITION: the caller has already passed the drain's authoring gate, so this entry is a
// parked place, a whitelisted fresh birth, or a container extraction. Asked about an entry the
// gate would have dropped, this function still answers by class -- the restriction to admitted
// places is the caller's, exactly as the surrounding module does it elsewhere.
//
// `isDriveClass`  the placed actor is of the prop_drive lineage (drive_chain::IsDriveClass)
// `freshBirth`    the drain's unparked-whitelisted-birth term; deliberately unused, kept so the
//                 pre-fix policy stays expressible beside this one in the standalone test
constexpr bool NotesDrivePayloadAuthorship(bool isDriveClass, bool /*freshBirth*/) {
    return isDriveClass;
}

}  // namespace coop::prop_drop_intent
