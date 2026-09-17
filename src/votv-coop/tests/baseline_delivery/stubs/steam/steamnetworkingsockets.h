#pragma once

#include <cstdint>

using int64 = std::int64_t;
using uint16 = std::uint16_t;
using HSteamNetConnection = std::uint32_t;
using HSteamNetPollGroup = std::uint32_t;

inline constexpr int k_EResultOK = 1;
inline constexpr int k_EResultInvalidState = 11;
inline constexpr int k_EResultLimitExceeded = 25;
inline constexpr int k_nSteamNetworkingSend_Reliable = 8;
inline constexpr int k_ESteamNetConnectionEnd_App_Generic = 1000;

struct SteamNetworkingMessage_t {
    void* m_pData = nullptr;
    std::uint32_t m_conn = 0;
    int m_nFlags = 0;
    uint16 m_idxLane = 0;
    int m_cbSize = 0;
    int64 m_nConnUserData = 0;

    void Release() {
        delete[] static_cast<std::uint8_t*>(m_pData);
        delete this;
    }
};

struct SteamNetConnectionRealTimeStatus_t {
    int m_cbPendingReliable = 0;
    int m_cbPendingUnreliable = 0;
    float m_flInBytesPerSec = 0.0f;
    float m_flOutBytesPerSec = 0.0f;
    float m_flInPacketsPerSec = 0.0f;
    float m_flOutPacketsPerSec = 0.0f;
    float m_flConnectionQualityLocal = 1.0f;
    float m_flConnectionQualityRemote = 1.0f;
    int m_nPing = 0;
    int m_nSendRateBytesPerSecond = 0;
    int m_cbSentUnackedReliable = 0;
    int64 m_usecQueueTime = 0;
};

namespace qualification_transport {
inline int64 sendResult = 1;
inline int pendingReliable = 0;
inline int pendingUnreliable = 0;
inline int sendCalls = 0;
inline bool allocate = true;

inline void Reset() {
    sendResult = 1;
    pendingReliable = 0;
    pendingUnreliable = 0;
    sendCalls = 0;
    allocate = true;
}
}  // namespace qualification_transport

class ISteamNetworkingUtils {
public:
    SteamNetworkingMessage_t* AllocateMessage(int size) {
        if (!qualification_transport::allocate) return nullptr;
        auto* message = new SteamNetworkingMessage_t;
        message->m_pData = new std::uint8_t[static_cast<std::size_t>(size)];
        message->m_cbSize = size;
        return message;
    }
};

class ISteamNetworkingSockets {
public:
    void RunCallbacks() {}

    bool CloseConnection(HSteamNetConnection, int, const char*, bool) { return true; }

    int ReceiveMessagesOnPollGroup(HSteamNetPollGroup, SteamNetworkingMessage_t**, int) {
        return 0;
    }

    int ReceiveMessagesOnConnection(HSteamNetConnection, SteamNetworkingMessage_t**, int) {
        return 0;
    }

    void SendMessages(int count, SteamNetworkingMessage_t** messages, int64* results,
                      bool deleteFailedMessages) {
        for (int i = 0; i < count; ++i) {
            ++qualification_transport::sendCalls;
            results[i] = qualification_transport::sendResult;
            if (deleteFailedMessages || results[i] >= 0) {
                delete[] static_cast<std::uint8_t*>(messages[i]->m_pData);
                delete messages[i];
                messages[i] = nullptr;
            }
        }
    }

    int GetConnectionRealTimeStatus(std::uint32_t, SteamNetConnectionRealTimeStatus_t* status,
                                    int, void*) {
        status->m_cbPendingReliable = qualification_transport::pendingReliable;
        status->m_cbPendingUnreliable = qualification_transport::pendingUnreliable;
        return k_EResultOK;
    }
};

inline ISteamNetworkingUtils* SteamNetworkingUtils() {
    static ISteamNetworkingUtils utils;
    return &utils;
}

inline ISteamNetworkingSockets* SteamNetworkingSockets() {
    static ISteamNetworkingSockets sockets;
    return &sockets;
}
