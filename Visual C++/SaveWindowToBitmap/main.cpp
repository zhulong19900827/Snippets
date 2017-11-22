#include <windows.h>
#include "save_window_to_bitmap.h"


int main(int argc, char **argv) {
  bool result = SaveScreenToBitmap(L"screen.bmp");
  if (result) {
    printf("save screen to \"screen.bmp\" sucessfully\n");
  } else {
    printf("failed to save screen to \"screen.bmp\"\n");
  }

  Sleep(2000);

  HWND console_wnd = GetConsoleWindow();
  RECT rect;
  GetWindowRect(console_wnd, &rect);
  result = SaveWindowToBitmap(console_wnd,
                              false,
                              rect.right - rect.left,
                              rect.bottom - rect.top,
                              L"console_wnd.bmp");
  if (result) {
    printf("save console wnd to \"console_wnd.bmp\" sucessfully\n");
  } else {
    printf("failed to save console wnd to \"console_wnd.bmp\"\n");
  }

  GetClientRect(console_wnd, &rect);
  result = SaveWindowToBitmap(console_wnd,
                              true,
                              rect.right - rect.left,
                              rect.bottom - rect.top,
                              L"console_wnd_client.bmp");
  if (result) {
    printf("save console wnd to \"console_wnd_client.bmp\" sucessfully\n");
  }
  else {
    printf("failed to save console wnd to \"console_wnd_client.bmp\"\n");
  }

  return 0;
}
