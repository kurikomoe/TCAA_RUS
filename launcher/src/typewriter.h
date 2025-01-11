/*
    Fix TypeWriter Effects in Chinese mode
*/
#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct __declspec(align(4)) System_String_Fields // sizeof=0x8
{
    int32_t _stringLength;
    uint16_t _firstChar;
    // padding byte
    // padding byte
};

struct System_String_o // sizeof=0x10
{
    void *klass;
    void *monitor;
    System_String_Fields fields;
};

System_String_o* new_sep = nullptr;

// FIXME(kuriko): use PatternSearch rather than hardcode RVA
intptr_t System_String_Split = 0x3FFE60;
intptr_t tgt_TypewriterSplitText = 0x1fff73;
void* orig_TypewriterSplitText = nullptr;

using SystemStringSplitFnT = void*(void* This, void* sep, int32_t options, void* method);


extern "C" void* new_System_String_Split(void* This, void* sep, int32_t options, void* method) {
    if (new_sep == nullptr) {
        new_sep = (System_String_o*)malloc(sizeof(System_String_o));
        memcpy(new_sep, sep, sizeof(System_String_o));
        new_sep->fields._firstChar = 0x200B;  // FIXME(kuriko): ZeroWidth Space
    }
    auto* ptr = reinterpret_cast<SystemStringSplitFnT*>(System_String_Split);
    // tail jump
    return ptr(This, new_sep, options, method);
}


void __declspec(naked) hook_TypewriterSplitText() {
    __asm {
        call new_System_String_Split
        jmp orig_TypewriterSplitText
    };
}
