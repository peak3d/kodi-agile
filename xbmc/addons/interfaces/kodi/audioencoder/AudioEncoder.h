#pragma once
/*
 *      Copyright (C) 2015-2016 Team Kodi
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
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "addons/AddonDll.h"
#include "addons/kodi-addon-dev-kit/include/kodi/audioencoder/AudioEncoder.h"
#include "cdrip/IEncoder.h"

namespace ADDON
{

  class CAudioEncoder : public CAddonDll, public IEncoder
  {
  public:
    static std::unique_ptr<CAudioEncoder> FromExtension(AddonProps, const cp_extension_t* ext);

    explicit CAudioEncoder(AddonProps props);
    CAudioEncoder(AddonProps props, std::string extension);
    virtual ~CAudioEncoder() {}

    // Child functions related to IEncoder
    virtual bool Init(sAddonToKodiFuncTable_AudioEncoder &callbacks) override;
    virtual int Encode(int nNumBytesRead, uint8_t* pbtStream) override;
    virtual bool Close() override;

    const std::string extension;

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

    sFuncTable_AudioEncoder m_struct;
    void* m_addonInstance;
  };

} /* namespace ADDON */
