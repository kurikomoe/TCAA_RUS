#pragma once

#include <MinHook.h>
#include <string>
#include <cstdint>
#include <map>

#include "game_def.h"
#include "helper.hpp"


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

using SystemEnumGetNameT = System_String_array* (void*, void*);
intptr_t tgt_SystemEnumGetName = 0x523ab0;
SystemEnumGetNameT* orig_SystemEnumGetName = nullptr;
System_String_array* hook_SystemEnumGetName(void* enumType, void* method) {

    auto* ret = orig_SystemEnumGetName(enumType, method);

    std::vector<std::wstring> msgVec;
    for (int i = 0; i < ret->max_length; i++) {
        auto* item = ret->m_Items[i];

        std::wstring ss(
            (wchar_t*)&item->fields._firstChar,
            item->fields._stringLength
        );

        msgVec.emplace_back(ss);
    }

    bool isKeyEnum = std::find(msgVec.cbegin(), msgVec.cend(), L"Backspace") != msgVec.cend() \
        && std::find(msgVec.cbegin(), msgVec.cend(), L"LeftAlt") != msgVec.cend();

    std::wstring msg;
    for (int i = 0; i < ret->max_length; i++) {
        auto* item = ret->m_Items[i];

        auto ss = utils::wstring(item);

        if (!enum_string_map.contains(ss)) continue;

        auto new_name = enum_string_map[ss];

        // Avoid renaming common key Enter & Return etc
        // if (new_name.find(L"键") != std::wstring::npos && !isKeyEnum)
        //     continue;

        ret->m_Items[i] = utils::GetSystemString(new_name, item);
    }

    return ret;
}

namespace EnumString {

    int init(DWORD base) {
        if (utils::ApplyPatch(
            L"SystemEnumGetName",
            base,
            tgt_SystemEnumGetName,
            hook_SystemEnumGetName,
            orig_SystemEnumGetName) != 0) {
            return 1;
        }

        return 0;
    }
}
