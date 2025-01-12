#pragma once

#include <cstdint>

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
