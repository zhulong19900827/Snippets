#include "save_window_to_bitmap.h"


bool SaveWindowToBitmap(HWND wnd,
                        bool client,
                        int width,
                        int height,
                        const std::wstring &path) {
  bool successful = false;

  HDC wnd_dc = client ? GetDC(wnd) : GetWindowDC(wnd);
  if (NULL == wnd_dc) return successful;

  HDC compatible_dc = CreateCompatibleDC(wnd_dc);
  if (NULL == compatible_dc) goto failed;

  HBITMAP compatible_bitmap = CreateCompatibleBitmap(wnd_dc, width, height);
  if (NULL == compatible_bitmap) goto failed1;

  HGDIOBJ original_bitmap_obj = SelectObject(compatible_dc, compatible_bitmap);
  if (NULL == original_bitmap_obj) goto failed2;

  BOOL result = BitBlt(compatible_dc, 0, 0, width, height, wnd_dc, 0, 0,
                       SRCCOPY);
  if (!result) goto failed3;

  BITMAP bitmap;
  int result1 = GetObject(compatible_bitmap, sizeof(BITMAP), &bitmap);
  if (!result1) goto failed3;

  BITMAPINFOHEADER bitmap_info_header;
  bitmap_info_header.biSize = sizeof(BITMAPINFOHEADER);
  bitmap_info_header.biWidth = bitmap.bmWidth;
  bitmap_info_header.biHeight = bitmap.bmHeight;
  bitmap_info_header.biPlanes = 1;
  bitmap_info_header.biBitCount = 32;
  bitmap_info_header.biCompression = BI_RGB;
  bitmap_info_header.biSizeImage = 0;
  bitmap_info_header.biXPelsPerMeter = 0;
  bitmap_info_header.biYPelsPerMeter = 0;
  bitmap_info_header.biClrUsed = 0;
  bitmap_info_header.biClrImportant = 0;

  DWORD bitmap_data_size = ((bitmap.bmWidth * bitmap_info_header.biBitCount +
    31) / 32) * 4 * bitmap.bmHeight;
  HANDLE bitmap_data_buffer = GlobalAlloc(GHND, bitmap_data_size);
  if (NULL == bitmap_data_buffer) goto failed3;

  char *bitmap_data = (char *)GlobalLock(bitmap_data_buffer);
  if (NULL == bitmap_data) goto failed4;

  result1 = GetDIBits(wnd_dc, compatible_bitmap, 0, bitmap.bmHeight, bitmap_data,
                      (BITMAPINFO *)&bitmap_info_header, DIB_RGB_COLORS);
  if (!result1) goto failed5;

  HANDLE file = CreateFile(path.c_str(),
                           GENERIC_WRITE,
                           0,
                           NULL,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);
  if (INVALID_HANDLE_VALUE == file) goto failed5;

  BITMAPFILEHEADER bitmap_file_header;
  bitmap_file_header.bfType = 0x4D42;
  bitmap_file_header.bfSize = sizeof(BITMAPFILEHEADER) +
    sizeof(BITMAPINFOHEADER) + bitmap_data_size;
  bitmap_file_header.bfOffBits = sizeof(BITMAPFILEHEADER) +
    sizeof(BITMAPINFOHEADER);

  DWORD written = 0;
  result = WriteFile(file, (LPSTR)&bitmap_file_header, sizeof(BITMAPFILEHEADER),
                     &written, NULL);
  if (!result) goto failed6;

  result = WriteFile(file, (LPSTR)&bitmap_info_header, sizeof(BITMAPINFOHEADER),
                     &written, NULL);
  if (!result) goto failed6;

  result = WriteFile(file, (LPSTR)bitmap_data, bitmap_data_size, &written,
                     NULL);
  if (!result) goto failed6;

  successful = true;

failed6:
  CloseHandle(file);
failed5:
  GlobalUnlock(bitmap_data_buffer);
failed4:
  GlobalFree(bitmap_data_buffer);
failed3:
  SelectObject(compatible_dc, original_bitmap_obj);
failed2:
  DeleteObject(compatible_bitmap);
failed1:
  DeleteDC(compatible_dc);
failed:
  ReleaseDC(wnd, wnd_dc);

  return successful;
}

bool SaveScreenToBitmap(const std::wstring &path) {
  return SaveWindowToBitmap(GetDesktopWindow(),
                            false,
                            GetSystemMetrics(SM_CXSCREEN),
                            GetSystemMetrics(SM_CYSCREEN),
                            path);
}
