#include <MinHook.h>
#include <cstdint>
#include <filesystem>
#include <format>
#include <cstdlib>
#include <cstdio>
#include <ios>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <windows.h>

#include "flags.h"
#include "game_def.h"

#include "tmpro.h"

std::map<std::wstring, std::wstring> tmpro_data = {
    {L"Courtroom", L"法庭"},
    {L"Backspace", L"退格键"},
    {L"Return", L"回车键"},
    {L"Tab", L"Tab 键"},
};

std::set<std::wstring> text_set;

std::mutex text_out_mutex;
FILE* f_text = nullptr;

using TMPro_TMP_Text__set_textT = void(void *, System_String_o *, void *);
intptr_t tgt_TMPro_TMP_Text__set_text = 0x906590;
TMPro_TMP_Text__set_textT *orig_TMPro_TMP_Text__set_text = nullptr;

void hook_TMPro_TMP_Text__set_text(void *This, System_String_o *text,
                                   void *method) {
  auto ss = utils::wstring(text);

  if constexpr (IsDebug) {
    text_out_mutex.lock();
    if (f_text == nullptr) {
      wchar_t buf[2048];
      GetModuleFileNameW(NULL, buf, 2048);
      std::filesystem::path root_path(buf);
      auto file_path = root_path.parent_path() / L"texts.txt";

      _wfopen_s(&f_text, file_path.wstring().c_str(), L"r+");
      fseek(f_text, 0, SEEK_SET);

      constexpr size_t MAX_BUF_SIZE = 2048;
      wchar_t line[MAX_BUF_SIZE];
      while (~fwscanf_s(f_text, L"%ls\n", line, _countof(line))) {
        std::wcout << "Reading: " << line << std::endl;
        text_set.insert(line);
      }
      fseek(f_text, 0, SEEK_END);
    }

    if (text_set.find(ss) == text_set.end()) {
      fwprintf(f_text, L"%s\n", ss.c_str());
      fflush(f_text);
      text_set.insert(ss);
      wprintf_s(L"%ls, len(text_set)=%zu\n", ss.c_str(), text_set.size());
    }
    text_out_mutex.unlock();
  }

  if (tmpro_data.contains(ss)) {
    auto new_name = tmpro_data[ss];

    text = utils::GetSystemString(new_name, text);
  }

  orig_TMPro_TMP_Text__set_text(This, text, method);

  return;
}
int TMPro::init(DWORD base) {
  tgt_TMPro_TMP_Text__set_text += base;
  std::cout << std::format("TMPro_TMP_Text__set_text: {:#x}\n",
                           tgt_TMPro_TMP_Text__set_text);
  if (MH_CreateHook((LPVOID)tgt_TMPro_TMP_Text__set_text,
                    &hook_TMPro_TMP_Text__set_text,
                    (LPVOID *)&orig_TMPro_TMP_Text__set_text) != MH_OK) {
    std::cout << std::format("MH_CreateHook Failed: {:#x}\n",
                             (DWORD)tgt_TMPro_TMP_Text__set_text);
    return 1;
  }

  if (MH_EnableHook((LPVOID)tgt_TMPro_TMP_Text__set_text) != MH_OK) {
    std::cout << std::format("MH_EnableHook Failed\n");
    return 1;
  }

  return 0;
}
