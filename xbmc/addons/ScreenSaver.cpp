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
#include "ScreenSaver.h"
#include "guilib/GraphicContext.h"
#include "interfaces/generic/ScriptInvocationManager.h"
#include "settings/Settings.h"
#include "utils/AlarmClock.h"
#include "windowing/WindowingFactory.h"

// What sound does a python screensaver make?
#define SCRIPT_ALARM "sssssscreensaver"

#define SCRIPT_TIMEOUT 15 // seconds

namespace ADDON
{

CScreenSaver::CScreenSaver(const char *addonID)
  : ADDON::CAddonDll<DllScreenSaver, ScreenSaver, SCR_PROPS>(AddonProps(addonID, ADDON_UNKNOWN)),
    m_addonInstance(nullptr)
{
  m_pInfo.name = nullptr;
  m_pInfo.presets = nullptr;
  m_pInfo.profile = nullptr;
}

bool CScreenSaver::IsInUse() const
{
  return CSettings::GetInstance().GetString(CSettings::SETTING_SCREENSAVER_MODE) == ID();
}

bool CScreenSaver::CreateScreenSaver()
{
  ADDON_STATUS status;

  if (CScriptInvocationManager::GetInstance().HasLanguageInvoker(LibPath()))
  {
    // Don't allow a previously-scheduled alarm to kill our new screensaver
    g_alarmClock.Stop(SCRIPT_ALARM, true);

    if (!CScriptInvocationManager::GetInstance().Stop(LibPath()))
      CScriptInvocationManager::GetInstance().ExecuteAsync(LibPath(), AddonPtr(new CScreenSaver(*this)));
    return true;
  }
 // pass it the screen width,height
 // and the name of the screensaver
  int iWidth = g_graphicsContext.GetWidth();
  int iHeight = g_graphicsContext.GetHeight();

#ifdef HAS_DX
  m_pInfo.device     = g_Windowing.Get3D11Context();
#else
  m_pInfo.device     = nullptr;
#endif
  m_pInfo.x          = 0;
  m_pInfo.y          = 0;
  m_pInfo.width      = iWidth;
  m_pInfo.height     = iHeight;
  m_pInfo.pixelRatio = g_graphicsContext.GetResInfo().fPixelRatio;
  m_pInfo.name       = strdup(Name().c_str());
  m_pInfo.presets    = strdup(CSpecialProtocol::TranslatePath(Path()).c_str());
  m_pInfo.profile    = strdup(CSpecialProtocol::TranslatePath(Profile()).c_str());

  status = CAddonDll<DllScreenSaver, ScreenSaver, SCR_PROPS>::Create();
  if (status != ADDON_STATUS_OK)
    return false;

  status = CAddonDll<DllScreenSaver, ScreenSaver, SCR_PROPS>::CreateInstance(ADDON_INSTANCE_SCREENSAVER, ID().c_str(), &m_pInfo, m_pStruct, this, &m_addonInstance);
  if (status != ADDON_STATUS_OK && status != ADDON_STATUS_NOT_IMPLEMENTED)
    return false;

  return true;
}

void CScreenSaver::Start()
{
  // notify screen saver that they should start
  if (Initialized()) m_pStruct->Start(m_addonInstance);
}

void CScreenSaver::Render()
{
  // ask screensaver to render itself
  if (Initialized()) m_pStruct->Render(m_addonInstance);
}

void CScreenSaver::GetInfo(SCR_INFO *info)
{
  // get info from screensaver
  if (Initialized()) m_pStruct->GetInfo(m_addonInstance, info);
}

void CScreenSaver::Destroy()
{
#ifdef HAS_PYTHON
  if (URIUtils::HasExtension(LibPath(), ".py"))
  {
    /* FIXME: This is a hack but a proper fix is non-trivial. Basically this code
     * makes sure the addon gets terminated after we've moved out of the screensaver window.
     * If we don't do this, we may simply lockup.
     */
    g_alarmClock.Start(SCRIPT_ALARM, SCRIPT_TIMEOUT, "StopScript(" + LibPath() + ")", true, false);
    return;
  }
#endif
  // Release what was allocated in method CScreenSaver::CreateScreenSaver.
  if (m_pInfo.name)
    free((void *) m_pInfo.name);
  m_pInfo.name = nullptr;
  if (m_pInfo.presets)
    free((void *) m_pInfo.presets);
  m_pInfo.presets = nullptr;
  if (m_pInfo.profile)
  free((void *) m_pInfo.profile);
  m_pInfo.profile = nullptr;

  CAddonDll<DllScreenSaver, ScreenSaver, SCR_PROPS>::DestroyInstance(ADDON_INSTANCE_SCREENSAVER, ID().c_str(), m_addonInstance);
  CAddonDll<DllScreenSaver, ScreenSaver, SCR_PROPS>::Destroy();
  m_addonInstance = nullptr;
}

} /*namespace ADDON*/

