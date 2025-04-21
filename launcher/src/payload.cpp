#include <string>
#include <format>
#include <iostream>

#include <windows.h>
#include <MinHook.h>

#include "payload.h"

#include "createfile.h"
#include "typewriter.h"
#include "textdelay.h"
#include "enum_string.h"
#include "character.h"
#include "tmpro.h"
#include "savefile.h"
#include "yarn.h"

std::wstring ORIG_TITLE_NAME(L"Attorney of the Arcane");
std::wstring TITLE_NAME_SUFFIX;

using SetWindowTextWT = decltype(&SetWindowTextW);
SetWindowTextWT orig_SetWindowTextW = NULL;

BOOL WINAPI hook_SetWindowTextW(HWND hWnd, LPCWSTR lpString) {
    if (lpString) {
        std::wstring window_name(lpString);
        std::wcout << window_name << std::endl;

        if (window_name == ORIG_TITLE_NAME) {
            // window_name += TITLE_NAME_SUFFIX;
            window_name = L"Адвокат Арканы " + TITLE_NAME_SUFFIX;
            lpString = window_name.c_str();
        }
    }

    return orig_SetWindowTextW(hWnd, lpString);
}

DWORD __stdcall payload_main(void*) {
    // inits
    TITLE_NAME_SUFFIX = std::format(group_text, version_string());

    std::wcout << L"inside payload" << std::endl;
    std::wcout << TITLE_NAME_SUFFIX << std::endl;

    if (MH_Initialize() != MH_OK) {
        std::wcout << L"MH_Initialize failed" << std::endl;
        return 0;
    }

    if (MH_CreateHook(&SetWindowTextW, hook_SetWindowTextW, (LPVOID*)&orig_SetWindowTextW) != MH_OK) {
        std::cout << "SetWindowTextW Failed"  << std::endl;
        return 0;
    }

    if (MH_EnableHook(&SetWindowTextW) != MH_OK)
        return 0;

    // ========================= System Apis =================================
    if (CreateFileHook::init() != 0) {
        std::cout << "CreateFile hook failed" << std::endl;
    }

    // ========================= Game Specific =================================
    DWORD hDll = (DWORD)LoadLibraryW(L"GameAssembly.dll");
    DWORD base = (DWORD)GetModuleHandleW(L"GameAssembly.dll");
    printf("Module base: %ld\n", base);

    if (Typewriter::init(base) != 0) {
        std::cout << "Typewriter hook failed" << std::endl;
    }

    if (TextDelay::init(base) != 0) {
        std::cout << "TextDelay hook failed" << std::endl;
    }

    if (EnumString::init(base) != 0) {
        std::cout << "EnumString hook failed" << std::endl;
    }

    if (TMPro::init(base) != 0) {
        std::cout << "TMPro hook failed" << std::endl;
    }

    // if (Yarn::init(base) != 0) {
    //     std::cout << "Yarn hook failed" << std::endl;
    // }

    if (SaveFile::init(base) != 0) {
        std::cout << "SaveFile hook failed" << std::endl;
    }

    if (Character::init(base) != 0) {
        std::cout << "Character hook failed" << std::endl;
    }

    return 1;
}
