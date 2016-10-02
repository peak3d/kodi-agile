/*
 *      Copyright (C) 2015-2016 Team KODI
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
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "General.h"
#include "ControlButton.h"
#include "ControlEdit.h"
#include "ControlFadeLabel.h"
#include "ControlImage.h"
#include "ControlLabel.h"
#include "ControlProgress.h"
#include "ControlRadioButton.h"
#include "ControlRendering.h"
#include "ControlSettingsSlider.h"
#include "ControlSlider.h"
#include "ControlSpin.h"
#include "ControlTextBox.h"
#include "DialogContextMenu.h"
#include "DialogExtendedProgressBar.h"
#include "DialogFileBrowser.h"
#include "DialogKeyboard.h"
#include "DialogNumeric.h"
#include "DialogOK.h"
#include "DialogProgress.h"
#include "DialogSelect.h"
#include "DialogTextViewer.h"
#include "DialogYesNo.h"
#include "ListItem.h"
#include "Window.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/General.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUIWindowManager.h"

using namespace kodi; // addon-dev-kit namespace
using namespace kodi::gui; // addon-dev-kit namespace

namespace ADDON
{

int Interface_GUIGeneral::m_iAddonGUILockRef = 0;

extern "C"
{

void Interface_GUIGeneral::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui = (sAddonToKodiFuncTable_kodi_gui*)malloc(sizeof(sAddonToKodiFuncTable_kodi_gui));
  funcTable->toKodi.kodi_gui->general.Lock = Lock;
  funcTable->toKodi.kodi_gui->general.Unlock = Unlock;
  funcTable->toKodi.kodi_gui->general.GetScreenHeight = GetScreenHeight;
  funcTable->toKodi.kodi_gui->general.GetScreenWidth = GetScreenWidth;
  funcTable->toKodi.kodi_gui->general.GetVideoResolution = GetVideoResolution;
  funcTable->toKodi.kodi_gui->general.GetCurrentWindowDialogId = GetCurrentWindowDialogId;
  funcTable->toKodi.kodi_gui->general.GetCurrentWindowId = GetCurrentWindowId;

  Interface_GUIControlButton::Init(funcTable);
  Interface_GUIControlEdit::Init(funcTable);
  Interface_GUIControlFadeLabel::Init(funcTable);
  Interface_GUIControlImage::Init(funcTable);
  Interface_GUIControlLabel::Init(funcTable);
  Interface_GUIControlProgress::Init(funcTable);
  Interface_GUIControlRadioButton::Init(funcTable);
  Interface_GUIControlAddonRendering::Init(funcTable);
  Interface_GUIControlSettingsSlider::Init(funcTable);
  Interface_GUIControlSlider::Init(funcTable);
  Interface_GUIControlSpin::Init(funcTable);
  Interface_GUIControlTextBox::Init(funcTable);
  Interface_GUIDialogContextMenu::Init(funcTable);
  Interface_GUIDialogExtendedProgress::Init(funcTable);
  Interface_GUIDialogFileBrowser::Init(funcTable);
  Interface_GUIDialogKeyboard::Init(funcTable);
  Interface_GUIDialogNumeric::Init(funcTable);
  Interface_GUIDialogOK::Init(funcTable);
  Interface_GUIDialogProgress::Init(funcTable);
  Interface_GUIDialogSelect::Init(funcTable);
  Interface_GUIDialogTextViewer::Init(funcTable);
  Interface_GUIDialogYesNo::Init(funcTable);
  Interface_GUIWindow::Init(funcTable);
  Interface_GUIListItem::Init(funcTable);
}

void Interface_GUIGeneral::DeInit(sFuncTable_Addon* funcTable)
{
  if (funcTable->toKodi.kodi_gui)
  {
    free(funcTable->toKodi.kodi_gui);
    funcTable->toKodi.kodi_gui = nullptr;
  }
}

//@{
void Interface_GUIGeneral::Lock()
{
  if (m_iAddonGUILockRef == 0)
    g_graphicsContext.Lock();
  ++m_iAddonGUILockRef;
}

void Interface_GUIGeneral::Unlock()
{
  if (m_iAddonGUILockRef > 0)
  {
    --m_iAddonGUILockRef;
    if (m_iAddonGUILockRef == 0)
      g_graphicsContext.Unlock();
  }
}
//@}

//@{
int Interface_GUIGeneral::GetScreenHeight(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return -1;
  }

  try
  {
    return g_graphicsContext.GetHeight();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

int Interface_GUIGeneral::GetScreenWidth(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return -1;
  }

  try
  {
    return g_graphicsContext.GetWidth();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

int Interface_GUIGeneral::GetVideoResolution(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return -1;
  }

  try
  {
    return (int)g_graphicsContext.GetVideoResolution();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}
//@}

//@{
int Interface_GUIGeneral::GetCurrentWindowDialogId(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return -1;
  }

  try
  {
    CSingleLock gl(g_graphicsContext);
    return g_windowManager.GetTopMostModalDialogID();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

int Interface_GUIGeneral::GetCurrentWindowId(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return -1;
  }

  try
  {
    CSingleLock gl(g_graphicsContext);
    return g_windowManager.GetActiveWindow();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

//@}

} /* extern "C" */
} /* namespace ADDON */
