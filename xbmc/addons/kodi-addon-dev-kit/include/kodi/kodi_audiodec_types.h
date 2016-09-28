#pragma once
/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "xbmc_addon_types.h"
#include <stdint.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AUDIODEC_PROPS
{
  int dummy;
} AUDIODEC_PROPS;

typedef struct sAddonToKodiFuncTable_AudioDecoder
{
  void* kodiInstance;
} sAddonToKodiFuncTable_AudioDecoder;

typedef struct sKodiToAddonFuncTable_AudioDecoder
{
  bool (__cdecl* Init) (void* addonInstance, const char* file,
                        unsigned int filecache,
                        int* channels, int* samplerate,
                        int* bitspersample, int64_t* totaltime,
                        int* bitrate, eAudioDataFormat* format,
                        const eAudioChannel** info);
  int  (__cdecl* ReadPCM) (void* addonInstance, uint8_t* buffer, int size, 
                           int* actualsize);
  int64_t  (__cdecl* Seek) (void* addonInstance, int64_t time);
  bool (__cdecl* ReadTag) (void* addonInstance, const char* file, char* title,
                          char* artist, int* length);
  int  (__cdecl* TrackCount) (void* addonInstance, const char* file);
} sKodiToAddonFuncTable_AudioDecoder;

typedef struct sAddonInstance_AudioDecoder
{
  AUDIODEC_PROPS props;
  sAddonToKodiFuncTable_AudioDecoder toKodi;
  sKodiToAddonFuncTable_AudioDecoder toAddon;
} sAddonInstance_AudioDecoder;

#ifdef __cplusplus
namespace kodi {
namespace addon {
namespace audiodecoder {

  class CAddon
  {
  public:
    //==========================================================================
    /// @brief Class constructor
    ///
    /// @param[in] instance             The from Kodi given instance given be
    ///                                 add-on CreateInstance call with instance
    ///                                 id ADDON_INSTANCE_AUDIOENCODER.
    CAddon(void* instance)
      : m_instance(static_cast<sAddonInstance_AudioDecoder*>(instance))
    {
      m_instance->toAddon.Init = ADDON_Init;
      m_instance->toAddon.ReadPCM = ADDON_ReadPCM;
      m_instance->toAddon.Seek = ADDON_Seek;
      m_instance->toAddon.ReadTag = ADDON_ReadTag;
      m_instance->toAddon.TrackCount = ADDON_TrackCount;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Initialize a decoder
    ///
    /// @param[in] file                 The file to read
    /// @param[in] filecache            The file cache size
    /// @param[out] channels            Number of channels in output stream
    /// @param[out] samplerate          Samplerate of output stream
    /// @param[out] bitspersample       Bits per sample in output stream
    /// @param[out] totaltime           Total time for stream
    /// @param[out] bitrate             Average bitrate of input stream
    /// @param[out] format              Data format for output stream
    /// @param[out] channellist         Channel mapping for output stream
    /// @return                         true if successfully done, otherwise
    ///                                 false
    ///
    virtual bool Init(std::string file, unsigned int filecache,
                      int& channels, int& samplerate,
                      int& bitspersample, int64_t& totaltime,
                      int& bitrate, eAudioDataFormat& format,
                      std::vector<eAudioChannel>& channellist) { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Produce some noise
    ///
    /// @param[in] buffer               Output buffer
    /// @param[in] size                 Size of output buffer
    /// @param[out] actualsize          Actual number of bytes written to output buffer
    /// @return                         Return with following possible values:
    ///                                 | Value | Description                  |
    ///                                 |:-----:|:-----------------------------|
    ///                                 |   0   | on success
    ///                                 |  -1   | on end of stream
    ///                                 |   1   | on failure
    ///
    virtual int ReadPCM(uint8_t* buffer, int size, int& actualsize) { return 0; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Seek in output stream
    ///
    /// @param[in] time                 Time position to seek to in milliseconds
    /// @return                         Time position seek ended up on
    ///
    virtual int64_t Seek(int64_t time) { return time; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Read tag of a file
    ///
    /// @param[in] file                 File to read tag for
    /// @param[out] title               Title of file
    /// @param[out] artist              Artist of file
    /// @param[out] length              Length of file
    /// @return                         True on success, false on failure
    ///
    virtual bool ReadTag(std::string file, std::string& title, std::string& artist, int& length) { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Get number of tracks in a file
    ///
    /// @param[in] file                 File to read tag for
    /// @return                         Number of tracks in file
    ///
    virtual int TrackCount(std::string file) { return 1; }
    //--------------------------------------------------------------------------

  private:
    inline static bool ADDON_Init(void* addonInstance, const char* file, unsigned int filecache,
                                  int* channels, int* samplerate,
                                  int* bitspersample, int64_t* totaltime,
                                  int* bitrate, eAudioDataFormat* format,
                                  const eAudioChannel** info)
    {
      static_cast<CAddon*>(addonInstance)->m_channelList.clear();
      bool ret = static_cast<CAddon*>(addonInstance)->Init(file, filecache, *channels,
                                                           *samplerate, *bitspersample, 
                                                           *totaltime, *bitrate, *format, 
                                                           static_cast<CAddon*>(addonInstance)->m_channelList);
      if (!static_cast<CAddon*>(addonInstance)->m_channelList.empty())
        *info = static_cast<CAddon*>(addonInstance)->m_channelList.data();
      else
        *info = nullptr;
      return ret;
    }

    inline static int ADDON_ReadPCM(void* addonInstance, uint8_t* buffer, int size, int* actualsize)
    {
      return static_cast<CAddon*>(addonInstance)->ReadPCM(buffer, size, *actualsize);
    }

    inline static int64_t ADDON_Seek(void* addonInstance, int64_t time)
    {
      return static_cast<CAddon*>(addonInstance)->Seek(time);
    }

    inline static bool ADDON_ReadTag(void* addonInstance, const char* file, char* title, char* artist, int* length)
    {
      std::string intTitle;
      std::string intArtist;
      memset(title, 0, ADDON_STANDARD_STRING_LENGTH_SMALL);
      memset(artist, 0, ADDON_STANDARD_STRING_LENGTH_SMALL);
      bool ret = static_cast<CAddon*>(addonInstance)->ReadTag(file, intTitle, intArtist, *length);
      if (ret)
      {
        strncpy(title, intTitle.c_str(), ADDON_STANDARD_STRING_LENGTH_SMALL-1);
        strncpy(artist, intArtist.c_str(), ADDON_STANDARD_STRING_LENGTH_SMALL-1);
      }
      return ret;
    }

    inline static int ADDON_TrackCount(void* addonInstance, const char* file)
    {
      return static_cast<CAddon*>(addonInstance)->TrackCount(file);
    }

    std::vector<eAudioChannel> m_channelList;
    sAddonInstance_AudioDecoder* m_instance;
  };

} /* namespace audiodecoder */
} /* namespace addon */
} /* namespace kodi */
} /* extern "C" */
#endif
