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

#include "AddonBase.h"

#ifdef __cplusplus
extern "C" {
#endif

/* current codec API version */
#define KODI_CODEC_API_VERSION "1.0.0"

typedef unsigned int xbmc_codec_id_t;

typedef enum
{
    XBMC_CODEC_TYPE_UNKNOWN = -1,
    XBMC_CODEC_TYPE_VIDEO,
    XBMC_CODEC_TYPE_AUDIO,
    XBMC_CODEC_TYPE_DATA,
    XBMC_CODEC_TYPE_SUBTITLE,
    XBMC_CODEC_TYPE_RDS,
    XBMC_CODEC_TYPE_NB
} xbmc_codec_type_t;

typedef struct
{
  xbmc_codec_type_t codec_type;
  xbmc_codec_id_t   codec_id;
} xbmc_codec_t;

#define XBMC_INVALID_CODEC_ID   0
#define XBMC_INVALID_CODEC      { XBMC_CODEC_TYPE_UNKNOWN, XBMC_INVALID_CODEC_ID }

typedef struct sAddonToKodiFuncTable_kodi_codec
{
  xbmc_codec_t (*get_codec_by_name)(void* kodiInstance, const char* strCodecName);
} sAddonToKodiFuncTable_kodi_codec;

namespace kodi
{
namespace codec
{

  /*!
   * @brief Get the codec id used by XBMC
   * @param strCodecName The name of the codec
   * @return The codec_id, or a codec_id with 0 values when not supported
   */
  inline xbmc_codec_t GetCodecByName(const char* strCodecName)
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_codec->get_codec_by_name(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, strCodecName);
  }

} /* namespace codec */
} /* namespace kodi */
#ifdef __cplusplus
};
#endif
