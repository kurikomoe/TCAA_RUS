#pragma once

#include <iostream>
#include <format>
#include <MinHook.h>
#include <string>
#include <cstdint>
#include <map>

#include "game_def.h"
#include "flags.h"


using Yarn_Unity_DialogueRunner__StartT = void* (void*, void*);
intptr_t tgt_Yarn_Unity_DialogueRunner__Start = 0xbd61f0;
Yarn_Unity_DialogueRunner__StartT* orig_Yarn_Unity_DialogueRunner__Start = nullptr;
void hook_Yarn_Unity_DialogueRunner__Start(Yarn_Unity_DialogueRunner_o *This, void* method) {

    This->fields.verboseLogging = true;
    std::wcout << L"Yarn Start Called" << std::endl;
    orig_Yarn_Unity_DialogueRunner__Start(This, method);
}

using UnityEngine_Debug__get_isDebugBuildT = bool (void*);
intptr_t tgt_UnityEngine_Debug__get_isDebugBuild = 0x97e110;
UnityEngine_Debug__get_isDebugBuildT* orig_UnityEngine_Debug__get_isDebugBuild = nullptr;
bool hook_UnityEngine_Debug__get_isDebugBuild(void* method) {
    return true;
}

using Func1T = void (GameState_o*, System_String_o*, System_String_o*, System_String_o*, void*);
intptr_t tgt_Func1 = 0x1beae0;
Func1T* orig_Func1 = nullptr;
void hook_Func1(GameState_o* state, System_String_o* location, System_String_o* occupant, System_String_o* examine, void* method) {
    std::wcout << "Location: " << utils::wstring(state->fields.location) << std::endl;
    orig_Func1(state, location, occupant, examine, method);
}


namespace Yarn {

    int init(DWORD base) {
        tgt_Yarn_Unity_DialogueRunner__Start += base;
        std::cout << std::format("Yarn_Unity_DialogueRunner__Start: {:#x}\n", tgt_Yarn_Unity_DialogueRunner__Start);
        if (MH_CreateHook((LPVOID)tgt_Yarn_Unity_DialogueRunner__Start, &hook_Yarn_Unity_DialogueRunner__Start, (LPVOID*)&orig_Yarn_Unity_DialogueRunner__Start) != MH_OK) {
            std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_Yarn_Unity_DialogueRunner__Start);
            return 1;
        }

        if (MH_EnableHook((LPVOID)tgt_Yarn_Unity_DialogueRunner__Start) != MH_OK) {
            std::cout << std::format("MH_EnableHook Failed\n");
            return 1;
        }

        tgt_UnityEngine_Debug__get_isDebugBuild += base;
        std::cout << std::format("UnityEngine_Debug__get_isDebugBuild: {:#x}\n", tgt_UnityEngine_Debug__get_isDebugBuild);
        if (MH_CreateHook((LPVOID)tgt_UnityEngine_Debug__get_isDebugBuild, &hook_UnityEngine_Debug__get_isDebugBuild, (LPVOID*)&orig_UnityEngine_Debug__get_isDebugBuild) != MH_OK) {
            std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_UnityEngine_Debug__get_isDebugBuild);
            return 1;
        }

        if (MH_EnableHook((LPVOID)tgt_UnityEngine_Debug__get_isDebugBuild) != MH_OK) {
            std::cout << std::format("MH_EnableHook Failed\n");
            return 1;
        }

        tgt_Func1 += base;
        std::cout << std::format("Func1: {:#x}\n", tgt_Func1);
        if (MH_CreateHook((LPVOID)tgt_Func1, &hook_Func1, (LPVOID*)&orig_Func1) != MH_OK) {
            std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_Func1);
            return 1;
        }

        if (MH_EnableHook((LPVOID)tgt_Func1) != MH_OK) {
            std::cout << std::format("MH_EnableHook Failed\n");
            return 1;
        }

        return 0;
    }
}
