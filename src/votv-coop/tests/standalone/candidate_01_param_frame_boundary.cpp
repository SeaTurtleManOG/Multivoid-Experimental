#include "ue_wrap/core/call.h"
#include "ue_wrap/core/log.h"
#include "ue_wrap/core/reflection.h"

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

namespace {

int g_dispatches = 0;
int g_errorLogs = 0;
float g_expected[3] = {1.0f, 2.0f, 3.0f};
bool g_payloadMatched = false;

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    return condition;
}

}  // namespace

namespace ue_wrap::log {

void Write(Level level, const char*, ...) {
    if (level == Level::Error) ++g_errorLogs;
}

}  // namespace ue_wrap::log

namespace ue_wrap::reflection {

int32_t FunctionFrameSize(void*) {
    return static_cast<int32_t>(sizeof(g_expected));
}

std::vector<ParamInfo> FunctionParams(void*) {
    return {{L"KnownVector", 0, static_cast<int32_t>(sizeof(g_expected)), 0}};
}

bool CallFunction(void*, void*, void* params) {
    ++g_dispatches;
    g_payloadMatched = params != nullptr &&
        std::memcmp(params, g_expected, sizeof(g_expected)) == 0;
    return true;
}

}  // namespace ue_wrap::reflection

int main() {
    bool ok = true;
    void* const function = reinterpret_cast<void*>(static_cast<uintptr_t>(0x101));
    void* const object = reinterpret_cast<void*>(static_cast<uintptr_t>(0x202));

    ue_wrap::ParamFrame control(function);
    ok &= Check(control.SetRaw(L"KnownVector", g_expected, sizeof(g_expected)),
                "positive control writes a reflected parameter");
    ok &= Check(ue_wrap::Call(object, control),
                "positive control dispatches a fully written frame");
    ok &= Check(g_dispatches == 1 && g_payloadMatched,
                "positive control reaches the dispatch stub with intended bytes");

    for (int attempt = 0; attempt < 3; ++attempt) {
        ue_wrap::ParamFrame failed(function);
        ok &= Check(!failed.SetRaw(L"NewScale3D", g_expected, sizeof(g_expected)),
                    "unknown named write reports failure");
        ok &= Check(!ue_wrap::Call(object, failed),
                    "a frame with a failed named write is refused");
    }

    ok &= Check(g_dispatches == 1,
                "failed frames never reach the dispatch stub");
    ok &= Check(g_errorLogs == 1,
                "identical write failure is diagnosed once");
    return ok ? 0 : 1;
}
