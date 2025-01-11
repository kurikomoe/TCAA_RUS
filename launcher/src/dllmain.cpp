#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <windows.h>

#include "payload.h"
#include "version.h"

std::atomic_flag Initialized;
constexpr bool IsDebug = true;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    if (!Initialized.test_and_set()) {
      version.dll = LoadLibraryW(LR"(C:\Windows\SysWOW64\version.dll)");
      setupFunctions();

      if (IsDebug) {
        AllocConsole();
        freopen_s((FILE **)stdout, "CONOUT$", "w",
                  stdout); // Redirect stdout
        freopen_s((FILE **)stdin, "CONIN$", "r",
                  stdin); // Redirect stdin
        freopen_s((FILE **)stderr, "CONOUT$", "w",
                  stderr); // Redirect stderr
      }

      HANDLE mainHandle =
          CreateThread(NULL, 0, payload_main, 0, CREATE_SUSPENDED, 0);

      if (mainHandle) {
        SetThreadPriority(mainHandle, THREAD_PRIORITY_TIME_CRITICAL);
        ResumeThread(mainHandle);
        CloseHandle(mainHandle);
      }
    }
    break;
  case DLL_PROCESS_DETACH:
    FreeLibrary(version.dll);
    FreeConsole();
    break;
  }
  return 1;
}

extern "C" __declspec(dllexport) int WINAPI TCAA_CHS() { return 0; }
