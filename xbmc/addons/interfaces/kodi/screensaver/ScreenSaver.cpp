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
#include "addons/interfaces/ExceptionHandling.h"
#include "addons/kodi-addon-dev-kit/include/kodi/General.h"
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

CScreenSaver::CScreenSaver(AddonProps props)
  : ADDON::CAddonDll(std::move(props)),
    m_addonInstance(nullptr)
{
  memset(&m_struct, 0, sizeof(m_struct));
}

CScreenSaver::CScreenSaver(const char *addonID)
  : ADDON::CAddonDll(AddonProps(addonID, ADDON_UNKNOWN)),
    m_addonInstance(nullptr)
{
  memset(&m_struct, 0, sizeof(m_struct));
}

bool CScreenSaver::IsInUse() const
{
  /* Check is active and selected in settings */
  return CSettings::GetInstance().GetString(CSettings::SETTING_SCREENSAVER_MODE) == ID() &&
         m_struct.toKodi.kodiInstance != nullptr;
}

bool CScreenSaver::CreateScreenSaver()
{
  if (CScriptInvocationManager::GetInstance().HasLanguageInvoker(LibPath()))
  {
    // Don't allow a previously-scheduled alarm to kill our new screensaver
    g_alarmClock.Stop(SCRIPT_ALARM, true);

    if (!CScriptInvocationManager::GetInstance().Stop(LibPath()))
      CScriptInvocationManager::GetInstance().ExecuteAsync(LibPath(), AddonPtr(new CScreenSaver(*this)));
    return true;
  }

  if (CAddonDll::Create() != ADDON_STATUS_OK)
    return false;

  m_struct.toKodi.kodiInstance = this;
  // pass it the screen width, height and the name of the screensaver
#ifdef HAS_DX
  m_struct.props.device = g_Windowing.Get3D11Context();
#else
  m_struct.props.device = nullptr;
#endif
  m_struct.props.x = 0;
  m_struct.props.y = 0;
  m_struct.props.width = g_graphicsContext.GetWidth();
  m_struct.props.height = g_graphicsContext.GetHeight();
  m_struct.props.pixelRatio = g_graphicsContext.GetResInfo().fPixelRatio;
  m_struct.props.name = strdup(Name().c_str());
  m_struct.props.presets = strdup(CSpecialProtocol::TranslatePath(Path()).c_str());
  m_struct.props.profile = strdup(CSpecialProtocol::TranslatePath(Profile()).c_str());

  return (CAddonDll::CreateInstance(ADDON_INSTANCE_SCREENSAVER, ID().c_str(), &m_struct, &m_addonInstance) == ADDON_STATUS_OK);
}

void CScreenSaver::Start()
{
  // notify screen saver that they should start
  try
  {
    if (m_struct.toAddon.Start)
      m_struct.toAddon.Start(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionStdHandle(ex, __FUNCTION__); }
  catch (int ex) { ExceptionErrHandle(ex, __FUNCTION__); }
}

void CScreenSaver::Render()
{
  // ask screensaver to render itself
  try
  {
    if (m_struct.toAddon.Render)
      m_struct.toAddon.Render(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionStdHandle(ex, __FUNCTION__); }
  catch (int ex) { ExceptionErrHandle(ex, __FUNCTION__); }
}

void CScreenSaver::Destroy()
{
  // Check first screensaver is based upon python, if yes are the other parts
  // not used and return in 'if'
  if (URIUtils::HasExtension(LibPath(), ".py"))
  {
    /* FIXME: This is a hack but a proper fix is non-trivial. Basically this code
     * makes sure the addon gets terminated after we've moved out of the screensaver window.
     * If we don't do this, we may simply lockup.
     */
    g_alarmClock.Start(SCRIPT_ALARM, SCRIPT_TIMEOUT, "StopScript(" + LibPath() + ")", true, false);
    return;
  }

  // Destroy the from binary add-on opened instance of screensaver.
  CAddonDll::DestroyInstance(ADDON_INSTANCE_SCREENSAVER, m_addonInstance);
  m_addonInstance = nullptr;

  // Release what was allocated in method CScreenSaver::CreateScreenSaver in
  // case of a binary add-on.
  if (m_struct.props.name)
    free((void *) m_struct.props.name);
  if (m_struct.props.presets)
    free((void *) m_struct.props.presets);
  if (m_struct.props.profile)
    free((void *) m_struct.props.profile);

  memset(&m_struct, 0, sizeof(m_struct));
}

void CScreenSaver::ExceptionStdHandle(std::exception& ex, const char* function)
{
  Exception::LogStdException(this, ex, function);
  Destroy();
  CAddonMgr::GetInstance().DisableAddon(ID());
  Exception::ShowExceptionErrorDialog(this);
}

void CScreenSaver::ExceptionErrHandle(int ex, const char* function)
{
  Exception::LogErrException(this, ex, function);
  Destroy();
  CAddonMgr::GetInstance().DisableAddon(ID());
  Exception::ShowExceptionErrorDialog(this);
}

} /* namespace ADDON */
