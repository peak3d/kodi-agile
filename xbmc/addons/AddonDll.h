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
  class CAddonDll;
  typedef std::shared_ptr<CAddonDll> AddonDllPtr;

  class CAddonDll : public CAddon
  {
  public:
    CAddonDll(AddonInfoPtr addonInfo);

    //FIXME: does shallow pointer copy. no copy assignment op
    CAddonDll(const CAddonDll &rhs);
    virtual ~CAddonDll();
    virtual ADDON_STATUS GetStatus();
    virtual bool IsInUse() const;

    // addon settings
    virtual void SaveSettings();

    ADDON_STATUS Create(KODI_HANDLE firstKodiInstance);
    ADDON_STATUS Create(int type, void* funcTable, void* info);
    void Destroy();

    bool DllLoaded(void) const;

    ADDON_STATUS CreateInstance(int instanceType, const std::string& instanceID, KODI_HANDLE instance, KODI_HANDLE* addonInstance, ::kodi::addon::IAddonInstance* parentInstance = nullptr);
    void DestroyInstance(const std::string& instanceID);

    virtual void UpdateSettings(std::map<std::string, std::string>& settings);

  protected:
    bool Initialized() { return m_initialized; }
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
    bool CheckAPIVersion(int type);

    static void AddOnStatusCallback(void *userData, const ADDON_STATUS status, const char* msg);
    static bool AddOnGetSetting(void *userData, const char *settingName, void *settingValue);
    static void AddOnOpenSettings(const char *url, bool bReload);
    static void AddOnOpenOwnSettings(void *userData, bool bReload);

    /// addon to kodi basic callbacks below
    //@{
    AddonGlobalInterface m_interface;

    inline bool InitInterface(KODI_HANDLE firstKodiInstance);
    inline void DeInitInterface();

    static char* get_addon_path(void* kodiInstance);
    static char* get_base_user_path(void* kodiInstance);
    static void addon_log_msg(void* kodiInstance, const int addonLogLevel, const char* strMessage);
    static void free_string(void* kodiInstance, char* str);
    //@}
  };

  /*!
   * Class to make use of standard add-on functions more easy available on
   * Kodi's add-on instance classes.
   *
   * This functions in class are needed on several places, without them must be
   * on every instance class the same function added, with them can it be
   * prevent.
   */
  class CAddonInstanceInfo
  {
  public:
    CAddonInstanceInfo(AddonDllPtr addon) : m_addon(addon) { }

    std::string ID() const { return m_addon->ID(); }
    std::string Name() const { return m_addon->Name(); }
    std::string Path() const { return m_addon->Path(); }
    std::string Profile() const { return m_addon->Profile(); }

  protected:
    ADDON::AddonDllPtr m_addon;
  };

}; /* namespace ADDON */

