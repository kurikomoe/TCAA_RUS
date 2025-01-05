#include <windows.h>

#pragma region Proxy
struct version_dll {
	HMODULE dll;
	FARPROC oGetFileVersionInfoA;
	FARPROC oGetFileVersionInfoByHandle;
	FARPROC oGetFileVersionInfoExA;
	FARPROC oGetFileVersionInfoExW;
	FARPROC oGetFileVersionInfoSizeA;
	FARPROC oGetFileVersionInfoSizeExA;
	FARPROC oGetFileVersionInfoSizeExW;
	FARPROC oGetFileVersionInfoSizeW;
	FARPROC oGetFileVersionInfoW;
	FARPROC oVerFindFileA;
	FARPROC oVerFindFileW;
	FARPROC oVerInstallFileA;
	FARPROC oVerInstallFileW;
	FARPROC oVerLanguageNameA;
	FARPROC oVerLanguageNameW;
	FARPROC oVerQueryValueA;
	FARPROC oVerQueryValueW;
} version;

extern "C" {
	void __declspec(naked) fGetFileVersionInfoA() { __asm { jmp[version.oGetFileVersionInfoA] } }
	void __declspec(naked) fGetFileVersionInfoByHandle() { __asm { jmp[version.oGetFileVersionInfoByHandle] } }
	void __declspec(naked) fGetFileVersionInfoExA() { __asm { jmp[version.oGetFileVersionInfoExA] } }
	void __declspec(naked) fGetFileVersionInfoExW() { __asm { jmp[version.oGetFileVersionInfoExW] } }
	void __declspec(naked) fGetFileVersionInfoSizeA() { __asm { jmp[version.oGetFileVersionInfoSizeA] } }
	void __declspec(naked) fGetFileVersionInfoSizeExA() { __asm { jmp[version.oGetFileVersionInfoSizeExA] } }
	void __declspec(naked) fGetFileVersionInfoSizeExW() { __asm { jmp[version.oGetFileVersionInfoSizeExW] } }
	void __declspec(naked) fGetFileVersionInfoSizeW() { __asm { jmp[version.oGetFileVersionInfoSizeW] } }
	void __declspec(naked) fGetFileVersionInfoW() { __asm { jmp[version.oGetFileVersionInfoW] } }
	void __declspec(naked) fVerFindFileA() { __asm { jmp[version.oVerFindFileA] } }
	void __declspec(naked) fVerFindFileW() { __asm { jmp[version.oVerFindFileW] } }
	void __declspec(naked) fVerInstallFileA() { __asm { jmp[version.oVerInstallFileA] } }
	void __declspec(naked) fVerInstallFileW() { __asm { jmp[version.oVerInstallFileW] } }
	void __declspec(naked) fVerLanguageNameA() { __asm { jmp[version.oVerLanguageNameA] } }
	void __declspec(naked) fVerLanguageNameW() { __asm { jmp[version.oVerLanguageNameW] } }
	void __declspec(naked) fVerQueryValueA() { __asm { jmp[version.oVerQueryValueA] } }
	void __declspec(naked) fVerQueryValueW() { __asm { jmp[version.oVerQueryValueW] } }
}

void setupFunctions() {
	version.oGetFileVersionInfoA = GetProcAddress(version.dll, "GetFileVersionInfoA");
	version.oGetFileVersionInfoByHandle = GetProcAddress(version.dll, "GetFileVersionInfoByHandle");
	version.oGetFileVersionInfoExA = GetProcAddress(version.dll, "GetFileVersionInfoExA");
	version.oGetFileVersionInfoExW = GetProcAddress(version.dll, "GetFileVersionInfoExW");
	version.oGetFileVersionInfoSizeA = GetProcAddress(version.dll, "GetFileVersionInfoSizeA");
	version.oGetFileVersionInfoSizeExA = GetProcAddress(version.dll, "GetFileVersionInfoSizeExA");
	version.oGetFileVersionInfoSizeExW = GetProcAddress(version.dll, "GetFileVersionInfoSizeExW");
	version.oGetFileVersionInfoSizeW = GetProcAddress(version.dll, "GetFileVersionInfoSizeW");
	version.oGetFileVersionInfoW = GetProcAddress(version.dll, "GetFileVersionInfoW");
	version.oVerFindFileA = GetProcAddress(version.dll, "VerFindFileA");
	version.oVerFindFileW = GetProcAddress(version.dll, "VerFindFileW");
	version.oVerInstallFileA = GetProcAddress(version.dll, "VerInstallFileA");
	version.oVerInstallFileW = GetProcAddress(version.dll, "VerInstallFileW");
	version.oVerLanguageNameA = GetProcAddress(version.dll, "VerLanguageNameA");
	version.oVerLanguageNameW = GetProcAddress(version.dll, "VerLanguageNameW");
	version.oVerQueryValueA = GetProcAddress(version.dll, "VerQueryValueA");
	version.oVerQueryValueW = GetProcAddress(version.dll, "VerQueryValueW");
}
#pragma endregion
