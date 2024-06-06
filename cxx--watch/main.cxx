#define UNICODE
#define _UNICODE

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tchar.h>
#include <windows.h>

std::filesystem::path resolvePath(std::filesystem::path const &path) {
  TCHAR lpBuffer[1]{};
  // requiredSize includes space for the terminating null character
  DWORD requiredSize = GetFullPathNameW(path.c_str(), 1, lpBuffer, NULL);
  if (requiredSize > 0) {
    TCHAR *lpBuffer = new TCHAR[requiredSize]{};
    // copiedSize does NOT include the terminating null character
    DWORD copiedSize = GetFullPathNameW(path.c_str(), requiredSize, lpBuffer, NULL);
    return std::filesystem::path{lpBuffer};
  }
  return std::filesystem::path{""};
}

/* Taken from https://gist.github.com/nickav/a57009d4fcc3b527ed0f5c9cf30618f8
Assuming we are allowed to use this as it was presented as example code. */
int watchDirectory(std::filesystem::path const &path) {
  DWORD const buf_size{1024 * 16};
  alignas(DWORD) uint8_t change_buf[buf_size]{};
  OVERLAPPED overlapped{};
  overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);
  HANDLE hDirectory{INVALID_HANDLE_VALUE};
  BOOL bSuccess{0};
  int restart_delay_ms{1000};

  std::wcout << "0 " << path.c_str() << std::endl;

_Start:
  while (true) {
    // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
    hDirectory = CreateFileW( //
        path.c_str(),         //
        FILE_LIST_DIRECTORY,  //  For a directory, the right to list the contents of the directory.
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, //
        NULL,                                                   //
        OPEN_EXISTING,                                          //
        FILE_FLAG_BACKUP_SEMANTICS  // You must set this flag to obtain a handle to a directory.
            | FILE_FLAG_OVERLAPPED, // The file or device is being opened or created for asynchronous I/O.
        NULL                        //
    );
    if (hDirectory == INVALID_HANDLE_VALUE)
      goto _Exception_CreateFile;
    // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-readdirectorychangesw
    bSuccess = ReadDirectoryChangesW( //
        hDirectory,                   //
        change_buf, // A pointer to the DWORD-aligned formatted buffer in which the read results are to be returned.
        buf_size,   // The size of the buffer that is pointed to by the lpBuffer parameter, in bytes.
        TRUE,       // If this parameter is TRUE, the function monitors the directory tree.
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, //
        NULL,        // For asynchronous calls, this parameter is undefined.
        &overlapped, // A pointer to an OVERLAPPED structure that supplies data to be used during asynchronous
                     // operation.
        NULL // A pointer to a completion routine to be called when the operation has been completed or canceled...
    );
    if (bSuccess == 0)
      goto _Exception_ReadDirectoryChanges;

    // https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
    DWORD result{WaitForSingleObject(overlapped.hEvent, 1000)};
    if (result == WAIT_OBJECT_0) {
      std::wstringstream out_changes{};
      DWORD bytes_transferred{};
      // https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-getoverlappedresult
      GetOverlappedResult(hDirectory, &overlapped, &bytes_transferred, FALSE);
      // https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-file_notify_information
      FILE_NOTIFY_INFORMATION *event{(FILE_NOTIFY_INFORMATION *)change_buf};
      for (;;) {
        switch (event->Action) {
        case FILE_ACTION_ADDED:
          out_changes << "1 ";
          break;
        case FILE_ACTION_REMOVED:
          out_changes << "2 ";
          break;
        case FILE_ACTION_MODIFIED:
          out_changes << "3 ";
          break;
        case FILE_ACTION_RENAMED_OLD_NAME:
          out_changes << "4 ";
          break;
        case FILE_ACTION_RENAMED_NEW_NAME:
          out_changes << "\x09";
          break;
        }
        DWORD name_len{event->FileNameLength / sizeof(wchar_t)};
        out_changes << std::wstring{event->FileName, name_len};

        // Are there more events to handle?
        if (event->NextEntryOffset) {
          *((uint8_t **)&event) += event->NextEntryOffset;
        } else {
          break;
        }
      }
      std::wcout << out_changes.str() << std::endl;
    }
    CloseHandle(hDirectory);
  }

_Exception_CreateFile:
  std::cerr << "1 CreateFile. Could not open target directory for watching." << std::endl;
  Sleep(restart_delay_ms);
  goto _Start;

_Exception_ReadDirectoryChanges:
  std::cerr << "2 ReadDirectoryChanges. Possibly too many changes to track." << std::endl;
  Sleep(restart_delay_ms);
  goto _Start;

_Exit:
  CloseHandle(hDirectory);
  return 0;
}

const char *help() {
  return R"(Watches a directory for changes.

watch <Path>

  Path - Relative or absolute directory path for target directory.

Standard Output
  <Change Code> <Path>[<Tab><Path>]

    Path - Relative to target directory, except for Change Code 0.
    Tab - The tab key. Usually ascii value \x09.

Change Codes
  0 - Watching
      - <Change Code> <Absolute Path>
  1 - Added
  2 - Removed
  3 - Modified
  4 - Renamed 
      - <Change Code> <Old Path><Tab><New Path>

Standard Error
  <Error Code> <Short Description>

Error Codes
  1 - CreateFile. Could not open target directory for watching.
  2 - ReadDirectoryChanges. Possibly too many changes to track.)";
}

int _tmain(int argc, TCHAR *argv[]) { // requires <tchar.h>
  if (argc > 1) {
    return watchDirectory(resolvePath(std::filesystem::path{argv[1]}));
  } else {
    std::wcout << help() << std::endl;
  }
  return 0;
}