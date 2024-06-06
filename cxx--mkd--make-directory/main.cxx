#define UNICODE
#define _UNICODE

#include <filesystem>
#include <iostream>
#include <tchar.h>

const char *help() {
  return R"(Creates one or more directories.
Intermediate directories in path will also be created.

mkd path [+ path] [+ ...]

  path    Specifies a relative or absolute directory path.

Examples:
  mk directory ...
  mk path/to/directory ...)";
}

void makePath(std::filesystem::path const &path) {
  std::filesystem::create_directories(path);
}

int _tmain(int argc, TCHAR *argv[]) { // requires <tchar.h>
  if (argc > 1) {
    // don't know how to do array.map, so we're using for loop
    for (int i = 1; i < argc; ++i) { // 0th arg is program path
      makePath(std::filesystem::path{argv[i]});
    }
  } else {
    std::cout << help() << std::endl;
  }

  return 0;
}