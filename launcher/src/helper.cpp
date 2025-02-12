#include <MinHook.h>
#include <cassert>
#include <format>
#include <iostream>
#include <string_view>
#include <vector>
#include <windows.h>

#include "helper.hpp"


void Memory::PatchBytes(uintptr_t address, const char *pattern,
                        unsigned int numBytes) {
  DWORD oldProtect;
  VirtualProtect((LPVOID)address, numBytes, PAGE_EXECUTE_READWRITE,
                 &oldProtect);
  memcpy((LPVOID)address, pattern, numBytes);
  VirtualProtect((LPVOID)address, numBytes, oldProtect, &oldProtect);
}
std::uint8_t *Memory::PatternScan(void *module, const char *signature,
                                  size_t offset) {
  static auto pattern_to_byte = [](const char *pattern) {
    auto bytes = std::vector<int>{};
    auto start = const_cast<char *>(pattern);
    auto end = const_cast<char *>(pattern) + strlen(pattern);

    for (auto current = start; current < end; ++current) {
      if (*current == '?') {
        ++current;
        if (*current == '?')
          ++current;
        bytes.push_back(-1);
      } else {
        bytes.push_back(strtoul(current, &current, 16));
      }
    }
    return bytes;
  };

  auto dosHeader = (PIMAGE_DOS_HEADER)module;
  auto ntHeaders =
      (PIMAGE_NT_HEADERS)((std::uint8_t *)module + dosHeader->e_lfanew);

  auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
  auto patternBytes = pattern_to_byte(signature);
  auto scanBytes = reinterpret_cast<std::uint8_t *>(module);

  auto s = patternBytes.size();
  auto d = patternBytes.data();

  for (auto i = offset; i < sizeOfImage - s; ++i) {
    bool found = true;
    for (auto j = 0ul; j < s; ++j) {
      if (scanBytes[i + j] != d[j] && d[j] != -1) {
        found = false;
        break;
      }
    }
    if (found) {
      return &scanBytes[i];
    }
  }
  return nullptr;
}
static HMODULE Memory::GetThisDllHandle() {
  MEMORY_BASIC_INFORMATION info;
  size_t len = VirtualQueryEx(GetCurrentProcess(), (void *)GetThisDllHandle,
                              &info, sizeof(info));
  assert(len == sizeof(info));
  return len ? (HMODULE)info.AllocationBase : NULL;
}
uint32_t Memory::ModuleTimestamp(void *module) {
  auto dosHeader = (PIMAGE_DOS_HEADER)module;
  auto ntHeaders =
      (PIMAGE_NT_HEADERS)((std::uint8_t *)module + dosHeader->e_lfanew);
  return ntHeaders->FileHeader.TimeDateStamp;
}
uintptr_t Memory::GetAbsolute(uintptr_t address) noexcept {
  return (address + 4 + *reinterpret_cast<std::int32_t *>(address));
}
