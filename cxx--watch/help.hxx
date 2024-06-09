auto Help{LR"(Watches a directory for changes.

watch <Path>

  Path - Relative or absolute directory path for target directory.

Standard Output
  <Change Code> <Path>[<Tab><Path>]

    Path - Relative to target directory, except for Change Code 0.
    Tab - The tab key. Usually ascii value \x09.

Change Codes
  S - Started or restarted watching path
      - <Change Code> <Absolute Path>
  C - Created path
  D - Deleted path
  M - Modified path
  R - Renamed path
      - <Change Code> <Old Path><Tab><New Path>

Standard Error
  <Error Code> <Short Description>

Error Codes
  0 - Check Windows System Error Codes for the GetLastError value.
  1 - CreateFile. Could not open target directory for watching.
  2 - ReadDirectoryChanges. Possibly too many changes to track.)"};