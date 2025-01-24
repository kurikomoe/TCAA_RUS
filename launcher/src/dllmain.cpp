#include <atomic>
#include <cstdio>
#include <windows.h>
#include <fcntl.h>

#include "version_check.h"
#include "tmpro.h"
#include "payload.h"
#include "version.h"
#include "flags.h"
#include "consts.h"

std::atomic_flag Initialized;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {

  setlocale( LC_ALL, "");
  std::locale::global(std::locale( "" ));

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

      check_version();

      payload_main(nullptr);

      // HANDLE mainHandle =
      //     CreateThread(NULL, 0, payload_main, 0, CREATE_SUSPENDED, 0);

      // if (mainHandle) {
      //   SetThreadPriority(mainHandle, THREAD_PRIORITY_TIME_CRITICAL);
      //   ResumeThread(mainHandle);
      //   CloseHandle(mainHandle);
      // }
    }
    break;
  case DLL_PROCESS_DETACH:
    fclose(f_text);
    FreeConsole();
    FreeLibrary(version.dll);
    break;
  }
  return 1;
}

extern "C" __declspec(dllexport) int WINAPI TCAA_CHS() { return 0; }
