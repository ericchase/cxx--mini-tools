#define UNICODE
#define _UNICODE

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <windows.h>
#include <tchar.h>

#include "help.hxx"
#include "../info.hxx"
#include "../main.hxx"

void makePath(std::filesystem::path const &path) {
  std::filesystem::path parent = path.parent_path();
  if (!parent.empty()) {
    std::filesystem::create_directories(parent);
  }
  std::ofstream file(path, std::ios_base::app);
}

int _tmain(int argc, TCHAR *argv[]) { // requires <tchar.h>
  if (argc > 1) {
    // don't know how to do array.map, so we're using for loop
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