#include <windows.h>
#include "get_file_version.h"


int main(int argc, char **argv) {
  std::locale::global(std::locale("zh-CN"));

  std::wstring path = L"C:\\Windows\\System32\\cmd.exe";
  printf("%S: %s\n", path.c_str(), GetFileVersion(path).c_str());

  path = L"C:\\Windows\\System32\\regedit.exe";
  printf("%S: %s\n", path.c_str(), GetFileVersion(path).c_str());

  path = L"C:\\Windows\\System32\\calc.exe";
  printf("%S: %s\n", path.c_str(), GetFileVersion(path).c_str());

  path = L"C:\\Windows\\System32\\taskmgr.exe";
  printf("%S: %s\n", path.c_str(), GetFileVersion(path).c_str());

  path = L"C:\\Windows\\System32\\notepad.exe";
  printf("%S: %s\n", path.c_str(), GetFileVersion(path).c_str());

  return 0;
}
