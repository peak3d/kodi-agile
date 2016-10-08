#pragma once
/*
 *      Copyright (C) 2013 Arne Morten Kvarving
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
#include "addons/kodi-addon-dev-kit/include/kodi/audiodecoder/AudioDecoder.h"
#include "cores/paplayer/ICodec.h"
#include "music/tags/ImusicInfoTagLoader.h"
#include "filesystem/MusicFileDirectory.h"

namespace MUSIC_INFO
{
  class CMusicInfoTag;
  class EmbeddedArt;
}

namespace ADDON
{

  class CAudioDecoder : public CAddonDll,
                        public ICodec,
                        public MUSIC_INFO::IMusicInfoTagLoader,
                        public XFILE::CMusicFileDirectory
  {
  public:
    explicit CAudioDecoder(std::string addonPath);
    virtual ~CAudioDecoder();

    // Child functions related to ICodec
    virtual bool Init(const CFileItem& file, unsigned int filecache) override;
    virtual int ReadPCM(BYTE* buffer, int size, int* actualsize) override;
    virtual bool Seek(int64_t time) override;
    virtual bool CanInit() override { return true; }

    // Child functions related to CMusicFileDirectory
    virtual int GetTrackCount(const std::string& strPath) override;

    // Child functions related to MUSIC_INFO::IMusicInfoTagLoader
    virtual bool Load(const std::string& strFileName,
              MUSIC_INFO::CMusicInfoTag& tag,
              MUSIC_INFO::EmbeddedArt *art = nullptr) override;

    const std::string& GetExtensions() const { return m_extension; }
    const std::string& GetMimetypes() const { return m_mimetype; }
    bool HasTags() const { return m_tags; }
    bool HasTracks() const { return m_tracks; }

  protected:
    std::string m_extension;
    std::string m_mimetype;
    bool m_tags;
    bool m_tracks;

  private:
    void DeInit();

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
    * After them becomes the audio decoder add-on Destroyed and complete disabled.
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

    sAddonInstance_AudioDecoder m_struct;
    void* m_addonInstance;
  };

} /* namespace ADDON */
