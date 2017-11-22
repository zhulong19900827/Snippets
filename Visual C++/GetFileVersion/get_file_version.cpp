#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif


#include "get_file_version.h"
#include <windows.h>


#pragma comment(lib, "version.lib")


bool GetFileVersionNumber(const wchar_t *path,
                          unsigned short *a,
                          unsigned short *b,
                          unsigned short *c,
                          unsigned short *d) {
  if (NULL == path) return false;

  DWORD size = GetFileVersionInfoSize(path, NULL);
  if (0 == size) return false;

  bool result = false;
  unsigned char *buffer = new unsigned char[size];

  BOOL result1 = GetFileVersionInfo(path, 0, size, buffer);
  if (result1) {
    UINT length = 0;
    VS_FIXEDFILEINFO *fixed_file_info = NULL;

    result1 = VerQueryValue(buffer, L"\\", (void **)&fixed_file_info, &length);
    if (result1) {
      if (a != NULL) *a = HIWORD(fixed_file_info->dwFileVersionMS);
      if (b != NULL) *b = LOWORD(fixed_file_info->dwFileVersionMS);
      if (c != NULL) *c = HIWORD(fixed_file_info->dwFileVersionLS);
      if (d != NULL) *d = LOWORD(fixed_file_info->dwFileVersionLS);

      result = true;
    }
  }

  delete[] buffer;
  return result;
}

std::string GetFileVersion(const std::wstring &path) {
  unsigned short a = 0, b = 0, c = 0, d = 0;
  bool result = GetFileVersionNumber(path.c_str(), &a, &b, &c, &d);
  if (result) {
    char buffer[64] = {0};
    sprintf(buffer, "%hu.%hu.%hu.%hu", a, b, c, d);
    return std::string(buffer);
  } else {
    return std::string();
  }
}
