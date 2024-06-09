#define UNICODE
#define _UNICODE

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <windows.h>
#include <shellapi.h>
#include <tchar.h>

#include "help.hxx"
#include "../info.hxx"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(pCmdLine);

  int nArgs;
  LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
  if (NULL == szArglist) {
    std::wstringstream wss{};
    wss << "Error: Failed to parse argument list.\n\n" << Help << "\n\n" << Break << "\n\n" << Info;
    MessageBoxW(NULL, wss.str().c_str(), L"Elevate", MB_OK);
    return 0;
  }

  std::wstring program{nArgs > 1 ? szArglist[1] : L""};
  std::wstring arguments{pCmdLine};
  if (program.empty()) {
    std::wstringstream wss{};
    wss << "Error: Program path not provided.\n\n" << Help << "\n\n" << Break << "\n\n" << Info;
    MessageBoxW(NULL, wss.str().c_str(), L"Elevate", MB_OK);
    return 0;
  }

  if (arguments.find(L"\"" + program + L"\" ", 0) == 0) {
    arguments = arguments.substr((L"\"" + program + L"\" ").length());
  } else if (arguments.find(program + L" ", 0) == 0) {
    arguments = arguments.substr((program + L" ").length());
  } else {
    arguments = L"";
  }

  INT_PTR retVal = (INT_PTR)ShellExecuteW(NULL, L"runas", program.c_str(), arguments.c_str(), NULL, SW_SHOWDEFAULT);

  // Success
  if (retVal > 32) {
    return 0;
  }
  // The operating system is out of memory or resources.
  if (retVal == 0) {
    return -1;
  }
  return (int)retVal;
}