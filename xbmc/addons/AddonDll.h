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

#include "Addon.h"
#include "DllAddon.h"
#include "AddonManager.h"
#include "addons/interfaces/AddonInterfaces.h"
#include "addons/kodi-addon-dev-kit/include/kodi/versions.h"
#include "utils/XMLUtils.h"

namespace ADDON
{
  class CAddonDll : public CAddon
  {
  public:
    CAddonDll(AddonProps props);

    //FIXME: does shallow pointer copy. no copy assignment op
    CAddonDll(const CAddonDll &rhs);
    virtual ~CAddonDll();
    virtual ADDON_STATUS GetStatus();

    // addon settings
    virtual void SaveSettings();
    virtual std::string GetSetting(const std::string& key);

    ADDON_STATUS Create();
    ADDON_STATUS Create(int type, void* funcTable, void* info);
    void Destroy();

    bool DllLoaded(void) const;

    ADDON_STATUS CreateInstance(int instanceType, const std::string& instanceID, KODI_HANDLE instance, KODI_HANDLE* addonInstance);
    void DestroyInstance(const std::string& instanceID);

  protected:
    bool Initialized() { return m_initialized; }
    virtual bool LoadSettings();
    static uint32_t GetChildCount() { static uint32_t childCounter = 0; return childCounter++; }
    CAddonInterfaces* m_pHelpers;
    bool m_bIsChild;
    std::string m_parentLib;

  private:
    DllAddon* m_pDll;
    bool m_initialized;
    bool LoadDll();
    bool m_needsavedsettings;
    std::map<std::string, std::pair<int, KODI_HANDLE>> m_usedInstances;

    virtual ADDON_STATUS TransferSettings();
    TiXmlElement MakeSetting(DllSetting& setting) const;
    bool CheckAPIVersion(int type);

    static void AddOnStatusCallback(void *userData, const ADDON_STATUS status, const char* msg);
    static bool AddOnGetSetting(void *userData, const char *settingName, void *settingValue);
    static void AddOnOpenSettings(const char *url, bool bReload);
    static void AddOnOpenOwnSettings(void *userData, bool bReload);
  };

}; /* namespace ADDON */

