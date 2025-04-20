#pragma once

#include <MinHook.h>
#include <string>
#include <cstdint>
#include <map>

#include "game_def.h"
#include "helper.hpp"


std::map<std::wstring, std::wstring> enum_string_map = {
    {L"Autopsy", L"Вскрытие"},
    {L"Compendium", L"Компендиум"},
    {L"Testimony", L"Показания"},
    {L"Contract", L"Контракт"},
    {L"Map", L"Карта"},

    {L"Evocation", L"Призывание"},
    {L"Transmutation", L"Трансмутация"},
    {L"Conjuration", L"Колдовство"},
    {L"Divination", L"Прорицание"},
    {L"Illusion", L"Иллюзия"},
    {L"Abjuration", L"Отречение"},
    {L"Necromancy", L"Некромантия"},

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
