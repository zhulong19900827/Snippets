#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <windows.h>
#include "get_video_cpature_format_list.h"


std::string GUIDToString(const GUID &guid) {
  char buffer[64];
  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
          guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1],
          guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5],
          guid.Data4[6], guid.Data4[7]);

  return std::string(buffer);
}

int main(int argc, char **argv) {
  std::locale::global(std::locale("zh-CN"));

  CoInitialize(NULL);

  std::vector<VideoCaptureFormatInfo> list;
  GetVideoCaptureFormatList(&list);

  for (size_t i = 0; i < list.size(); ++i) {
    const VideoCaptureFormatInfo &vcfi = list[i];

    printf("friendly name: %S\n", vcfi.firendly_name.c_str());
    printf("major type: %s\n", GUIDToString(vcfi.major_type).c_str());
    printf("sub type: %s\n", GUIDToString(vcfi.sub_type).c_str());
    printf("format type: %s\n", GUIDToString(vcfi.format_type).c_str());
    printf("frame width: %d\n", vcfi.frame_width);
    printf("frame height: %d\n", vcfi.frame_height);
    printf("frame rate: %d\n", vcfi.frame_rate);
    printf("min frame rate: %d\n", vcfi.min_frame_rate);
    printf("max frame rate: %d\n", vcfi.max_frame_rate);
    printf("\n");
  }

  CoUninitialize();

  return 0;
}
