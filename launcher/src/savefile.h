#pragma once

#include <MinHook.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstdint>
#include <map>

#include "game_def.h"
#include "helper.hpp"


namespace SaveFile {

    int init(DWORD base) {
        for (int i = 0; i < 5; i++) {
            Memory::Write(base + 0x1c340f + i, (char)0x90);
        }

        return 0;
    }
}
