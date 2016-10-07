#pragma once
/*
 *      Copyright (C) 2016 Team Kodi
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

#include "addons/AddonDll.h"
#include "FileItem.h"
#include "addons/kodi-addon-dev-kit/include/kodi/inputstream/InputStream.h"
#include "threads/CriticalSection.h"

#include <vector>
#include <map>

class CDemuxStream;

namespace ADDON
{

  class CInputStream : public CAddonDll
  {
  public:

    static std::unique_ptr<CInputStream> FromExtension(AddonProps props, const cp_extension_t* ext);

    explicit CInputStream(AddonProps props);
    CInputStream(const AddonProps& props,
                 const std::string& name,
                 const std::string& listitemprops,
                 const std::string& extensions,
                 const std::string& protocols);
    virtual ~CInputStream();

    ADDON_STATUS Create();
    void Destroy(void);

    virtual void SaveSettings() override;

    bool UseParent();
    bool Supports(const CFileItem &fileitem);
    bool Open(CFileItem &fileitem);
    void Close();

    bool HasDemux() { return m_caps.m_supportsIDemux; };
    bool HasPosTime() { return m_caps.m_supportsIPosTime; };
    bool HasDisplayTime() { return m_caps.m_supportsIDisplayTime; };
    bool CanPause() { return m_caps.m_supportsPause; };
    bool CanSeek() { return m_caps.m_supportsSeek; };

    // IDisplayTime
    int GetTotalTime();
    int GetTime();

    // IPosTime
    bool PosTime(int ms);

    // demux
    int GetNrOfStreams() const;
    CDemuxStream* GetStream(int iStreamId);
    std::vector<CDemuxStream*> GetStreams() const;
    DemuxPacket* ReadDemux();
    bool SeekTime(int time, bool backward, double* startpts);
    void AbortDemux();
    void FlushDemux();
    void SetSpeed(int iSpeed);
    void EnableStream(int iStreamId, bool enable);
    void SetVideoResolution(int width, int height);

    // stream
    int ReadStream(uint8_t* buf, unsigned int size);
    int64_t SeekStream(int64_t offset, int whence);
    int64_t PositionStream();
    int64_t LengthStream();
    void PauseStream(double time);
    bool IsRealTimeStream();

    // Static callback functions for add-on to Kodi below
    static DemuxPacket* InputStreamAllocateDemuxPacket(void* addonData, int iDataSize = 0); /*!< @ref kodi_addon_inputstream_AllocateDemuxPacket */
    static void InputStreamFreeDemuxPacket(void* addonData, DemuxPacket* pPacket); /*!< @ref kodi_addon_inputstream_FreeDemuxPacket */

  protected:
    void UpdateStreams();
    void DisposeStreams();
    void UpdateConfig();
    void CheckConfig();

    std::vector<std::string> m_fileItemProps;
    std::vector<std::string> m_extensionsList;
    std::vector<std::string> m_protocolsList;
    INPUTSTREAM_CAPABILITIES m_caps;
    std::map<int, CDemuxStream*> m_streams;

    static CCriticalSection m_parentSection;

    struct Config
    {
      std::vector<std::string> m_pathList;
      bool m_parentBusy;
      bool m_ready;
    };
    static std::map<std::string, Config> m_configMap;

  private:
    /*!
    * @{
    * @brief Functions below are used to handle exception between Kodi and his
    * add-on.
    *
    * ExceptionStdHandle(...):
    *   Used for to handle standard c++ exception partly generated from dev kits
    *   headers.
    *
    * ExceptionErrHandle(...):
    *   This is a special type basically used also in dev kit headers to give a
    *   exception with his ADDON_STATUS as integer value.
    *   Can be generated from headers by a massive fault detected on call of
    *   them.
    *
    * ExceptionUnkHandle(...);
    *   Used for everything else.
    *
    * With a call of them becomes the standardized function
    * Exception::LogUnkException(...) from ExceptionHandling.h used to write
    * log entry. This is always the first call, to see if it still crashes the
    * source of them.
    *
    * After them becomes the audio encoder add-on Destroyed and complete disabled.
    *
    * As last step comes a dialog to inform the used about the Exception.
    *
    * How it is set on add-on calls
    * ~~~~~~~~~~~~~{.cpp}
    * try
    * {
    *   ...
    * }
    * catch (std::exception& ex) { ExceptionStdHandle(ex, __FUNCTION__); }
    * catch (int ex)             { ExceptionErrHandle(ex, __FUNCTION__); }
    * catch (...)                { ExceptionUnkHandle(__FUNCTION__); }
    * ~~~~~~~~~~~~~
    *
    * @note this way becomes also used on add-on typed e.g. pvr ...
    */
    void ExceptionStdHandle(std::exception& ex, const char* function);
    void ExceptionErrHandle(int ex, const char* function);
    void ExceptionUnkHandle(const char* function);
    /* @}*/

    sFuncTable_InputStream m_struct;
    void* m_addonInstance;
  };

} /* namespace ADDON */
