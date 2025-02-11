#pragma once

#include <iostream>
#include <format>
#include <MinHook.h>
#include <string>
#include <cstdint>
#include <atomic>

#include "game_def.h"
#include "consts.h"


std::atomic_flag is_showing_chara_card;


intptr_t StringLiteral_6996 = 0xeaf258;

void ModifyStringWorker(System_String_o** tmp, const wchar_t* size, const wchar_t* voffset) {
    auto* system_string_ptr = *tmp;
    auto* ptr = &system_string_ptr->fields;
    std::wstring ss((wchar_t*)&ptr->_firstChar, ptr->_stringLength);
    std::wcout << L"Got occupation/name string: " << ss << std::endl;

    auto* new_string = utils::GetSystemString(
        std::format(L"<size={}><voffset={}>{}</voffset>", size, voffset, ss),
        system_string_ptr);
    *tmp = new_string;
};

using CharacterIntro__ShowChar_d__15__MoveNextT = bool(void*, void*);
intptr_t tgt_CharacterIntro__ShowChar_d__15__MoveNext = 0x1ff1b0;
CharacterIntro__ShowChar_d__15__MoveNextT* orig_CharacterIntro__ShowChar_d__15__MoveNext = nullptr;
bool hook_CharacterIntro__ShowChar_d__15__MoveNext(void* This, void* method) {
    is_showing_chara_card.test_and_set();
    auto ret = orig_CharacterIntro__ShowChar_d__15__MoveNext(This, method);
    is_showing_chara_card.clear();
    return ret;
}

using GameState__GetOccupationOverrideT = System_String_o*(void*, void*, void*);
intptr_t tgt_GameState__GetOccupationOverride = 0x1ff4b5;
intptr_t orig_GameState__GetOccupationOverride = 0x1be200;
void* next_GameState__GetOccupationOverride = nullptr;
System_String_o* new_GameState__GetOccupationOverride(void* This, void* char_name, void* method) {
    auto* ptr = (GameState__GetOccupationOverrideT*)orig_GameState__GetOccupationOverride;
    auto* ret = ptr(This, char_name, method);
    if ((intptr_t)ret != *(intptr_t*)StringLiteral_6996 && is_showing_chara_card.test()) {
        ModifyStringWorker(&ret, L"4em", L"0.23em");
        std::wstring ss((wchar_t*)&ret->fields._firstChar, ret->fields._stringLength);
        std::wcout << L"Override Occupation: " << ss << std::endl;
    }

    return ret;
}
void __declspec(naked) hook_GameState__GetOccupationOverride() {
    __asm {
        call new_GameState__GetOccupationOverride
        mov ebx, tgt_GameState__GetOccupationOverride
        add ebx, 0x5
        jmp ebx
    }
}

using CharacterLibrary__GetCharacterT = CharacterData_o* (void*, System_String_o*, void*);
intptr_t tgt_CharacterLibrary__GetCharacter = 0x1bab60;
CharacterLibrary__GetCharacterT* orig_CharacterLibrary__GetCharacter = nullptr;
CharacterData_o* hook_CharacterLibrary__GetCharacter(void* This, System_String_o* char_name, void* method) {
    auto ret = orig_CharacterLibrary__GetCharacter(This, char_name, method);

    if (is_showing_chara_card.test()) {
        // NOTE(kuriko): I know this will memory leak, but since CharacterCard is not quite often shown, I think it's fine.
        auto* new_ret = (CharacterData_o*)malloc(sizeof(CharacterData_o));
        memcpy(new_ret, ret, sizeof(CharacterData_o));

        ModifyStringWorker(&new_ret->fields.displayName, L"100%", L"0.09em");
        ModifyStringWorker(&new_ret->fields.occupation, L"4em", L"0.23em");
        return new_ret;
    }

    return ret;
}

namespace Character {

    int init(DWORD base) {
        StringLiteral_6996 += base;

        tgt_CharacterIntro__ShowChar_d__15__MoveNext += base;
        std::cout << std::format("CharacterIntro__ShowChar_d__15__MoveNext: {:#x}\n", tgt_CharacterIntro__ShowChar_d__15__MoveNext);
        if (MH_CreateHook((LPVOID)tgt_CharacterIntro__ShowChar_d__15__MoveNext, &hook_CharacterIntro__ShowChar_d__15__MoveNext, (LPVOID*)&orig_CharacterIntro__ShowChar_d__15__MoveNext) != MH_OK) {
            std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_CharacterIntro__ShowChar_d__15__MoveNext);
            return 1;
        }

        if (MH_EnableHook((LPVOID)tgt_CharacterIntro__ShowChar_d__15__MoveNext) != MH_OK) {
            std::cout << std::format("MH_EnableHook Failed\n");
            return 1;
        }


        orig_GameState__GetOccupationOverride += base;
        tgt_GameState__GetOccupationOverride += base;
        std::cout << std::format("GameState__GetOccupationOverride: {:#x}\n", tgt_GameState__GetOccupationOverride);
        if (MH_CreateHook(
                (LPVOID)tgt_GameState__GetOccupationOverride,
                &hook_GameState__GetOccupationOverride,
                (LPVOID*)&next_GameState__GetOccupationOverride) != MH_OK) {
            std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_GameState__GetOccupationOverride);
            return 1;
        }

        if (MH_EnableHook((LPVOID)tgt_GameState__GetOccupationOverride) != MH_OK) {
            std::cout << std::format("MH_EnableHook Failed\n");
            return 1;
        }


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
