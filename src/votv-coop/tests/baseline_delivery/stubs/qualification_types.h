#pragma once

#include <cstdint>
#include <string>

struct QualificationActor {
    std::wstring name;
    std::wstring key;
    bool state = false;
    bool readOk = true;
    bool live = true;
    std::int32_t index = 0;
};
