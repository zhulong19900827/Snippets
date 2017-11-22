#ifndef GET_FILE_VERSION_H_
#define GET_FILE_VERSION_H_


#include <string>


bool GetFileVersionNumber(const wchar_t *path,
                          unsigned short *a,
                          unsigned short *b,
                          unsigned short *c,
                          unsigned short *d);

std::string GetFileVersion(const std::wstring &path);


#endif  // GET_FILE_VERSION_H_
