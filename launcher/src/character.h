#pragma once

#include <iostream>
#include <format>
#include <MinHook.h>
#include <string>
#include <cstdint>
#include <map>

#include "game_def.h"
#include "consts.h"


using CharacterLibrary__GetCharacterT = CharacterData_o* (void*, System_String_o*, void*);
intptr_t tgt_CharacterLibrary__GetCharacter = 0x1bab60;
CharacterLibrary__GetCharacterT* orig_CharacterLibrary__GetCharacter = nullptr;
CharacterData_o* hook_CharacterLibrary__GetCharacter(void* This, System_String_o* char_name, void* method) {
    auto ret = orig_CharacterLibrary__GetCharacter(This, char_name, method);
    auto ptr = &ret->fields.description->fields;
    std::wstring ss((wchar_t*)&ptr->_firstChar, ptr->_stringLength);
    // MessageBoxW(NULL, ss.c_str(), L"Hooked", MB_OK);
    return ret;
}

namespace Character {

    int init(DWORD base) {
        tgt_CharacterLibrary__GetCharacter += base;
        std::cout << std::format("CharacterLibrary__GetCharacter: {:#x}\n", tgt_CharacterLibrary__GetCharacter);
        if (MH_CreateHook((LPVOID)tgt_CharacterLibrary__GetCharacter, &hook_CharacterLibrary__GetCharacter, (LPVOID*)&orig_CharacterLibrary__GetCharacter) != MH_OK) {
            std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_CharacterLibrary__GetCharacter);
            return 1;
        }

        if (MH_EnableHook((LPVOID)tgt_CharacterLibrary__GetCharacter) != MH_OK) {
            std::cout << std::format("MH_EnableHook Failed\n");
            return 1;
        }

        return 0;
    }
}
