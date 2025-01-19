#pragma once

#include <iostream>
#include <format>
#include <MinHook.h>
#include <string>
#include <cstdint>
#include <map>

#include "game_def.h"
#include "consts.h"


std::map<std::wstring, std::wstring> enum_string_map = {
    {L"Autopsy", L"尸检报告"},
    {L"Compendium", L"法术汇编"},
    {L"Testimony", L"证词"},
    {L"Contract", L"契约"},
    {L"Map", L"地图"},

    {L"Evocation", L"塑能术"},
    {L"Transmutation", L"形变术"},
    {L"Conjuration", L"召唤术"},
    {L"Divination", L"占卜术"},
    {L"Illusion", L"幻象术"},
    {L"Abjuration", L"防护术"},
    {L"Necromancy", L"死灵术"},

    // Only for testing
    // {L"地图", L"Map"},
};

std::map<std::wstring, System_String_o*> new_string_cache;


using SystemEnumGetNameT = System_String_array* (void*, void*);
intptr_t tgt_SystemEnumGetName = 0x523ab0;
SystemEnumGetNameT* orig_SystemEnumGetName = nullptr;
System_String_array* hook_SystemEnumGetName(void* enumType, void* method) {

    auto* ret = orig_SystemEnumGetName(enumType, method);

    std::wstring msg;
    for (int i = 0; i < ret->max_length; i++) {
        auto* item = ret->m_Items[i];

        std::wstring ss(
            (wchar_t*)&item->fields._firstChar,
            item->fields._stringLength
        );

        if (!enum_string_map.contains(ss)) continue;

        auto new_name = enum_string_map[ss];

        if (!new_string_cache.contains(new_name)) {
            auto bufsize = sizeof(System_String_o) + sizeof(wchar_t) * new_name.length() * 2;
            auto* new_string = (System_String_o*)malloc(bufsize);
            memset(new_string, 0, bufsize);
            memcpy(new_string, item, sizeof(System_String_o));
            new_string->fields._stringLength = new_name.length();
            wcscpy((wchar_t*)&new_string->fields._firstChar, new_name.c_str());

            new_string_cache[new_name] = new_string;
        }

        ret->m_Items[i] = new_string_cache[new_name];
    }

    return ret;
}

namespace EnumString {

    int init(DWORD base) {
        tgt_SystemEnumGetName += base;
        std::cout << std::format("System_Enum__GetName: {:#x}\n", tgt_SystemEnumGetName);
        if (MH_CreateHook((LPVOID)tgt_SystemEnumGetName, &hook_SystemEnumGetName, (LPVOID*)&orig_SystemEnumGetName) != MH_OK) {
            std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_SystemEnumGetName);
            return 1;
        }

        if (MH_EnableHook((LPVOID)tgt_SystemEnumGetName) != MH_OK) {
            std::cout << std::format("MH_EnableHook Failed\n");
            return 1;
        }

        return 0;
    }
}
