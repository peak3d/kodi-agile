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

#include "AddonDll.h"
#include "addons/kodi-addon-dev-kit/include/kodi/xbmc_audioenc_types.h"
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
    void ExceptionHandle(std::exception& ex, const char* function);

    sFuncTable_AudioEncoder m_struct;
    void* m_addonInstance;
  };

} /* namespace ADDON */
