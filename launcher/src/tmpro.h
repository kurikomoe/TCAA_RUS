#pragma once

#include <windows.h>
#include <MinHook.h>
#include <string>
#include <map>
#include <fstream>
#include <set>

#include "game_def.h"


extern std::map<std::wstring, System_String_o*> new_string_cache;

extern std::set<std::wstring> text_set;

extern FILE* f_text;

void hook_TMPro_TMP_Text__set_text(void *This, System_String_o *text, void *method);

namespace TMPro {

int init(DWORD base);
}
