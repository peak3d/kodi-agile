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

#include "../AddonBase.h"
#include <stdint.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The four structures below are used from the headers to create the "C"
 * interface to Kodi. It contains the needed data and for Kodi needed add-on
 * function addresses for the call of them.
 *
 * The sAddonInstance_AudioDecoder structure becomes given to the class creation
 * of CInstanceAudioDecoder(KODI_HANDLE instance) who structure is send from
 * Kodi before.
 *
 * No need of doxygen documentation of the structures here, are only internally
 * used.
 *
 * Note: AUDIODEC_PROPS and sAddonToKodiFuncTable_AudioDecoder are currently
 * unused, but present in lowest structure to have equal with other types and
 * to have values `props`, `toKodi` and `toAddon` present.
 */

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
                        int* bitrate, AudioDataFormat* format,
                        const AudioChannel** info);
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

  //============================================================================
  ///
  /// \addtogroup cpp_kodi_addon_audiodecoder
  /// @brief \cpp_class{ kodi::addon::CInstanceAudioDecoder }
  /// **Audio Decoder add-on instance**
  ///
  /// An audio codec is a add-on capable of coding or decoding a digital data
  /// stream of audio.
  ///
  /// It has the header \ref AudioDecoder.h "#include <kodi/AudioDecoder/AudioDecoder.h>"
  /// be included to enjoy it.
  ///
  /// The interface is small and easy usable, it have minimum two function which
  /// must be present. One for the creation with <b><c>Init()</c></b> and for
  /// the data read with <b><c>ReadPCM()</c></b>.
  ///
  ///
  /// --------------------------------------------------------------------------
  ///
  /// **Here as example what is minimum required to start on a audio decoder:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/AudioDecoder/AudioDecoder.h>
  ///
  /// class CMyAudioDecoder : public ::kodi::addon::CInstanceAudioDecoder
  /// {
  /// public:
  ///   CMyAudioDecoder(void* instance);
  ///
  ///   bool Init(std::string file, unsigned int filecache,
  ///             int& channels, int& samplerate,
  ///             int& bitspersample, int64_t& totaltime,
  ///             int& bitrate, AudioDataFormat& format,
  ///             std::vector<AudioChannel>& channellist) override;
  ///   int ReadPCM(uint8_t* buffer, int size, int& actualsize) override;
  /// };
  ///
  /// CMyAudioDecoder::CMyAudioDecoder(void* instance)
  ///   : CInstanceAudioDecoder(instance)
  /// {
  ///   ...
  /// }
  ///
  /// bool CMyAudioDecoder::Init(std::string file, unsigned int filecache,
  ///                            int& channels, int& samplerate,
  ///                            int& bitspersample, int64_t& totaltime,
  ///                            int& bitrate, AudioDataFormat& format,
  ///                            std::vector<AudioChannel>& channellist)
  /// {
  ///   ...
  ///   return true;
  /// }
  ///
  /// int CMyAudioDecoder::ReadPCM(uint8_t* buffer, int size, int& actualsize)
  /// {
  ///   int length = 0;
  ///   ...
  ///   return length;
  /// }
  ///
  /// /*----------------------------------------------------------------------*/
  ///
  /// class CMyAddon : public ::kodi::addon::CAddonBase
  /// {
  /// public:
  ///   CMyAddon() { }
  ///   ADDON_STATUS CreateInstance(int instanceType,
  ///                               std::string instanceID,
  ///                               KODI_HANDLE instance,
  ///                               KODI_HANDLE& addonInstance) override;
  /// };
  ///
  /// /* If you use only one instance in your add-on, can be instanceType and
  ///  * instanceID ignored */
  /// ADDON_STATUS CMyAddon::CreateInstance(int instanceType,
  ///                                       std::string instanceID,
  ///                                       KODI_HANDLE instance,
  ///                                       KODI_HANDLE& addonInstance)
  /// {
  ///   kodi::Log(LOG_NOTICE, "Creating my Audio Decoder");
  ///   addonInstance = new CMyAudioDecoder(instance);
  ///   return ADDON_STATUS_OK;
  /// }
  ///
  /// ADDONCREATOR(CMyAddon);
  /// ~~~~~~~~~~~~~
  ///
  /// The **desctruction** of the here created example class `CMyAudioDecoder`
  /// becomes done from Kodi's header and never for add-on instances a delete on
  /// add-on itself necessary.
  ///
  //----------------------------------------------------------------------------
  class CInstanceAudioDecoder : public IAddonInstance
  {
  public:
    //==========================================================================
    /// @ingroup cpp_kodi_addon_audiodecoder
    /// @brief Class constructor
    ///
    /// @param[in] instance               The with <b>`kodi::addon::CInstanceAudioDecoderBase::CreateInstance(...)`</b>
    ///                                   given `instance` value.
    ///   @warning Use always only `instance` value from CreateInstance call
    ///
    CInstanceAudioDecoder(KODI_HANDLE instance)
      : IAddonInstance(ADDON_INSTANCE_AUDIODECODER),
        m_instance(static_cast<sAddonInstance_AudioDecoder*>(instance))
    {
      m_instance->toAddon.Init = ADDON_Init;
      m_instance->toAddon.ReadPCM = ADDON_ReadPCM;
      m_instance->toAddon.Seek = ADDON_Seek;
      m_instance->toAddon.ReadTag = ADDON_ReadTag;
      m_instance->toAddon.TrackCount = ADDON_TrackCount;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_audiodecoder
    /// @brief Destructor
    ///
    virtual ~CInstanceAudioDecoder() { }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @ingroup cpp_kodi_addon_audiodecoder
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
                      int& bitrate, AudioDataFormat& format,
                      std::vector<AudioChannel>& channellist)=0;
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @ingroup cpp_kodi_addon_audiodecoder
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
    virtual int ReadPCM(uint8_t* buffer, int size, int& actualsize)=0;
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @ingroup cpp_kodi_addon_audiodecoder
    /// @brief Seek in output stream
    ///
    /// @param[in] time                 Time position to seek to in milliseconds
    /// @return                         Time position seek ended up on
    ///
    virtual int64_t Seek(int64_t time) { return time; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @ingroup cpp_kodi_addon_audiodecoder
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
    /// @ingroup cpp_kodi_addon_audiodecoder
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
                                  int* bitrate, AudioDataFormat* format,
                                  const AudioChannel** info)
    {
      static_cast<CInstanceAudioDecoder*>(addonInstance)->m_channelList.clear();
      bool ret = static_cast<CInstanceAudioDecoder*>(addonInstance)->Init(file, filecache, *channels,
                                                           *samplerate, *bitspersample,
                                                           *totaltime, *bitrate, *format,
                                                           static_cast<CInstanceAudioDecoder*>(addonInstance)->m_channelList);
      if (!static_cast<CInstanceAudioDecoder*>(addonInstance)->m_channelList.empty())
        *info = static_cast<CInstanceAudioDecoder*>(addonInstance)->m_channelList.data();
      else
        *info = nullptr;
      return ret;
    }

    inline static int ADDON_ReadPCM(void* addonInstance, uint8_t* buffer, int size, int* actualsize)
    {
      return static_cast<CInstanceAudioDecoder*>(addonInstance)->ReadPCM(buffer, size, *actualsize);
    }

    inline static int64_t ADDON_Seek(void* addonInstance, int64_t time)
    {
      return static_cast<CInstanceAudioDecoder*>(addonInstance)->Seek(time);
    }

    inline static bool ADDON_ReadTag(void* addonInstance, const char* file, char* title, char* artist, int* length)
    {
      std::string intTitle;
      std::string intArtist;
      memset(title, 0, ADDON_STANDARD_STRING_LENGTH_SMALL);
      memset(artist, 0, ADDON_STANDARD_STRING_LENGTH_SMALL);
      bool ret = static_cast<CInstanceAudioDecoder*>(addonInstance)->ReadTag(file, intTitle, intArtist, *length);
      if (ret)
      {
        strncpy(title, intTitle.c_str(), ADDON_STANDARD_STRING_LENGTH_SMALL-1);
        strncpy(artist, intArtist.c_str(), ADDON_STANDARD_STRING_LENGTH_SMALL-1);
      }
      return ret;
    }

    inline static int ADDON_TrackCount(void* addonInstance, const char* file)
    {
      return static_cast<CInstanceAudioDecoder*>(addonInstance)->TrackCount(file);
    }

    std::vector<AudioChannel> m_channelList;
    sAddonInstance_AudioDecoder* m_instance;
  };

} /* namespace addon */
} /* namespace kodi */
} /* extern "C" */
#endif
