#include <windows.h>
#include <cstdint>
#include <iostream>
#include <cstddef>
#include <string>
#include <map>
#include <algorithm>

#include "game_def.h"

namespace utils {
std::map<std::wstring, System_String_o*> new_string_cache;

System_String_o *GetSystemString(const std::wstring &text,
                                        System_String_o *ref_system_string) {
  if (new_string_cache.contains(text)) {
    return new_string_cache[text];
  }

  auto bufsize = sizeof(System_String_o) + sizeof(wchar_t) * text.length() * 2;
  auto *new_string = (System_String_o *)malloc(bufsize);
  memset(new_string, 0, bufsize);
  memcpy(new_string, ref_system_string, sizeof(System_String_o));
  new_string->fields._stringLength = text.length();
  wcscpy((wchar_t *)&new_string->fields._firstChar, text.c_str());

  new_string_cache[text] = new_string;
  return new_string;
}
std::wstring wstring(System_String_o *text) {
  if (text->fields._stringLength == 0) {
    return L"";
  }

  std::wstring ss((wchar_t *)&text->fields._firstChar,
                  text->fields._stringLength);
  return ss;
}
}
