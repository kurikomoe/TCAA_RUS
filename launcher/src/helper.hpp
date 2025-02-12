#pragma once

#include <windows.h>
#include <vector>
#include <cassert>
#include <MinHook.h>
#include <iostream>
#include <format>
#include <string_view>

namespace Memory {

    template<typename T>
    void Write(uintptr_t writeAddress, T value) {
        DWORD oldProtect;
        VirtualProtect((LPVOID)(writeAddress), sizeof(T), PAGE_EXECUTE_WRITECOPY, &oldProtect);
        *(reinterpret_cast<T*>(writeAddress)) = value;
        VirtualProtect((LPVOID)(writeAddress), sizeof(T), oldProtect, &oldProtect);
    }

    void PatchBytes(uintptr_t address, const char *pattern,
                    unsigned int numBytes);

    // CSGOSimple's pattern scan
    // https://github.com/OneshotGH/CSGOSimple-master/blob/master/CSGOSimple/helpers/utils.cpp
    std::uint8_t *PatternScan(void *module, const char *signature, size_t offset = 0);

    static HMODULE GetThisDllHandle();

    uint32_t ModuleTimestamp(void *module);

    uintptr_t GetAbsolute(uintptr_t address) noexcept;
}

namespace utils {
    template<typename hookT, typename origT>
    DWORD ApplyPatch(
        std::wstring_view name,
        DWORD base, intptr_t& tgt_func, hookT& hook_func, origT& orig_func
    ) {
        tgt_func += base;
        std::wcout << std::format(L"{} address: {:#x}\n", name, tgt_func);
        if (MH_CreateHook(
            (LPVOID)tgt_func,
            &hook_func,
            (LPVOID*)&orig_func
        ) != MH_OK) {
            std::wcout << std::format(L"MH_CreateHook Failed ({}): {:#x}\n", name, (DWORD)tgt_func);
            return 1;
        }

        if (MH_EnableHook((LPVOID)tgt_func) != MH_OK) {
            std::wcout << std::format(L"MH_EnableHook Failed ({}): {:#x}\n", name, (DWORD)tgt_func);
            return 1;
        }

        return 0;
    }
}
