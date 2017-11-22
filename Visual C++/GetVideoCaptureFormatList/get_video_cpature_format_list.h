#ifndef GET_VIDEO_CAPTURE_FORMAT_LIST_H_
#define GET_VIDEO_CAPTURE_FORMAT_LIST_H_


#include <dshow.h>
#include <string>
#include <vector>


struct VideoCaptureFormatInfo {
  std::wstring firendly_name;
  GUID major_type;
  GUID sub_type;
  GUID format_type;
  unsigned int frame_width;
  unsigned int frame_height;
  unsigned int frame_rate;
  unsigned int min_frame_rate;
  unsigned int max_frame_rate;
};


bool GetVideoCaptureFormatList(std::vector<VideoCaptureFormatInfo> *list);


#endif  // GET_VIDEO_CAPTURE_FORMAT_LIST_H_
