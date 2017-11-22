#ifndef SAVE_WINDOW_TO_BITMAP_H_
#define SAVE_WINDOW_TO_BITMAP_H_


#include <windows.h>
#include <string>


bool SaveWindowToBitmap(HWND wnd,
                        bool client,
                        int width,
                        int height,
                        const std::wstring &path);

bool SaveScreenToBitmap(const std::wstring &path);


#endif  // SAVE_WINDOW_TO_BITMAP_H_
