#define UNICODE
#define _UNICODE

#include <filesystem>
#include <fstream>
#include <iostream>
#include <tchar.h>

char const *const help{
    R"(Creates one or more files.
Intermediate directories in path will also be created.

mkf path [+ path] [+ ...]

  path    Specifies a relative or absolute file path.

Examples:
  mk file ...
  mk path/to/file ...)"};

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
    std::cout << help << std::endl;
  }

  return 0;
}