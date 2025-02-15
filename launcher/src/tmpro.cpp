#include <MinHook.h>
#include <cstdint>
#include <filesystem>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <windows.h>

#include "flags.h"
#include "game_def.h"

#include "tmpro.h"
#include "helper.hpp"

std::map<std::wstring, std::wstring> gTmproData = {
    {L"Courtroom", L"法庭"},
    {L"Backspace", L"退格键"},
    {L"Return", L"回车键"},
    {L"Tab", L"Tab 键"},
    {L"None", L"无"},
    {L"LeftMeta", L"左 Win 键"},

    {L"Space", L"空格键"},
    {L"LeftButton", L"鼠标左键"},
    {L"RightButton", L"鼠标右键"},
    {L"MiddleButton", L"鼠标中键"},

    // {L"Cross", L"Ⓐ"},
    // {L"Square", L"Ⓧ"},
    // {L"Triangle", L"Ⓨ"},
    // {L"Circle", L"Ⓑ"},
    {L"Cross", L"手柄 A 键"},
    {L"Square", L"手柄 X 键"},
    {L"Triangle", L"手柄 Y 键"},
    {L"Circle", L"手柄 B 键"},
    {L"L1", L"左肩键"},
    {L"R1", L"右肩键"},
    {L"L2", L"左扳机"},
    {L"R2", L"右扳机"},
    {L"L3", L"按下左摇杆"},
    {L"R3", L"按下右摇杆"},

    {L"Normal", L"正常"},
    {L"Fast", L"快速"},
    {L"Instant", L"立即显示「汉化组推荐」"},

    // {L"Normal", L"正常"},
    {L"Large", L"大"},

    {L"Windowed", L"窗口化"},
    {L"Fullscreen", L"全屏"},

    {L"Disabled", L"禁用"},
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

  if constexpr (false && IsDebug) {
    text_out_mutex.lock();
    if (f_text == nullptr) {
      wchar_t buf[2048];
      GetModuleFileNameW(NULL, buf, 2048);
      std::filesystem::path root_path(buf);
      auto file_path = root_path.parent_path() / L"texts.txt";

      _wfopen_s(&f_text, file_path.wstring().c_str(), L"a+");
      fseek(f_text, 0, SEEK_SET);

      constexpr size_t MAX_BUF_SIZE = 2048;
      wchar_t line[MAX_BUF_SIZE];
      while (~fwscanf_s(f_text, L"%ls\n", line, _countof(line))) {
        // std::wcout << "Reading: " << line << std::endl;
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
  } else if constexpr (IsDebug){
    if (text_set.find(ss) == text_set.end()) {
      text_set.insert(ss);
      std::wcout << ss << L"\n";
    }
  }

  if (gTmproData.contains(ss)) {
    auto new_name = gTmproData[ss];

    text = utils::GetSystemString(new_name, text);
  }

  orig_TMPro_TMP_Text__set_text(This, text, method);

  return;
}

int TMPro::init(DWORD base) {
  if (utils::ApplyPatch(
      L"TMPro_TMP_Text__set_text",
      base,
      tgt_TMPro_TMP_Text__set_text,
      hook_TMPro_TMP_Text__set_text,
      orig_TMPro_TMP_Text__set_text) != 0) {
      return 1;
  }
  return 0;
}
