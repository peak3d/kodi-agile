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

CScreenSaver::CScreenSaver(ADDON::AddonDllPtr addon)
  : m_addon(addon),
    m_addonInstance(nullptr)
{
  memset(&m_struct, 0, sizeof(m_struct));
}

bool CScreenSaver::CreateScreenSaver()
{
  if (!m_addon)
    return false;

  if (CScriptInvocationManager::GetInstance().HasLanguageInvoker(m_addon->LibPath()))
  {
    // Don't allow a previously-scheduled alarm to kill our new screensaver
    g_alarmClock.Stop(SCRIPT_ALARM, true);

    if (!CScriptInvocationManager::GetInstance().Stop(m_addon->LibPath()))
      CScriptInvocationManager::GetInstance().ExecuteAsync(m_addon->LibPath(), AddonPtr(new CAddonDll(*m_addon)));
    return true;
  }

  if (m_addon->Create() != ADDON_STATUS_OK)
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
  m_struct.props.name = strdup(m_addon->Name().c_str());
  m_struct.props.presets = strdup(CSpecialProtocol::TranslatePath(m_addon->Path()).c_str());
  m_struct.props.profile = strdup(CSpecialProtocol::TranslatePath(m_addon->Profile()).c_str());

  return (m_addon->CreateInstance(ADDON_INSTANCE_SCREENSAVER, m_addon->ID().c_str(), &m_struct, &m_addonInstance) == ADDON_STATUS_OK);
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
  catch (int ex)             { ExceptionErrHandle(ex, __FUNCTION__); }
  catch (...)                { ExceptionUnkHandle(__FUNCTION__); }
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
  catch (int ex)             { ExceptionErrHandle(ex, __FUNCTION__); }
  catch (...)                { ExceptionUnkHandle(__FUNCTION__); }
}

void CScreenSaver::Destroy()
{
  if (!m_addon)
    return;

  // Check first screensaver is based upon python, if yes are the other parts
  // not used and return in 'if'
  if (URIUtils::HasExtension(m_addon->LibPath(), ".py"))
  {
    /* FIXME: This is a hack but a proper fix is non-trivial. Basically this code
     * makes sure the addon gets terminated after we've moved out of the screensaver window.
     * If we don't do this, we may simply lockup.
     */
    g_alarmClock.Start(SCRIPT_ALARM, SCRIPT_TIMEOUT, "StopScript(" + m_addon->LibPath() + ")", true, false);
    return;
  }

  // Destroy the from binary add-on opened instance of screensaver.
  m_addon->DestroyInstance(ADDON_INSTANCE_SCREENSAVER, m_addonInstance);
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
  m_addon->Destroy();
}

void CScreenSaver::ExceptionStdHandle(std::exception& ex, const char* function)
{
  Exception::LogStdException(m_addon, ex, function);
  Destroy();
  CAddonMgr::GetInstance().DisableAddon(m_addon->ID());
  Exception::ShowExceptionErrorDialog(m_addon);
}

void CScreenSaver::ExceptionErrHandle(int ex, const char* function)
{
  Exception::LogErrException(m_addon, ex, function);
  Destroy();
  CAddonMgr::GetInstance().DisableAddon(m_addon->ID());
  Exception::ShowExceptionErrorDialog(m_addon);
}

void CScreenSaver::ExceptionUnkHandle(const char* function)
{
  Exception::LogUnkException(m_addon, function);
  Destroy();
  CAddonMgr::GetInstance().DisableAddon(m_addon->ID());
  Exception::ShowExceptionErrorDialog(m_addon);
}

} /* namespace ADDON */
