#pragma once

#include <iostream>
#include <format>
#include <MinHook.h>
#include <string>
#include <cstdint>
#include <atomic>

#include "game_def.h"
#include "consts.h"
#include "helper.hpp"


std::atomic_flag is_showing_chara_card;

// FIXME(kuriko): use NullOrWhitespace instead of this.
intptr_t StringLiteral_6996 = 0xeaf258;

// Some consts
const wchar_t* name_size = L"100%";
const wchar_t* name_voffset = L"0.09em";

const wchar_t* occupation_size = L"3.9em";
const wchar_t* occupation_voffset = L"0.215em";
const wchar_t* occupation_space = L"-1.5em";

void ModifyStringWorker(
    System_String_o** tmp,
    const wchar_t* size,
    const wchar_t* voffset,
    const wchar_t* space=nullptr
) {
    auto* system_string_ptr = *tmp;
    auto* ptr = &system_string_ptr->fields;
    std::wstring ss((wchar_t*)&ptr->_firstChar, ptr->_stringLength);
    std::wcout << L"Got occupation/name string: " << ss << std::endl;

    System_String_o* new_string = nullptr;
    if (space) {
        new_string = utils::GetSystemString(
            std::format(L"<space={}><size={}><voffset={}>{}</voffset>", space, size, voffset, ss),
            system_string_ptr);
    } else {
        new_string = utils::GetSystemString(
            std::format(L"<size={}><voffset={}>{}</voffset>", size, voffset, ss),
            system_string_ptr);
    }
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
        // Occupation
        ModifyStringWorker(
            &ret,
            occupation_size,
            occupation_voffset,
            occupation_space
        );
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

        // DisplayName
        ModifyStringWorker(
            &new_ret->fields.displayName,
            name_size,
            name_voffset
        );
        // Occupation
        ModifyStringWorker(
            &new_ret->fields.occupation,
            occupation_size,
            occupation_voffset,
            occupation_space
        );
        return new_ret;
    }

    return ret;
}

namespace Character {

    int init(DWORD base) {
        StringLiteral_6996 += base;

        if (utils::ApplyPatch(
            L"CharacterIntro__ShowChar_d__15__MoveNext",
            base,
            tgt_CharacterIntro__ShowChar_d__15__MoveNext,
            hook_CharacterIntro__ShowChar_d__15__MoveNext,
            orig_CharacterIntro__ShowChar_d__15__MoveNext) != 0) {
            return 1;
        }


        orig_GameState__GetOccupationOverride += base;
        if (utils::ApplyPatch(
            L"GameState__GetOccupationOverride",
            base,
            tgt_GameState__GetOccupationOverride,
            hook_GameState__GetOccupationOverride,
            next_GameState__GetOccupationOverride) != 0) {
            return 1;
        }

        if (utils::ApplyPatch(
            L"CharacterLibrary__GetCharacter",
            base,
            tgt_CharacterLibrary__GetCharacter,
            hook_CharacterLibrary__GetCharacter,
            orig_CharacterLibrary__GetCharacter) != 0) {
            return 1;
        }

        return 0;
    }
}
