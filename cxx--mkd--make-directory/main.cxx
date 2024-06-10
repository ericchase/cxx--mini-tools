#define UNICODE
#define _UNICODE

#include <filesystem>
#include <iostream>
#include <sstream>

#include <windows.h>
#include <tchar.h>

#include "help.hxx"
#include "../info.hxx"
#include "../main.hxx"

void makePath(std::filesystem::path const &path) {
  std::filesystem::create_directories(path);
}

int _tmain(int argc, TCHAR *argv[]) { // requires <tchar.h>
  SetConsoleOutputCP(65001);          // set console output code page to utf-8

  if (argc > 1) {
    for (int i = 1; i < argc; ++i) { // 0th arg is program path
      makePath(std::filesystem::path{argv[i]});
    }
  } else {
    std::wstringstream wss{};
    wss << Help << "\n\n" << Break << "\n\n" << Info;
    writeOut(wss);
  }
  return 0;
}
