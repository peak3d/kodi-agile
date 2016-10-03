#pragma once
/*
 *      Copyright (C) 2016 Team KODI
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#define INSTANCE_VERSION_ADSP                   "1.0.0"
#define INSTANCE_MIN_VERSION_ADSP               "1.0.0"

#define INSTANCE_VERSION_AUDIODECODER           "1.0.0"
#define INSTANCE_MIN_VERSION_AUDIODECODER       "1.0.0"

#define INSTANCE_VERSION_AUDIOENCODER           "1.0.0"
#define INSTANCE_MIN_VERSION_AUDIOENCODER       "1.0.0"

#define INSTANCE_VERSION_INPUTSTREAM            "1.0.0"
#define INSTANCE_MIN_VERSION_INPUTSTREAM        "1.0.0"

#define INSTANCE_VERSION_PERIPHERAL             "1.0.0"
#define INSTANCE_MIN_VERSION_PERIPHERAL         "1.0.0"

#define INSTANCE_VERSION_PVR                    "1.0.0"
#define INSTANCE_MIN_VERSION_PVR                "1.0.0"

#define INSTANCE_VERSION_SCREENSAVER            "1.0.0"
#define INSTANCE_MIN_VERSION_SCREENSAVER        "1.0.0"

#define INSTANCE_VERSION_VISUALIZATION          "1.0.0"
#define INSTANCE_MIN_VERSION_VISUALIZATION      "1.0.0"

typedef enum ADDON_INSTANCE_TYPE
{
  ADDON_INSTANCE_ADSP = 1,
  ADDON_INSTANCE_AUDIODECODER = 2,
  ADDON_INSTANCE_AUDIOENCODER = 3,
  ADDON_INSTANCE_INPUTSTREAM = 4,
  ADDON_INSTANCE_PERIPHERAL = 5,
  ADDON_INSTANCE_PVR = 6,
  ADDON_INSTANCE_SCREENSAVER = 7,
  ADDON_INSTANCE_VISUALIZATION = 8,
} ADDON_INSTANCE_TYPE;

#ifdef __cplusplus
extern "C" {
namespace kodi {
namespace addon {
#endif

inline void GetInstanceVersions(int instanceType, const char** version, const char** minVersion)
{
  switch (instanceType)
  {
    case ADDON_INSTANCE_ADSP:
      *version = INSTANCE_VERSION_ADSP;
      *minVersion = INSTANCE_MIN_VERSION_ADSP;
      break;
    case ADDON_INSTANCE_AUDIODECODER:
      *version = INSTANCE_VERSION_AUDIODECODER;
      *minVersion = INSTANCE_MIN_VERSION_AUDIODECODER;
      break;
    case ADDON_INSTANCE_AUDIOENCODER:
      *version = INSTANCE_VERSION_AUDIOENCODER;
      *minVersion = INSTANCE_MIN_VERSION_AUDIOENCODER;
      break;
    case ADDON_INSTANCE_INPUTSTREAM:
      *version = INSTANCE_VERSION_INPUTSTREAM;
      *minVersion = INSTANCE_MIN_VERSION_INPUTSTREAM;
      break;
    case ADDON_INSTANCE_PERIPHERAL:
      *version = INSTANCE_VERSION_PERIPHERAL;
      *minVersion = INSTANCE_MIN_VERSION_PERIPHERAL;
      break;
    case ADDON_INSTANCE_PVR:
      *version = INSTANCE_VERSION_PVR;
      *minVersion = INSTANCE_MIN_VERSION_PVR;
      break;
    case ADDON_INSTANCE_SCREENSAVER:
      *version = INSTANCE_VERSION_SCREENSAVER;
      *minVersion = INSTANCE_MIN_VERSION_SCREENSAVER;
      break;
    case ADDON_INSTANCE_VISUALIZATION:
      *version = INSTANCE_VERSION_VISUALIZATION;
      *minVersion = INSTANCE_MIN_VERSION_VISUALIZATION;
      break;
  }
  return;
}

inline const char* GetInstanceName(int instanceType)
{
  switch (instanceType)
  {
    case ADDON_INSTANCE_ADSP:
      return "ADSP";
    case ADDON_INSTANCE_AUDIODECODER:
      return "AudioDecoder";
    case ADDON_INSTANCE_AUDIOENCODER:
      return "AudioEncoder";
    case ADDON_INSTANCE_INPUTSTREAM:
      return "Inputstream";
    case ADDON_INSTANCE_PERIPHERAL:
      return "Peripheral";
    case ADDON_INSTANCE_PVR:
      return "PVR";
    case ADDON_INSTANCE_SCREENSAVER:
      return "ScreenSaver";
    case ADDON_INSTANCE_VISUALIZATION:
      return "Visualization";
  }
  return "unknown";
}

#ifdef __cplusplus
} /* namespace addon */
} /* namespace kodi */
} /* extern "C" */
#endif
