#include "get_video_cpature_format_list.h"
#include <dvdmedia.h>


#pragma comment(lib, "strmiids.lib")


template <class T>
void SafeRelease(T **p) {
  if (p != NULL && *p != NULL) {
    (*p)->Release();
    *p = NULL;
  }
}

void SafeDeleteMediaType(AM_MEDIA_TYPE **am_media_type) {
  if (am_media_type != NULL && *am_media_type != NULL) {
    if ((*am_media_type)->cbFormat != 0) {
      CoTaskMemFree((LPVOID)(*am_media_type)->pbFormat);
      (*am_media_type)->pbFormat = NULL;
    }

    if ((*am_media_type)->pUnk != NULL) {
      (*am_media_type)->pUnk->Release();
      (*am_media_type)->pUnk = NULL;
    }

    CoTaskMemFree((LPVOID)*am_media_type);
    *am_media_type = NULL;
  }
}

bool GetVideoCaptureFormatList(std::vector<VideoCaptureFormatInfo> *list) {
  ICreateDevEnum *create_dev_enum = NULL;
  HRESULT result = CoCreateInstance(CLSID_SystemDeviceEnum,
                                    NULL,
                                    CLSCTX_INPROC_SERVER,
                                    IID_ICreateDevEnum,
                                    (void **)&create_dev_enum);
  if (FAILED(result)) return false;

  IEnumMoniker *enum_moniker = NULL;
  result = create_dev_enum->CreateClassEnumerator(
      CLSID_VideoInputDeviceCategory,
      &enum_moniker,
      0);
  if (FAILED(result)) {
    SafeRelease(&create_dev_enum);
    return false;
  }

  IMoniker *moniker = NULL;
  while (S_OK == enum_moniker->Next(1, &moniker, NULL)) {
    IPropertyBag *property_bag = NULL;
    result = moniker->BindToStorage(NULL, NULL, IID_IPropertyBag,
                                    (void **)&property_bag);
    if (FAILED(result)) {
      SafeRelease(&moniker);
      continue;
    }

    std::wstring friendly_name;
    VARIANT var;
    VariantInit(&var);
    result = property_bag->Read(L"FriendlyName", &var, NULL);
    if (SUCCEEDED(result)) {
      friendly_name = var.bstrVal;
      VariantClear(&var);
    }

    SafeRelease(&property_bag);

    if (friendly_name.empty()) {
      SafeRelease(&moniker);
      continue;
    }

    IBaseFilter *base_filter = NULL;
    result = moniker->BindToObject(NULL, NULL, IID_IBaseFilter,
                                   (void **)&base_filter);
    if (FAILED(result)) {
      SafeRelease(&moniker);
      continue;
    }

    IEnumPins *enum_pins = NULL;
    result = base_filter->EnumPins(&enum_pins);
    if (FAILED(result)) {
      SafeRelease(&base_filter);
      SafeRelease(&moniker);
      continue;
    }

    IPin *pin = NULL;
    while (S_OK == enum_pins->Next(1, &pin, NULL)) {
      IAMStreamConfig *am_stream_config = NULL;
      result = pin->QueryInterface(IID_IAMStreamConfig,
                                   (void **)&am_stream_config);
      if (FAILED(result)) {
        SafeRelease(&pin);
        continue;
      }

      int count = 0;
      int size = 0;
      result = am_stream_config->GetNumberOfCapabilities(&count, &size);
      if (SUCCEEDED(result)) {
        for (int i = 0; i < count; ++i) {
          AM_MEDIA_TYPE *am_media_type = NULL;
          BYTE *buffer = new BYTE[size];
          memset(buffer, 0, size);
          am_stream_config->GetStreamCaps(i, &am_media_type, buffer);
          VIDEO_STREAM_CONFIG_CAPS *vscc = (VIDEO_STREAM_CONFIG_CAPS *)buffer;

          VideoCaptureFormatInfo vcfi;
          vcfi.firendly_name = friendly_name;

          memcpy((void *)&vcfi.major_type, (void *)&am_media_type->majortype,
                 sizeof(vcfi.major_type));
          memcpy((void *)&vcfi.sub_type, (void *)&am_media_type->subtype,
                 sizeof(vcfi.sub_type));
          memcpy((void *)&vcfi.format_type, (void *)&am_media_type->formattype,
                 sizeof(vcfi.format_type));

          if (am_media_type->formattype == FORMAT_VideoInfo) {
            if (am_media_type->cbFormat >= sizeof(VIDEOINFOHEADER)) {
              VIDEOINFOHEADER *vih = (VIDEOINFOHEADER *)am_media_type->pbFormat;
              vcfi.frame_width = vih->bmiHeader.biWidth;
              vcfi.frame_height = vih->bmiHeader.biHeight;
              LONGLONG frame_rate = 10000000i64 / vih->AvgTimePerFrame;
              vcfi.frame_rate = (unsigned int)frame_rate;
            }
          } else if (am_media_type->formattype == FORMAT_VideoInfo2) {
            if (am_media_type->cbFormat >= sizeof(VIDEOINFOHEADER2)) {
              VIDEOINFOHEADER2 *vih = (VIDEOINFOHEADER2 *)am_media_type->pbFormat;
              vcfi.frame_width = vih->bmiHeader.biWidth;
              vcfi.frame_height = vih->bmiHeader.biHeight;
              LONGLONG frame_rate = 10000000i64 / vih->AvgTimePerFrame;
              vcfi.frame_rate = (unsigned int)frame_rate;
            }
          } else {
            vcfi.frame_width = 0;
            vcfi.frame_height = 0;
            vcfi.frame_rate = 0;
          }

          LONGLONG min_frame_rate = 10000000i64 / vscc->MaxFrameInterval;
          vcfi.min_frame_rate = (unsigned int)min_frame_rate;

          LONGLONG max_frame_rate = 10000000i64 / vscc->MinFrameInterval;
          vcfi.max_frame_rate = (unsigned int)max_frame_rate;

          if (list != NULL) list->push_back(vcfi);

          SafeDeleteMediaType(&am_media_type);
          delete[] buffer;
        }
      }

      SafeRelease(&am_stream_config);
      SafeRelease(&pin);
    }

    SafeRelease(&enum_pins);
    SafeRelease(&base_filter);
    SafeRelease(&moniker);
  }

  SafeRelease(&enum_moniker);
  SafeRelease(&create_dev_enum);
  return true;
}
