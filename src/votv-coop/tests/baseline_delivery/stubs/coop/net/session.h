#pragma once

#include "coop/net/protocol.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <vector>

namespace coop::net {

enum class Role : std::uint8_t { Host, Client };

struct QualificationSend {
    int slot = -1;
    std::uint32_t generation = 0;
    KeyedTogglePayload payload{};
    bool accepted = false;
};

class Session {
public:
    static constexpr int kSlots = 8;

    bool connectedValue = true;
    Role roleValue = Role::Host;
    std::array<std::uint32_t, kSlots> generations{};
    std::array<bool, kSlots> broadcastResults{};
    std::array<std::deque<bool>, kSlots> targetedResults;
    std::vector<QualificationSend> targetedSends;
    std::vector<QualificationSend> broadcastSends;

    bool connected() const { return connectedValue; }
    Role role() const { return roleValue; }

    std::uint32_t peerGenerationForSlot(int slot) const {
        return slot >= 0 && slot < kSlots ? generations[slot] : 0;
    }

    bool SendReliable(ReliableKind, const void* data, int size) {
        bool anySuccess = false;
        for (int slot = 1; slot < kSlots; ++slot) {
            if (generations[slot] == 0) continue;
            QualificationSend send{};
            send.slot = slot;
            send.generation = generations[slot];
            if (size == static_cast<int>(sizeof(send.payload))) {
                std::memcpy(&send.payload, data, sizeof(send.payload));
            }
            send.accepted = broadcastResults[slot];
            anySuccess = anySuccess || send.accepted;
            broadcastSends.push_back(send);
        }
        return anySuccess;
    }

    bool SendReliableToSlot(int slot, ReliableKind kind, const void* data, int size,
                            std::uint8_t senderSlot = 0) {
        return SendTargeted(slot, 0, kind, data, size, senderSlot);
    }

    bool SendReliableToSlotForGeneration(int slot, std::uint32_t generation,
                                         ReliableKind kind, const void* data, int size,
                                         std::uint8_t senderSlot = 0) {
        return SendTargeted(slot, generation, kind, data, size, senderSlot);
    }

private:
    bool SendTargeted(int slot, std::uint32_t expectedGeneration, ReliableKind,
                      const void* data, int size, std::uint8_t) {
        QualificationSend send{};
        send.slot = slot;
        send.generation = expectedGeneration;
        if (size == static_cast<int>(sizeof(send.payload))) {
            std::memcpy(&send.payload, data, sizeof(send.payload));
        }
        const bool generationMatches = slot >= 0 && slot < kSlots && generations[slot] != 0 &&
            (expectedGeneration == 0 || generations[slot] == expectedGeneration);
        if (generationMatches && !targetedResults[slot].empty()) {
            send.accepted = targetedResults[slot].front();
            targetedResults[slot].pop_front();
        }
        targetedSends.push_back(send);
        return send.accepted;
    }
};

}  // namespace coop::net
