/*
    Fix TypeWriter Effects in Chinese mode
*/
#pragma once
#include <cstdint>
#include <iostream>
#include <windows.h>

#include <MinHook.h>

#include "game_def.h"


// FIXME(kuriko): use PatternSearch rather than hardcode RVA
// RVA here
using FuncT = bool(void* This, void* method);
intptr_t tgt_func = 0x1dbd50;
FuncT* orig_func = nullptr;
bool hook_func(DialogueGUI__TypeDialogue_d__90_o* This, void* method) {
    // DialogueGUI_o*
    DialogueGUI_o*  DialogueGUI_This = This->fields.__4__this;

    auto fastTextPause = 0.0; // DialogueGUI_This->fields.fastTextPause->fields.m_Seconds;
    auto fastTextWait = 0.0; //DialogueGUI_This->fields.fastTextWait->fields.m_Seconds;

    std::cout << "=========== in func =============" << std::endl;
    std::cout << "SkipTime: " << DialogueGUI_This->fields.skip_time << std::endl;
    std::cout << "typeDelay: " << DialogueGUI_This->fields.typeDelay << std::endl;
    std::cout << "pauseDelay: " << DialogueGUI_This->fields.pauseDelay << std::endl;
    std::cout << "autoPlayDelay: " << DialogueGUI_This->fields.autoPlayDelay << std::endl;
    std::cout << "textInstantPlayDelay: " << DialogueGUI_This->fields.textInstantPlayDelay << std::endl;

    std::wstring ss(
        (wchar_t*)&This->fields.dialogueLine->fields.RawText->fields._firstChar,
        This->fields.dialogueLine->fields.RawText->fields._stringLength
    );
    // std::wcout << L""ss << std::endl;
    // This->fields._letter_count_5__7;
    if (DialogueGUI_This->fields.slowTextPause) {
        std::cout << "slowTextPause: " << DialogueGUI_This->fields.slowTextPause->fields.m_Seconds << std::endl;
        DialogueGUI_This->fields.slowTextPause->fields.m_Seconds = fastTextPause;
    }
    if (DialogueGUI_This->fields.slowTextWait) {
        std::cout << "slowTextWait: " << DialogueGUI_This->fields.slowTextWait->fields.m_Seconds << std::endl;
        DialogueGUI_This->fields.slowTextWait->fields.m_Seconds = fastTextWait;
    }
    if (DialogueGUI_This->fields.textPause) {
        std::cout << "TextPause: " << DialogueGUI_This->fields.textPause->fields.m_Seconds << std::endl;
        DialogueGUI_This->fields.textPause->fields.m_Seconds = fastTextPause;
    }
    if (DialogueGUI_This->fields.textWait) {
        std::cout << "TextWait: " << DialogueGUI_This->fields.textWait->fields.m_Seconds << std::endl;
        DialogueGUI_This->fields.textWait->fields.m_Seconds = fastTextWait;
    }
    if (DialogueGUI_This->fields.fastTextPause) {
        std::cout << "fastTextPause: " << DialogueGUI_This->fields.fastTextPause->fields.m_Seconds << std::endl;
        DialogueGUI_This->fields.fastTextPause->fields.m_Seconds = fastTextPause;
    }
    if (DialogueGUI_This->fields.fastTextWait) {
        std::cout << "fastTextWait: " << DialogueGUI_This->fields.fastTextWait->fields.m_Seconds << std::endl;
        DialogueGUI_This->fields.fastTextWait->fields.m_Seconds = fastTextWait;
    }

    auto ret = orig_func(This, method);
    return ret;
}

using Func2T = bool(void* This, void* method);
intptr_t tgt_func2 = 0xbdcc30;
Func2T* orig_func2 = nullptr;
bool hook_func2(Yarn_Unity_LineView__RunLineInternal_d__21_o* This, void* method) {
    std::cout << "Hold Time: " << This->fields.__4__this->fields.holdTime << std::endl;
    This->fields.__4__this->fields.holdTime = 0.1;
    return orig_func2(This, method);
}

using Func3T = bool(void* This, void* method);
intptr_t tgt_func3 = 0xbde890;
Func3T* orig_func3 = nullptr;
bool hook_func3(Yarn_Unity_DefaultActions__Wait_d__0_o* This, void* method) {
    std::cout << "Wait Time: " << This->fields.duration << std::endl;
    // This->fields.__4__this->fields.holdTime = 0.1;
    return orig_func3(This, method);
}

using Func4T = void(void* This, float seconds, void* method);
intptr_t tgt_func4 = 0x9bc750;
Func4T* orig_func4 = nullptr;
void hook_func4(void* This, float seconds, void* method) {
    // std::cout << "Wait Time2: " << seconds << std::endl;
    orig_func4(This, seconds, method);
}

using Func5T = bool(void* This, void* method);
intptr_t tgt_func5 = 0x1dd6e0;
Func5T* orig_func5 = nullptr;
bool hook_func5(DialogueGUI__WaitForProgressInput_d__97_o* This, void* method) {
    // std::cout << "Wait Time3: " << seconds << std::endl;
    auto* DialogueGUI_This = This->fields.__4__this;

    auto ret = orig_func5(This, method);
    std::cout << "=========== in func5 =============" << std::endl;
    std::cout << "SkipTime: " << DialogueGUI_This->fields.skip_time << std::endl;
    std::cout << "typeDelay: " << DialogueGUI_This->fields.typeDelay << std::endl;
    std::cout << "pauseDelay: " << DialogueGUI_This->fields.pauseDelay << std::endl;
    std::cout << "autoPlayDelay: " << DialogueGUI_This->fields.autoPlayDelay << std::endl;
    std::cout << "textInstantPlayDelay: " << DialogueGUI_This->fields.textInstantPlayDelay << std::endl;

    // DialogueGUI_This->fields.skip_time = 0;
    // DialogueGUI_This->fields.typeDelay = 0;
    // DialogueGUI_This->fields.pauseDelay = 0;
    // DialogueGUI_This->fields.autoPlayDelay = 0;
    // DialogueGUI_This->fields.textInstantPlayDelay = 0;
    return ret;
}

using Func6T = bool(void* This, void* method);
intptr_t tgt_func6 = 0x1cc270;
Func6T* orig_func6 = nullptr;
bool hook_func6(Axiom__YarnWait_d__9_o* This, void* method) {
    // std::cout << "=========== in func6 =============" << std::endl;
    // std::cout << "Wait Time6: " << This->fields.time << std::endl;
    return orig_func6(This, method);
}

using Func7T = bool(void* This, void* method);
intptr_t tgt_func7 = 0x1ca7e0;
Func7T* orig_func7 = nullptr;
bool hook_func7(SFXInstance__Routine_d__7_o* This, void* method) {
    // std::cout << "=========== in func7 =============" << std::endl;
    // std::cout << "Wait Time7: " << This->fields.time << std::endl;
    return orig_func7(This, method);
}

using Func8T = bool(void* This, void* method);
intptr_t tgt_func8 = 0xbdcc30;
Func8T* orig_func8 = nullptr;
bool hook_func8(SFXInstance__Routine_d__7_o* This, void* method) {
    // std::cout << "=========== in func8 =============" << std::endl;
    // std::cout << "Wait Time7: " << This->fields.time << std::endl;
    return orig_func8(This, method);
}


namespace TextDelay {
    int init(DWORD base) {

        // auto playSFX1 = base + 0x1dc10f;
        // auto playSFX2 = base + 0x1dc895;
        // for (int i = 0; i < 5; i++) {
        //     Memory::Write(playSFX1 + i, (uint8_t)0x90);
        //     Memory::Write(playSFX2 + i, (uint8_t)0x90);
        // }

        // tgt_func += base;
        // std::cout << std::format("DialogueGUI__TypeDialogue_d__90__MoveNext: {:#x}\n", tgt_func);
        // if (MH_CreateHook((LPVOID)tgt_func, &hook_func, (LPVOID*)&orig_func) != MH_OK) {
        //     std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_func);
        //     return 1;
        // }
        // if (MH_EnableHook((LPVOID)tgt_func) != MH_OK) {
        //     std::cout << std::format("MH_EnableHook Failed\n");
        //     return 1;
        // }

        // tgt_func2 += base;
        // std::cout << std::format("Yarn_Unity_LineView__RunLineInternal_d__21__MoveNext: {:#x}\n", tgt_func2);
        // if (MH_CreateHook((LPVOID)tgt_func2, &hook_func2, (LPVOID*)&orig_func2) != MH_OK) {
        //     std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_func2);
        //     return 1;
        // }

        // if (MH_EnableHook((LPVOID)tgt_func2) != MH_OK) {
        //     std::cout << std::format("MH_EnableHook Failed\n");
        //     return 1;
        // }

        // tgt_func3 += base;
        // std::cout << std::format("Yarn_Unity_DefaultActions__Wait_d__0__MoveNext: {:#x}\n", tgt_func3);
        // if (MH_CreateHook((LPVOID)tgt_func3, &hook_func3, (LPVOID*)&orig_func3) != MH_OK) {
        //     std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_func3);
        //     return 1;
        // }

        // if (MH_EnableHook((LPVOID)tgt_func3) != MH_OK) {
        //     std::cout << std::format("MH_EnableHook Failed\n");
        //     return 1;
        // }

        // tgt_func4 += base;
        // std::cout << std::format("UnityEngine.WaitForSeconds$$.ctor: {:#x}\n", tgt_func4);
        // if (MH_CreateHook((LPVOID)tgt_func4, &hook_func4, (LPVOID*)&orig_func4) != MH_OK) {
        //     std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_func4);
        //     return 1;
        // }

        // if (MH_EnableHook((LPVOID)tgt_func4) != MH_OK) {
        //     std::cout << std::format("MH_EnableHook Failed\n");
        //     return 1;
        // }

        // tgt_func5 += base;
        // std::cout << std::format("DialogueGUI__WaitForProgressInput_d__97__MoveNext: {:#x}\n", tgt_func5);
        // if (MH_CreateHook((LPVOID)tgt_func5, &hook_func5, (LPVOID*)&orig_func5) != MH_OK) {
        //     std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_func5);
        //     return 1;
        // }

        // if (MH_EnableHook((LPVOID)tgt_func5) != MH_OK) {
        //     std::cout << std::format("MH_EnableHook Failed\n");
        //     return 1;
        // }

        // tgt_func6 += base;
        // std::cout << std::format("Axiom__YarnWait_d__9__MoveNext: {:#x}\n", tgt_func6);
        // if (MH_CreateHook((LPVOID)tgt_func6, &hook_func6, (LPVOID*)&orig_func6) != MH_OK) {
        //     std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_func6);
        //     return 1;
        // }

        // if (MH_EnableHook((LPVOID)tgt_func6) != MH_OK) {
        //     std::cout << std::format("MH_EnableHook Failed\n");
        //     return 1;
        // }

        // tgt_func7 += base;
        // std::cout << std::format("SFXInstance__Routine_d__7__MoveNext: {:#x}\n", tgt_func7);
        // if (MH_CreateHook((LPVOID)tgt_func7, &hook_func7, (LPVOID*)&orig_func7) != MH_OK) {
        //     std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_func7);
        //     return 1;
        // }

        // if (MH_EnableHook((LPVOID)tgt_func7) != MH_OK) {
        //     std::cout << std::format("MH_EnableHook Failed\n");
        //     return 1;
        // }

        // tgt_func9 += base;
        // std::cout << std::format("System_Enum__GetName: {:#x}\n", tgt_func9);
        // if (MH_CreateHook((LPVOID)tgt_func9, &hook_func9, (LPVOID*)&orig_func9) != MH_OK) {
        //     std::cout << std::format("MH_CreateHook Failed: {:#x}\n", (DWORD)tgt_func9);
        //     return 1;
        // }

        // if (MH_EnableHook((LPVOID)tgt_func9) != MH_OK) {
        //     std::cout << std::format("MH_EnableHook Failed\n");
        //     return 1;
        // }

        return 0;
    }
}
