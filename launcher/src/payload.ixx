#include <windows.h>
#include <string>

#include "consts.h"

export module Payload;

using CreateWindowExWT = HWND (WINAPI *)(
    DWORD     dwExStyle,
    LPCWSTR   lpClassName,
    LPCWSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
);

CreateWindowExWT orig_CreateWindowExW = NULL;

HWND WINAPI DetourCreateWindowExW(
    DWORD     dwExStyle,
    LPCWSTR   lpClassName,
    LPCWSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
) {
    if (lpWindowName) {
        std::wstring window_name(lpWindowName);

        if (window_name == ORIG_TITLE_NAME) {
            window_name += TITLE_NAME_SUFFIX;
            lpWindowName = window_name.c_str();
        }
    }

    return orig_CreateWindowExW(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}
