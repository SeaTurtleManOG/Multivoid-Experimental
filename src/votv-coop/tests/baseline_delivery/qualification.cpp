#include "qualification_types.h"

#include "coop/interactables/interactable_channel.h"
#include "coop/net/send_backlog.h"
#include "steam/steamnetworkingsockets.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

namespace {

using coop::interactable_sync::Channel;
using coop::net::KeyedTogglePayload;
using coop::net::Session;

bool EnsureResolved() { return true; }
bool IsInstance(void*) { return true; }
std::wstring GetKey(void* object) {
    return static_cast<QualificationActor*>(object)->key;
}

bool ReadState(void* object, bool& on) {
    auto* actor = static_cast<QualificationActor*>(object);
    on = actor->state;
    return actor->readOk;
}

bool ApplyState(void* object, bool on) {
    static_cast<QualificationActor*>(object)->state = on;
    return true;
}

const coop::interactable_sync::Adapter kAdapter{
    "qualification",
    coop::net::ReliableKind::QualificationToggle,
    &EnsureResolved,
    &IsInstance,
    &GetKey,
    &ReadState,
    &ApplyState,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    false,
    nullptr,
};

[[noreturn]] void Fail(const char* message) {
    std::cerr << "FAIL " << message << '\n';
    std::exit(1);
}

void Require(bool condition, const char* message) {
    if (!condition) Fail(message);
}

QualificationActor MakeActor(const wchar_t* key, bool state, int index) {
    return QualificationActor{L"QualificationActor", key, state, true, true, index};
}

void Seed(Channel& channel, QualificationActor& actor) {
    channel.HubPassBegin(true);
    channel.HubMatch(&actor);
    channel.HubPassComplete(true, ue_wrap::world_identity::Generation());
}

void QueueResult(Session& session, int slot, bool result) {
    session.targetedResults[slot].push_back(result);
}

int CountTargeted(const Session& session, int slot) {
    int count = 0;
    for (const auto& send : session.targetedSends) {
        if (send.slot == slot) ++count;
    }
    return count;
}

const coop::net::QualificationSend& LastTargeted(const Session& session) {
    Require(!session.targetedSends.empty(), "no targeted send was captured");
    return session.targetedSends.back();
}

void CaseBackpressurePositiveControl() {
    qualification_transport::Reset();
    qualification_transport::sendResult = -k_EResultLimitExceeded;
    coop::net::SendBacklog backlog;
    const std::uint8_t wire[]{1, 2, 3, 4, 5};

    const bool accepted = backlog.SendOrQueue(1, 1, 0x1234u, wire, sizeof(wire));
    Require(accepted, "LimitExceeded did not return true");
    Require(backlog.DepthBytes(1) == sizeof(wire), "LimitExceeded did not queue the packet");

    qualification_transport::sendResult = 9;
    backlog.Drain(1, 0x1234u,
                  coop::net::SendBacklog::kReserve + static_cast<int>(sizeof(wire)) + 1);
    Require(backlog.DepthBytes(1) == 0, "queued backpressure packet did not drain");
    std::cout << "positive_backpressure: queued_then_drained result=pass\n";
}

void CaseSteadyBankingPositiveControl() {
    auto actor = MakeActor(L"steady-control", false, 1);
    Session session;
    session.generations[1] = 10;
    Channel channel(kAdapter);
    channel.SetSession(&session);
    Seed(channel, actor);

    channel.PollAndBroadcast();
    Require(session.broadcastSends.empty(), "first steady poll did not prime silently");

    actor.state = true;
    session.broadcastResults[1] = false;
    channel.PollAndBroadcast();
    channel.PollAndBroadcast();
    Require(session.broadcastSends.size() == 2, "failed steady sends were not retried");

    session.broadcastResults[1] = true;
    channel.PollAndBroadcast();
    channel.PollAndBroadcast();
    Require(session.broadcastSends.size() == 3, "successful steady send was not banked");
    std::cout << "positive_steady: false_false_true_then_quiet result=pass\n";
}

void CaseRetryPersistsUntilTargetAccepts() {
    auto actor = MakeActor(L"retry", true, 2);
    Session session;
    session.generations[1] = 101;
    QueueResult(session, 1, false);  // connect snapshot refusal
    QueueResult(session, 1, false);  // first pending retry refusal
    QueueResult(session, 1, true);   // later pending retry acceptance
    Channel channel(kAdapter);
    channel.SetSession(&session);
    Seed(channel, actor);

    channel.QueueConnectBroadcastForSlot(1);
    channel.Tick();
    Require(CountTargeted(session, 1) == 2,
            "failed connect row was not retried on the first retry tick");
    Require(!LastTargeted(session).accepted, "scripted failed retry unexpectedly succeeded");

    std::this_thread::sleep_for(coop::interactable_sync::kRetryRebuildThrottle +
                                std::chrono::milliseconds(100));
    channel.Tick();
    Require(CountTargeted(session, 1) == 3,
            "pending row did not survive a failed retry until target acceptance");
    Require(LastTargeted(session).accepted, "final targeted retry was not accepted");
    channel.PollAndBroadcast();
    Require(session.broadcastSends.empty(), "successful targeted retry was not banked");
    std::cout << "retry: refused_refused_accepted_then_quiet result=pass\n";
}

void CaseFanoutCannotEraseTargetedRetry() {
    auto actor = MakeActor(L"fanout", true, 3);
    Session session;
    session.generations[1] = 201;
    session.generations[2] = 202;
    session.broadcastResults[1] = false;
    session.broadcastResults[2] = true;
    QueueResult(session, 1, false);
    QueueResult(session, 1, true);
    Channel channel(kAdapter);
    channel.SetSession(&session);
    Seed(channel, actor);
    channel.PreUpdateLastKnown(actor.key, false);

    channel.QueueConnectBroadcastForSlot(1);
    channel.PollAndBroadcast();
    Require(session.broadcastSends.size() == 2, "fan-out control did not attempt both peers");
    Require(!session.broadcastSends[0].accepted && session.broadcastSends[1].accepted,
            "fan-out control did not model missed target plus successful other peer");

    channel.Tick();
    Require(CountTargeted(session, 1) == 2,
            "another peer's fan-out success erased the original target's retry");
    Require(LastTargeted(session).accepted, "original target did not accept its retained retry");
    std::cout << "fanout: target_missed_other_succeeded_target_retried result=pass\n";
}

void CaseRetryReadsCurrentActorState() {
    auto actor = MakeActor(L"current-state", true, 4);
    Session session;
    session.generations[1] = 301;
    QueueResult(session, 1, false);
    QueueResult(session, 1, true);
    Channel channel(kAdapter);
    channel.SetSession(&session);
    Seed(channel, actor);
    channel.PreUpdateLastKnown(actor.key, true);

    channel.QueueConnectBroadcastForSlot(1);
    actor.state = false;
    channel.Tick();
    Require(CountTargeted(session, 1) == 2, "current-state case did not retry");
    Require(LastTargeted(session).payload.action == 0,
            "pending retry replayed stale snapshot state instead of current actor state");
    Require(session.broadcastSends.empty(),
            "successful current-state retry was not banked before steady polling");
    std::cout << "current_state: queued_ON_retried_OFF result=pass\n";
}

void CaseSlotReuseCancelsPending() {
    auto actor = MakeActor(L"slot-reuse", true, 5);
    Session session;
    session.generations[1] = 401;
    QueueResult(session, 1, false);
    QueueResult(session, 1, true);
    Channel channel(kAdapter);
    channel.SetSession(&session);
    Seed(channel, actor);

    channel.QueueConnectBroadcastForSlot(1);
    session.generations[1] = 402;
    channel.Tick();
    Require(CountTargeted(session, 1) == 1,
            "pending row was delivered to a different generation in the recycled slot");
    std::cout << "slot_reuse: generation_401_to_402_cancelled result=pass\n";
}

void CaseSuccessfulSnapshotBanks() {
    auto actor = MakeActor(L"successful-bank", true, 6);
    Session session;
    session.generations[1] = 501;
    QueueResult(session, 1, true);
    Channel channel(kAdapter);
    channel.SetSession(&session);
    Seed(channel, actor);
    channel.PreUpdateLastKnown(actor.key, false);

    channel.QueueConnectBroadcastForSlot(1);
    channel.PollAndBroadcast();
    Require(session.broadcastSends.empty(),
            "successful connect snapshot did not bank and caused a double broadcast");
    std::cout << "success_bank: accepted_snapshot_then_quiet result=pass\n";
}

void CasePeerLeftAndDisconnectCancelPending() {
    {
        auto actor = MakeActor(L"peer-left", true, 7);
        Session session;
        session.generations[1] = 601;
        QueueResult(session, 1, false);
        QueueResult(session, 1, true);
        Channel channel(kAdapter);
        channel.SetSession(&session);
        Seed(channel, actor);
        channel.QueueConnectBroadcastForSlot(1);
        channel.OnPeerLeft(1);
        channel.Tick();
        Require(CountTargeted(session, 1) == 1,
                "OnPeerLeft did not cancel the slot's pending row");
    }
    {
        auto actor = MakeActor(L"disconnect", true, 8);
        Session session;
        session.generations[1] = 602;
        QueueResult(session, 1, false);
        QueueResult(session, 1, true);
        Channel channel(kAdapter);
        channel.SetSession(&session);
        Seed(channel, actor);
        channel.QueueConnectBroadcastForSlot(1);
        channel.OnDisconnect();
        channel.Tick();
        Require(CountTargeted(session, 1) == 1,
                "OnDisconnect did not cancel pending delivery rows");
    }
    std::cout << "lifecycle: peer_left_and_disconnect_cancelled result=pass\n";
}

void CaseMissingActorCancelsPending() {
    auto actor = MakeActor(L"missing-actor", true, 9);
    Session session;
    session.generations[1] = 701;
    QueueResult(session, 1, false);
    QueueResult(session, 1, true);
    Channel channel(kAdapter);
    channel.SetSession(&session);
    Seed(channel, actor);

    channel.QueueConnectBroadcastForSlot(1);
    actor.live = false;
    channel.Tick();
    actor.live = true;
    std::this_thread::sleep_for(coop::interactable_sync::kRetryRebuildThrottle +
                                std::chrono::milliseconds(100));
    channel.Tick();
    Require(CountTargeted(session, 1) == 1,
            "row for a missing actor was retained and later delivered");
    std::cout << "missing_actor: pending_cancelled result=pass\n";
}

void CaseSnapshotAccounting() {
    qualification_log::Clear();
    auto actor = MakeActor(L"accounting", true, 10);
    Session session;
    session.generations[1] = 801;
    QueueResult(session, 1, false);
    Channel channel(kAdapter);
    channel.SetSession(&session);
    Seed(channel, actor);

    channel.QueueConnectBroadcastForSlot(1);
    Require(qualification_log::Contains(
                "attempted=1 accepted=0 failed=1 pending=1"),
            "snapshot log did not report truthful attempted/accepted/failed/pending counts");
    Require(!qualification_log::Contains("sent 1 full state(s)"),
            "snapshot log still labels a refused invocation as sent");
    std::cout << "accounting: attempted_1_accepted_0_failed_1_pending_1 result=pass\n";
}

void RunNamed(const std::string& name) {
    if (name == "retry") return CaseRetryPersistsUntilTargetAccepts();
    if (name == "fanout") return CaseFanoutCannotEraseTargetedRetry();
    if (name == "current-state") return CaseRetryReadsCurrentActorState();
    if (name == "slot-reuse") return CaseSlotReuseCancelsPending();
    if (name == "success-bank") return CaseSuccessfulSnapshotBanks();
    if (name == "lifecycle") return CasePeerLeftAndDisconnectCancelPending();
    if (name == "missing-actor") return CaseMissingActorCancelsPending();
    if (name == "accounting") return CaseSnapshotAccounting();
    Fail("unknown case name");
}

}  // namespace

int main(int argc, char** argv) {
    CaseBackpressurePositiveControl();
    CaseSteadyBankingPositiveControl();
    if (argc == 3 && std::string(argv[1]) == "--case") {
        RunNamed(argv[2]);
    } else if (argc == 1) {
        CaseRetryPersistsUntilTargetAccepts();
        CaseFanoutCannotEraseTargetedRetry();
        CaseRetryReadsCurrentActorState();
        CaseSlotReuseCancelsPending();
        CaseSuccessfulSnapshotBanks();
        CasePeerLeftAndDisconnectCancelPending();
        CaseMissingActorCancelsPending();
        CaseSnapshotAccounting();
    } else {
        Fail("usage: qualification.exe [--case NAME]");
    }
    std::cout << "ALL_REQUESTED_CASES_PASS\n";
    return 0;
}
