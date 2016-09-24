#pragma once

/*
 *      Copyright (C) 2005-2015 Team Kodi
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
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

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

enum ADDON_STATUS
{
  ADDON_STATUS_OK,
  ADDON_STATUS_LOST_CONNECTION,
  ADDON_STATUS_NEED_RESTART,
  ADDON_STATUS_NEED_SETTINGS,
  ADDON_STATUS_UNKNOWN,
  ADDON_STATUS_NEED_SAVEDSETTINGS,
  ADDON_STATUS_PERMANENT_FAILURE,   /**< permanent failure, like failing to resolve methods */
  ADDON_STATUS_NOT_IMPLEMENTED
};

typedef struct
{
  int           type;
  char*         id;
  char*         label;
  int           current;
  char**        entry;
  unsigned int  entry_elements;
} ADDON_StructSetting;

/*!
 * @brief Handle used to return data from the PVR add-on to CPVRClient
 */
struct ADDON_HANDLE_STRUCT
{
  void *callerAddress;  /*!< address of the caller */
  void *dataAddress;    /*!< address to store data in */
  int   dataIdentifier; /*!< parameter to pass back when calling the callback */
};
typedef ADDON_HANDLE_STRUCT *ADDON_HANDLE;

// To have a on add-on and kodi itself handled string always on known size!
#define ADDON_STANDARD_STRING_LENGTH 1024
#define ADDON_STANDARD_STRING_LENGTH_SMALL 256

typedef enum eAudioDataFormat
{
  AUDIO_FMT_INVALID = -1,

  AUDIO_FMT_U8,

  AUDIO_FMT_S16BE,
  AUDIO_FMT_S16LE,
  AUDIO_FMT_S16NE,

  AUDIO_FMT_S32BE,
  AUDIO_FMT_S32LE,
  AUDIO_FMT_S32NE,

  AUDIO_FMT_S24BE4,
  AUDIO_FMT_S24LE4,
  AUDIO_FMT_S24NE4,    // 24 bits in lower 3 bytes
  AUDIO_FMT_S24NE4MSB, // S32 with bits_per_sample < 32

  AUDIO_FMT_S24BE3,
  AUDIO_FMT_S24LE3,
  AUDIO_FMT_S24NE3, /* S24 in 3 bytes */

  AUDIO_FMT_DOUBLE,
  AUDIO_FMT_FLOAT,

  // Bitstream
  AUDIO_FMT_RAW,

  /* planar formats */
  AUDIO_FMT_U8P,
  AUDIO_FMT_S16NEP,
  AUDIO_FMT_S32NEP,
  AUDIO_FMT_S24NE4P,
  AUDIO_FMT_S24NE4MSBP,
  AUDIO_FMT_S24NE3P,
  AUDIO_FMT_DOUBLEP,
  AUDIO_FMT_FLOATP,

  AUDIO_FMT_MAX
} eAudioDataFormat;

typedef enum eAudioChannel
{
  AUDIO_CH_NULL = -1,
  AUDIO_CH_RAW ,

  AUDIO_CH_FL  , AUDIO_CH_FR , AUDIO_CH_FC , AUDIO_CH_LFE, AUDIO_CH_BL  , AUDIO_CH_BR  , AUDIO_CH_FLOC,
  AUDIO_CH_FROC, AUDIO_CH_BC , AUDIO_CH_SL , AUDIO_CH_SR , AUDIO_CH_TFL , AUDIO_CH_TFR , AUDIO_CH_TFC ,
  AUDIO_CH_TC  , AUDIO_CH_TBL, AUDIO_CH_TBR, AUDIO_CH_TBC, AUDIO_CH_BLOC, AUDIO_CH_BROC,

  /* p16v devices */
  AUDIO_CH_UNKNOWN1 , AUDIO_CH_UNKNOWN2 , AUDIO_CH_UNKNOWN3 , AUDIO_CH_UNKNOWN4 ,
  AUDIO_CH_UNKNOWN5 , AUDIO_CH_UNKNOWN6 , AUDIO_CH_UNKNOWN7 , AUDIO_CH_UNKNOWN8 ,
  AUDIO_CH_UNKNOWN9 , AUDIO_CH_UNKNOWN10, AUDIO_CH_UNKNOWN11, AUDIO_CH_UNKNOWN12,
  AUDIO_CH_UNKNOWN13, AUDIO_CH_UNKNOWN14, AUDIO_CH_UNKNOWN15, AUDIO_CH_UNKNOWN16,
  AUDIO_CH_UNKNOWN17, AUDIO_CH_UNKNOWN18, AUDIO_CH_UNKNOWN19, AUDIO_CH_UNKNOWN20,
  AUDIO_CH_UNKNOWN21, AUDIO_CH_UNKNOWN22, AUDIO_CH_UNKNOWN23, AUDIO_CH_UNKNOWN24,
  AUDIO_CH_UNKNOWN25, AUDIO_CH_UNKNOWN26, AUDIO_CH_UNKNOWN27, AUDIO_CH_UNKNOWN28,
  AUDIO_CH_UNKNOWN29, AUDIO_CH_UNKNOWN30, AUDIO_CH_UNKNOWN31, AUDIO_CH_UNKNOWN32,
  AUDIO_CH_UNKNOWN33, AUDIO_CH_UNKNOWN34, AUDIO_CH_UNKNOWN35, AUDIO_CH_UNKNOWN36,
  AUDIO_CH_UNKNOWN37, AUDIO_CH_UNKNOWN38, AUDIO_CH_UNKNOWN39, AUDIO_CH_UNKNOWN40,
  AUDIO_CH_UNKNOWN41, AUDIO_CH_UNKNOWN42, AUDIO_CH_UNKNOWN43, AUDIO_CH_UNKNOWN44,
  AUDIO_CH_UNKNOWN45, AUDIO_CH_UNKNOWN46, AUDIO_CH_UNKNOWN47, AUDIO_CH_UNKNOWN48,
  AUDIO_CH_UNKNOWN49, AUDIO_CH_UNKNOWN50, AUDIO_CH_UNKNOWN51, AUDIO_CH_UNKNOWN52,
  AUDIO_CH_UNKNOWN53, AUDIO_CH_UNKNOWN54, AUDIO_CH_UNKNOWN55, AUDIO_CH_UNKNOWN56,
  AUDIO_CH_UNKNOWN57, AUDIO_CH_UNKNOWN58, AUDIO_CH_UNKNOWN59, AUDIO_CH_UNKNOWN60,
  AUDIO_CH_UNKNOWN61, AUDIO_CH_UNKNOWN62, AUDIO_CH_UNKNOWN63, AUDIO_CH_UNKNOWN64,

  AUDIO_CH_MAX
} eAudioChannel;

#ifdef __cplusplus
};
#endif

