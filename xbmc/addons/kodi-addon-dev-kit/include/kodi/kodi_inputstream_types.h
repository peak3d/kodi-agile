#pragma once

/*
 *      Copyright (C) 2005-2016 Team Kodi
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

#ifdef TARGET_WINDOWS
#include <windows.h>
#else
#ifndef __cdecl
#define __cdecl
#endif
#ifndef __declspec
#define __declspec(X)
#endif
#endif

#ifdef BUILD_KODI_ADDON
#include "DVDDemuxPacket.h"
#else
#include "cores/VideoPlayer/DVDDemuxers/DVDDemuxPacket.h"
#endif

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

/* current API version */
#define INPUTSTREAM_API_VERSION "1.0.5"

extern "C" {

/* this are properties given to the addon on create
 * at this time we have no parameters for the addon
 * (used internal on header)
 */
typedef struct INPUTSTREAM_PROPS
{
  int dummy;
} INPUTSTREAM_PROPS;

/*
 * structure of key/value pairs passed to addon on Open() (used internal on header)
 */
typedef struct INPUTSTREAM
{
  static const unsigned int MAX_INFO_COUNT = 8;

  const char *m_strURL;

  unsigned int m_nCountInfoValues;
  struct LISTITEMPROPERTY
  {
    const char *m_strKey;
    const char *m_strValue;
  } m_ListItemProperties[MAX_INFO_COUNT];

  const char *m_libFolder;
  const char *m_profileFolder;
} INPUTSTREAM;

/*
 * Array of stream IDs (used internal on header)
 */
typedef struct INPUTSTREAM_IDS
{
  static const unsigned int MAX_STREAM_COUNT = 32;
  unsigned int m_streamCount;
  unsigned int m_streamIds[MAX_STREAM_COUNT];
} INPUTSTREAM_IDS;


/*!
  * @brief InputStream add-on capabilities. All capabilities are set to "false" as default.
  */
typedef struct INPUTSTREAM_CAPABILITIES
{
  bool m_supportsIDemux;                  /*!< @brief supports interface IDemux */
  bool m_supportsIPosTime;                /*!< @brief supports interface IPosTime */
  bool m_supportsIDisplayTime;            /*!< @brief supports interface IDisplayTime */
  bool m_supportsSeek;                    /*!< @brief supports seek */
  bool m_supportsPause;                   /*!< @brief supports pause */
} INPUTSTREAM_CAPABILITIES;

/*!
  * @brief stream properties
  */
typedef struct INPUTSTREAM_INFO
{
  enum STREAM_TYPE
  {
    TYPE_NONE,
    TYPE_VIDEO,
    TYPE_AUDIO,
    TYPE_SUBTITLE,
    TYPE_TELETEXT
  } m_streamType;

  char m_codecName[32];                /*!< @brief (required) name of codec according to ffmpeg */
  char m_codecInternalName[32];        /*!< @brief (optional) internal name of codec (selectionstream info) */
  unsigned int m_pID;                  /*!< @brief (required) physical index */
  unsigned int m_Bandwidth;            /*!< @brief (optional) bandwidth of the stream (selectionstream info) */

  const uint8_t *m_ExtraData;
  unsigned int m_ExtraSize;

  char m_language[4];                  /*!< @brief ISO 639 3-letter language code (empty string if undefined) */

  unsigned int m_FpsScale;             /*!< @brief Scale of 1000 and a rate of 29970 will result in 29.97 fps */
  unsigned int m_FpsRate;
  unsigned int m_Height;               /*!< @brief height of the stream reported by the demuxer */
  unsigned int m_Width;                /*!< @brief width of the stream reported by the demuxer */
  float m_Aspect;                      /*!< @brief display aspect of stream */

  unsigned int m_Channels;             /*!< @brief (required) amount of channels */
  unsigned int m_SampleRate;           /*!< @brief (required) sample rate */
  unsigned int m_BitRate;              /*!< @brief (required) bit rate */
  unsigned int m_BitsPerSample;        /*!< @brief (required) bits per sample */
  unsigned int m_BlockAlign;
} INPUTSTREAM_INFO;

typedef struct sAddonToKodiFuncTable_InputStream
{
  void* kodiInstance;
  void (*FreeDemuxPacket)(void *kodiInstance, DemuxPacket* pPacket);
  DemuxPacket* (*AllocateDemuxPacket)(void *kodiInstance, int iDataSize);
} sAddonToKodiFuncTable_InputStream;

typedef struct sKodiToAddonFuncTable_InputStream
{
  void (__cdecl* GetCapabilities)(void* addonInstance, INPUTSTREAM_CAPABILITIES*);

  bool (__cdecl* Open)(void* addonInstance, INPUTSTREAM*);
  void (__cdecl* Close)(void* addonInstance);
  const char* (__cdecl* GetPathList)(void* addonInstance);

  // IDemux
  void (__cdecl* GetStreamIds)(void* addonInstance, struct INPUTSTREAM_IDS* ids);
  void (__cdecl* GetStream)(void* addonInstance, int streamId, INPUTSTREAM_INFO* info);
  void (__cdecl* EnableStream)(void* addonInstance, int, bool);
  void (__cdecl* DemuxReset)(void* addonInstance);
  void (__cdecl* DemuxAbort)(void* addonInstance);
  void (__cdecl* DemuxFlush)(void* addonInstance);
  DemuxPacket* (__cdecl* DemuxRead)(void* addonInstance);
  bool (__cdecl* DemuxSeekTime)(void* addonInstance, int, bool, double*);
  void (__cdecl* DemuxSetSpeed)(void* addonInstance, int);
  void (__cdecl* SetVideoResolution)(void* addonInstance, int, int);

  // IDisplayTime
  int (__cdecl* GetTotalTime)(void* addonInstance);
  int (__cdecl* GetTime)(void* addonInstance);

  // IPosTime
  bool (__cdecl* PosTime)(void* addonInstance, int);

  // Seekable (mandatory)
  bool (__cdecl* CanPauseStream)(void* addonInstance);
  bool (__cdecl* CanSeekStream)(void* addonInstance);

  int (__cdecl* ReadStream)(void* addonInstance, uint8_t*, unsigned int);
  int64_t(__cdecl* SeekStream)(void* addonInstance, int64_t, int);
  int64_t (__cdecl* PositionStream)(void* addonInstance);
  int64_t (__cdecl* LengthStream)(void* addonInstance);
  void (__cdecl* PauseStream)(void* addonInstance, double);
  bool (__cdecl* IsRealTimeStream)(void* addonInstance);
} sKodiToAddonFuncTable_InputStream;

typedef struct sFuncTable_InputStream
{
  INPUTSTREAM_PROPS props;
  sAddonToKodiFuncTable_InputStream toKodi;
  sKodiToAddonFuncTable_InputStream toAddon;
} sFuncTable_InputStream;


#ifdef __cplusplus
namespace kodi {
namespace addon {
namespace inputstream {

  class CAddon
  {
  public:
    //==========================================================================
    /// @brief Class constructor
    ///
    /// @param[in] instance             The from Kodi given instance given be
    ///                                 add-on CreateInstance call with instance
    ///                                 id ADDON_INSTANCE_INPUTSTREAM.
    ///
    CAddon(void* instance)
      : m_instance(static_cast<sFuncTable_InputStream*>(instance))
    {
      m_instance->toAddon.GetCapabilities = ADDON_GetCapabilities;

      m_instance->toAddon.Open = ADDON_Open;
      m_instance->toAddon.Close = ADDON_Close;
      m_instance->toAddon.GetPathList = ADDON_GetPathList;

      // IDemux
      m_instance->toAddon.GetStreamIds = ADDON_GetStreamIds;
      m_instance->toAddon.GetStream = ADDON_GetStream;
      m_instance->toAddon.EnableStream = ADDON_EnableStream;
      m_instance->toAddon.DemuxReset = ADDON_DemuxReset;
      m_instance->toAddon.DemuxAbort = ADDON_DemuxAbort;
      m_instance->toAddon.DemuxFlush = ADDON_DemuxFlush;
      m_instance->toAddon.DemuxRead = ADDON_DemuxRead;
      m_instance->toAddon.DemuxSeekTime = ADDON_DemuxSeekTime;
      m_instance->toAddon.DemuxSetSpeed = ADDON_DemuxSetSpeed;
      m_instance->toAddon.SetVideoResolution = ADDON_SetVideoResolution;

      // IDisplayTime
      m_instance->toAddon.GetTime = ADDON_GetTime;
      m_instance->toAddon.GetTotalTime = ADDON_GetTotalTime;

      // IPosTime
      m_instance->toAddon.PosTime = ADDON_PosTime;

      // Seekable (mandatory)
      m_instance->toAddon.CanPauseStream = ADDON_CanPauseStream;
      m_instance->toAddon.CanSeekStream = ADDON_CanSeekStream;

      m_instance->toAddon.ReadStream = ADDON_ReadStream;
      m_instance->toAddon.SeekStream = ADDON_SeekStream;
      m_instance->toAddon.PositionStream = ADDON_PositionStream;
      m_instance->toAddon.LengthStream = ADDON_LengthStream;
      m_instance->toAddon.PauseStream = ADDON_PauseStream;
      m_instance->toAddon.IsRealTimeStream = ADDON_IsRealTimeStream;
    }
    //--------------------------------------------------------------------------
    
    //==========================================================================
    /// @brief Get Capabilities of this addon instance (**required**)
    ///
    /// @param[out] capabilities        The supported capabilities of this
    ///                                 class, see @ref INPUTSTREAM_CAPABILITIES
    ///                                 for possible values.
    ///
    virtual void GetCapabilities(INPUTSTREAM_CAPABILITIES& capabilities)=0;
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Open a stream (**required**)
    ///
    /// @param[in] props
    /// @return                         True if the stream has been opened 
    ///                                 successfully, false otherwise.
    ///
    virtual bool Open(const std::string& strURL, const std::map<std::string, std::string>& listItemProperties)=0;
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Close an open stream (**optional**)
    ///
    virtual void Close() { }
    //--------------------------------------------------------------------------
  
    //==========================================================================
    /// @brief Get path/url for this addon (**optional**)
    ///
    /// @param[out] pathList            todo
    ///
    virtual void GetPathList(std::string& pathList) { }
    //--------------------------------------------------------------------------

    
    // IDemux
    ///@{

    //==========================================================================
    /// @brief Get IDs of available streams
    ///
    /// @param[out] ids                 list where available stream ids becomes
    ///                                 inserted
    ///
    virtual void GetStreamIds(std::vector<int>& ids) { }
  
    //==========================================================================
    /// @brief Get properties of a stream.
    ///
    /// @param[in] streamId             unique id of stream
    /// @param[out] info                properties of a stream to add
    /// 
    virtual void GetStream(int streamId, INPUTSTREAM_INFO& info) { }

    //==========================================================================
    /// @brief Enable or disable a stream.
    ///
    /// A disabled stream does not send demux packets
    ///
    /// @param[in] streamId              unique id of stream
    /// @param[in] enable                true for enable, false for disable
    ///
    /// @remarks
    ///
    virtual void EnableStream(int streamid, bool enable) { }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Reset the demultiplexer in the add-on.
    ///
    /// @remarks Required if bHandlesDemuxing is set to true.
    ///
    virtual void DemuxReset() { }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Abort the demultiplexer thread in the add-on.
    ///
    /// @remarks Required if bHandlesDemuxing is set to true.
    ///
    virtual void DemuxAbort() { }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Flush all data that's currently in the demultiplexer buffer in the add-on.
    ///
    /// @remarks Required if bHandlesDemuxing is set to true.
    ///
    virtual void DemuxFlush() { }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Read the next packet from the demultiplexer, if there is one.
    ///
    /// @return                         The next packet.
    ///       If there is no next packet, then the add-on should return the
    ///       packet created by calling AllocateDemuxPacket(0) on the callback.
    ///       If the stream changed and Kodi's player needs to be reinitialised,
    ///       then, the add-on should call AllocateDemuxPacket(0) on the
    ///       callback, and set the streamid to DMX_SPECIALID_STREAMCHANGE and
    ///       return the value.
    ///       The add-on should return NULL if an error occured.
    ///
    /// @remarks Return NULL if this add-on won't provide this function.
    ///
    virtual DemuxPacket* DemuxRead() { return nullptr; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Notify the InputStream addon/demuxer that Kodi wishes to seek the
    /// stream by time Demuxer is required to set stream to an IDR frame
    ///
    /// @param[in] time                 The absolute time since stream start
    /// @param[in] backwards            True to seek to keyframe BEFORE time, 
    ///                                 else AFTER
    /// @param[out] startpts            can be updated to point to where display
    ///                                 should start
    /// @return                         True if the seek operation was possible
    ///
    /// @remarks Optional, and only used if addon has its own demuxer.
    ///
    virtual bool DemuxSeekTime(int time, bool backwards, double& startpts) { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Notify the InputStream addon/demuxer that Kodi wishes to change
    /// playback speed
    ///
    /// @param[in] speed                The requested playback speed
    ///
    /// @remarks Optional, and only used if addon has its own demuxer.
    ///
    virtual void DemuxSetSpeed(int speed) { }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Sets desired width / height
    ///
    /// @param[in] width
    /// @param[in] hight
    ///
    virtual void SetVideoResolution(int width, int height) { }
    //--------------------------------------------------------------------------

    ///@}

    // IDisplayTime
    ///@{
    
    //==========================================================================
    /// @brief Totel time in ms
    ///
    /// @return time in ms
    ///
    virtual int GetTotalTime() { return 0; }
    //--------------------------------------------------------------------------
    
    //==========================================================================
    /// @brief Playing time in ms
    ///
    /// @return time in ms
    ///
    virtual int GetTime() { return 0; }
    //--------------------------------------------------------------------------

    ///@}
  
    
    // IPosTime
    ///@{

    //==========================================================================
    /// @brief Positions inputstream to playing time given in ms
    ///
    /// @param[in] ms time
    ///
    virtual bool PosTime(int ms) { return false; }
    //--------------------------------------------------------------------------

    ///@}
    
    // Seekable (mandatory)
    ///@{

    //==========================================================================
    /// @brief Check if the backend support pausing the currently playing stream
    ///
    /// This will enable/disable the pause button in Kodi based on the return
    /// value.
    /// 
    /// @return                         false if the InputStream addon/backend
    ///                                 does not support pausing, true if
    ///                                 possible
    ///
    virtual bool CanPauseStream() { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Check if the backend supports seeking for the currently playing
    /// stream
    ///
    /// This will enable/disable the rewind/forward buttons in Kodi based on the
    /// return value.
    ///
    /// @return                         false if the InputStream addon/backend
    ///                                 does not support seeking, true if
    ///                                 possible
    ///
    virtual bool CanSeekStream() { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Read from an open stream.
    ///
    /// @param pBuffer                  The buffer to store the data in.
    /// @param iBufferSize              The amount of bytes to read.
    /// @return                         The amount of bytes that were actually
    ///                                 read from the stream.
    ///
    virtual int ReadStream(uint8_t* pBuffer, unsigned int iBufferSize) { return -1; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Seek in a stream.
    ///
    /// The whence argument is optional and defaults to SEEK_SET (0)
    ///
    /// @param[in] position             the position that you want to seek to
    /// @param[in] whence               [optional] offset relative to
    ///                                 You can set the value of whence to one 
    ///                                 of three things:
    ///  |   Value  | int | Description                                        |
    ///  |:--------:|:---:|:---------------------------------------------------|
    ///  | SEEK_SET |  0  | position is relative to the beginning of the file. This is probably what you had in mind anyway, and is the most commonly used value for whence.
    ///  | SEEK_CUR |  1  | position is relative to the current file pointer position. So, in effect, you can say, "Move to my current position plus 30 bytes," or, "move to my current position minus 20 bytes."
    ///  | SEEK_END |  2  | position is relative to the end of the file. Just like SEEK_SET except from the other end of the file. Be sure to use negative values for offset if you want to back up from the end of the file, instead of going past the end into oblivion.
    /// @return                         Returns the resulting offset location as
    ///                                 measured in bytes from the beginning of
    ///                                 the file. On error, the value -1 is
    ///                                 returned.
    ///
    virtual int64_t SeekStream(int64_t position, int whence = SEEK_SET) { return -1; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief 
    ///
    /// @return The position in the stream that's currently being read.
    ///
    virtual int64_t PositionStream() { return -1; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief 
    ///
    /// @return The total length of the stream that's currently being read.
    ///
    virtual int64_t LengthStream() { return -1; }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Notify the InputStream addon that Kodi (un)paused the currently
    /// playing stream
    ///
    /// @param[in] time
    ///
    virtual void PauseStream(double time) { }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Check for real-time streaming
    ///
    /// @return true if current stream is real-time
    ///
    virtual bool IsRealTimeStream() { return true; }
    //--------------------------------------------------------------------------

    ///@}

    // Callbacks to Kodi and to this class
    ///@{

    //==========================================================================
    /// @brief Get library path name
    ///
    /// @return                         library path
    ///
    const std::string& LibFolder() { return m_libFolder; }
    //--------------------------------------------------------------------------
  
    //==========================================================================
    /// @brief Get profile path name
    ///
    /// @return                         profile path
    ///
    const std::string& ProfileFolder() { return m_profileFolder; }
    //--------------------------------------------------------------------------
    
    //==========================================================================
    /// @brief Allocate a demux packet. Free with FreeDemuxPacket
    ///
    /// @param iDataSize                The size of the data that will go into
    ///                                 the packet
    /// @return The allocated packet
    ///
    DemuxPacket* AllocateDemuxPacket(int iDataSize)
    {
      return m_instance->toKodi.AllocateDemuxPacket(m_instance->toKodi.kodiInstance, iDataSize);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Free a packet that was allocated with AllocateDemuxPacket
    ///
    /// @param pPacket                  The packet to free
    ///
    void FreeDemuxPacket(DemuxPacket* pPacket)
    {
      m_instance->toKodi.FreeDemuxPacket(m_instance->toKodi.kodiInstance, pPacket);
    }
    //--------------------------------------------------------------------------

    ///@}

  private:
    static inline void ADDON_GetCapabilities(void* addonInstance, INPUTSTREAM_CAPABILITIES* capabilities)
    {
      static_cast<CAddon*>(addonInstance)->GetCapabilities(*capabilities);
    }

    static inline bool ADDON_Open(void* addonInstance, INPUTSTREAM* props)
    {
      static_cast<CAddon*>(addonInstance)->m_strURL = props->m_strURL;
      static_cast<CAddon*>(addonInstance)->m_libFolder = props->m_libFolder;
      static_cast<CAddon*>(addonInstance)->m_profileFolder = props->m_profileFolder;
      for (unsigned int i = 0; i < props->m_nCountInfoValues; ++i)
        static_cast<CAddon*>(addonInstance)->m_ListItemProperties.insert(std::pair<std::string,std::string>(props->m_ListItemProperties[i].m_strKey, props->m_ListItemProperties[i].m_strValue));

      return static_cast<CAddon*>(addonInstance)->Open(static_cast<CAddon*>(addonInstance)->m_strURL,
                                                       static_cast<CAddon*>(addonInstance)->m_ListItemProperties);
    }

    static inline void ADDON_Close(void* addonInstance)
    {
      static_cast<CAddon*>(addonInstance)->Close();
    }
    
    static inline const char* ADDON_GetPathList(void* addonInstance)
    {
      static_cast<CAddon*>(addonInstance)->GetPathList(static_cast<CAddon*>(addonInstance)->m_pathList);
      return static_cast<CAddon*>(addonInstance)->m_pathList.c_str();
    }
    
    // IDemux
    static inline void ADDON_GetStreamIds(void* addonInstance, struct INPUTSTREAM_IDS* ids)
    {
      std::vector<int> internalIds;
      static_cast<CAddon*>(addonInstance)->GetStreamIds(internalIds);
  
      ids->m_streamCount = 0;
      for (unsigned int i = 0; i < internalIds.size() && i < INPUTSTREAM_IDS::MAX_STREAM_COUNT; ++i)
      {
        ids->m_streamIds[i] = internalIds[i];
        ++ids->m_streamCount;
      }
    }
    
    static inline void ADDON_GetStream(void* addonInstance, int streamId, INPUTSTREAM_INFO* info)
    {
      static_cast<CAddon*>(addonInstance)->GetStream(streamId, *info);
    }

    static inline void ADDON_EnableStream(void* addonInstance, int streamid, bool enable)
    {
      static_cast<CAddon*>(addonInstance)->EnableStream(streamid, enable);
    }
    
    static inline void ADDON_DemuxReset(void* addonInstance)
    {
      static_cast<CAddon*>(addonInstance)->DemuxReset();
    }

    static inline void ADDON_DemuxAbort(void* addonInstance)
    {
      static_cast<CAddon*>(addonInstance)->DemuxAbort();
    }

    static inline void ADDON_DemuxFlush(void* addonInstance)
    {
      static_cast<CAddon*>(addonInstance)->DemuxFlush();
    }

    static inline DemuxPacket* ADDON_DemuxRead(void* addonInstance)
    { 
      return static_cast<CAddon*>(addonInstance)->DemuxRead();
    }

    static inline bool ADDON_DemuxSeekTime(void* addonInstance, int time, bool backwards, double* startpts)
    {
      return static_cast<CAddon*>(addonInstance)->DemuxSeekTime(time, backwards, *startpts);
    }

    static inline void ADDON_DemuxSetSpeed(void* addonInstance, int speed)
    {
      static_cast<CAddon*>(addonInstance)->DemuxSetSpeed(speed);
    }

    static inline void ADDON_SetVideoResolution(void* addonInstance, int width, int height)
    {
      static_cast<CAddon*>(addonInstance)->SetVideoResolution(width, height);
    }

    // IDisplayTime
    static inline int ADDON_GetTotalTime(void* addonInstance)
    { 
      return static_cast<CAddon*>(addonInstance)->GetTotalTime();
    }

    static inline int ADDON_GetTime(void* addonInstance)
    { 
      return static_cast<CAddon*>(addonInstance)->GetTime();
    }

    // IPosTime
    static inline bool ADDON_PosTime(void* addonInstance, int ms)
    { 
      return static_cast<CAddon*>(addonInstance)->PosTime(ms);
    }

    // Seekable (mandatory)
    static inline bool ADDON_CanPauseStream(void* addonInstance)
    { 
      return static_cast<CAddon*>(addonInstance)->CanPauseStream();
    }

    static inline bool ADDON_CanSeekStream(void* addonInstance)
    { 
      return static_cast<CAddon*>(addonInstance)->CanSeekStream();
    }

    static inline int ADDON_ReadStream(void* addonInstance, uint8_t* pBuffer, unsigned int iBufferSize)
    { 
      return static_cast<CAddon*>(addonInstance)->ReadStream(pBuffer, iBufferSize);
    }

    static inline int64_t ADDON_SeekStream(void* addonInstance, int64_t position, int whence)
    { 
      return static_cast<CAddon*>(addonInstance)->SeekStream(position, whence);
    }

    static inline int64_t ADDON_PositionStream(void* addonInstance)
    { 
      return static_cast<CAddon*>(addonInstance)->PositionStream();
    }

    static inline int64_t ADDON_LengthStream(void* addonInstance)
    { 
      return static_cast<CAddon*>(addonInstance)->LengthStream();
    }

    static inline void ADDON_PauseStream(void* addonInstance, double time)
    { 
      static_cast<CAddon*>(addonInstance)->PauseStream(time);
    }

    static inline bool ADDON_IsRealTimeStream(void* addonInstance)
    { 
      return static_cast<CAddon*>(addonInstance)->IsRealTimeStream();
    }

    std::string m_strURL;
    std::map<std::string, std::string> m_ListItemProperties;
    std::string m_libFolder;
    std::string m_profileFolder;
    std::string m_pathList;

    sFuncTable_InputStream* m_instance;
  };

} /* namespace inputstream */
} /* namespace addon */
} /* namespace kodi */
} /* extern "C" */
#endif
