#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <atomic>

#include "version.h"
#include "payload.h"


std::atomic_flag Initialized;
constexpr bool IsDebug = true;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		if (!Initialized.test_and_set()) {
			version.dll = LoadLibraryW(LR"(C:\Windows\SysWOW64\version.dll)");
			setupFunctions();

			payload_main();

			if (IsDebug) {
				AllocConsole();
				freopen_s((FILE**)stdout, "CONOUT$", "w", stdout); // Redirect stdout
				freopen_s((FILE**)stdin, "CONIN$", "r", stdin);   // Redirect stdin
				freopen_s((FILE**)stderr, "CONOUT$", "w", stderr); // Redirect stderr
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
