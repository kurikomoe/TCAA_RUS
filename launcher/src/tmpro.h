#pragma once

#include <iostream>
#include <format>
#include <MinHook.h>
#include <string>
#include <cstdint>
#include <map>

#include "game_def.h"
#include "consts.h"


extern std::map<std::wstring, System_String_o*> new_string_cache;

std::map<std::wstring, std::wstring> tmpro_data = {
    {L"Courtroom", L"法庭"},
};


using TMPro_TMP_Text__set_textT = void (void*, System_String_o*, void*);
intptr_t tgt_TMPro_TMP_Text__set_text = 0x906590;
TMPro_TMP_Text__set_textT* orig_TMPro_TMP_Text__set_text = nullptr;

void hook_TMPro_TMP_Text__set_text(void* This, System_String_o* text, void* method) {
    auto ss = utils::wstring(text);

    std::wcout << ss << std::endl;
    if (tmpro_data.contains(ss)) {
        auto new_name = tmpro_data[ss];

        text = utils::GetSystemString(new_name, text);
    }

    orig_TMPro_TMP_Text__set_text(This, text, method);

    return;
}

namespace TMPro {

    int init(DWORD base) {
        tgt_TMPro_TMP_Text__set_text += base;
        std::cout << std::format("TMPro_TMP_Text__set_text: {:#x}\n", tgt_TMPro_TMP_Text__set_text);
        if (MH_CreateHook((LPVOID)tgt_TMPro_TMP_Text__set_text, &hook_TMPro_TMP_Text__set_text, (LPVOID*)&orig_TMPro_TMP_Text__set_text) != MH_OK) {
            std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_TMPro_TMP_Text__set_text);
            return 1;
        }

        if (MH_EnableHook((LPVOID)tgt_TMPro_TMP_Text__set_text) != MH_OK) {
            std::cout << std::format("MH_EnableHook Failed\n");
            return 1;
        }

        return 0;
    }
}
