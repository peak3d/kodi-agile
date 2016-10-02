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

#include "Window.h"
#include "General.h"
#include "ControlRendering.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/Window.h"

#include "Application.h"
#include "FileItem.h"
#include "addons/AddonDll.h"
#include "addons/Skin.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "addons/interfaces/Translators.h"
#include "dialogs/GUIDialogProgress.h"
#include "filesystem/File.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/TextureManager.h"
#include "input/Key.h"
#include "messaging/ApplicationMessenger.h"
#include "messaging/helpers/DialogHelper.h"
#include "utils/log.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/Variant.h"

#define CONTROL_BTNVIEWASICONS  2
#define CONTROL_BTNSORTBY       3
#define CONTROL_BTNSORTASC      4
#define CONTROL_LABELFILES      12

using namespace ADDON;
using namespace KODI::MESSAGING;
using KODI::MESSAGING::HELPERS::DialogResponse;

using namespace kodi; // addon-dev-kit namespace
using namespace kodi::gui; // addon-dev-kit namespace

namespace ADDON
{
extern "C"
{

void Interface_GUIWindow::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->window.New = New;
  funcTable->toKodi.kodi_gui->window.Delete = Delete;
  funcTable->toKodi.kodi_gui->window.SetCallbacks = SetCallbacks;
  funcTable->toKodi.kodi_gui->window.Show = Show;
  funcTable->toKodi.kodi_gui->window.Close = Close;
  funcTable->toKodi.kodi_gui->window.DoModal = DoModal;
  funcTable->toKodi.kodi_gui->window.SetFocusId = SetFocusId;
  funcTable->toKodi.kodi_gui->window.GetFocusId = GetFocusId;
  funcTable->toKodi.kodi_gui->window.SetProperty = SetProperty;
  funcTable->toKodi.kodi_gui->window.SetPropertyInt = SetPropertyInt;
  funcTable->toKodi.kodi_gui->window.SetPropertyBool = SetPropertyBool;
  funcTable->toKodi.kodi_gui->window.SetPropertyDouble = SetPropertyDouble;
  funcTable->toKodi.kodi_gui->window.GetProperty = GetProperty;
  funcTable->toKodi.kodi_gui->window.GetPropertyInt = GetPropertyInt;
  funcTable->toKodi.kodi_gui->window.GetPropertyBool = GetPropertyBool;
  funcTable->toKodi.kodi_gui->window.GetPropertyDouble = GetPropertyDouble;
  funcTable->toKodi.kodi_gui->window.ClearProperties = ClearProperties;
  funcTable->toKodi.kodi_gui->window.ClearProperty = ClearProperty;
  funcTable->toKodi.kodi_gui->window.GetListSize = GetListSize;
  funcTable->toKodi.kodi_gui->window.ClearList = ClearList;
  funcTable->toKodi.kodi_gui->window.AddItem = AddItem;
  funcTable->toKodi.kodi_gui->window.AddStringItem = AddStringItem;
  funcTable->toKodi.kodi_gui->window.RemoveItem = RemoveItem;
  funcTable->toKodi.kodi_gui->window.RemoveItemFile = RemoveItemFile;
  funcTable->toKodi.kodi_gui->window.GetListItem = GetListItem;
  funcTable->toKodi.kodi_gui->window.SetCurrentListPosition = SetCurrentListPosition;
  funcTable->toKodi.kodi_gui->window.GetCurrentListPosition = GetCurrentListPosition;
  funcTable->toKodi.kodi_gui->window.SetControlLabel = SetControlLabel;
  funcTable->toKodi.kodi_gui->window.MarkDirtyRegion = MarkDirtyRegion;

  funcTable->toKodi.kodi_gui->window.GetControl_Button = GetControl_Button;
  funcTable->toKodi.kodi_gui->window.GetControl_Edit = GetControl_Edit;
  funcTable->toKodi.kodi_gui->window.GetControl_FadeLabel = GetControl_FadeLabel;
  funcTable->toKodi.kodi_gui->window.GetControl_Image = GetControl_Image;
  funcTable->toKodi.kodi_gui->window.GetControl_Label = GetControl_Label;
  funcTable->toKodi.kodi_gui->window.GetControl_Spin = GetControl_Spin;
  funcTable->toKodi.kodi_gui->window.GetControl_RadioButton = GetControl_RadioButton;
  funcTable->toKodi.kodi_gui->window.GetControl_Progress = GetControl_Progress;
  funcTable->toKodi.kodi_gui->window.GetControl_RenderAddon = GetControl_RenderAddon;
  funcTable->toKodi.kodi_gui->window.GetControl_Slider = GetControl_Slider;
  funcTable->toKodi.kodi_gui->window.GetControl_SettingsSlider= GetControl_SettingsSlider;
  funcTable->toKodi.kodi_gui->window.GetControl_TextBox = GetControl_TextBox;
}


void* Interface_GUIWindow::New(void* kodiInstance, const char *xmlFilename,
                               const char *defaultSkin, bool forceFallback,
                               bool asDialog)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    RESOLUTION_INFO res;
    std::string strSkinPath;
    if (!forceFallback)
    {
      /* Check to see if the XML file exists in current skin. If not use
         fallback path to find a skin for the addon */
      strSkinPath = g_SkinInfo->GetSkinPath(xmlFilename, &res);

      if (!XFILE::CFile::Exists(strSkinPath))
      {
        /* Check for the matching folder for the skin in the fallback skins folder */
        std::string basePath = URIUtils::AddFileToFolder(addon->Path(), "resources");
        basePath = URIUtils::AddFileToFolder(basePath, "skins");
        basePath = URIUtils::AddFileToFolder(basePath, URIUtils::GetFileName(g_SkinInfo->Path()));
        strSkinPath = g_SkinInfo->GetSkinPath(xmlFilename, &res, basePath);
        if (!XFILE::CFile::Exists(strSkinPath))
        {
          /* Finally fallback to the DefaultSkin as it didn't exist in either the
             Kodi Skin folder or the fallback skin folder */
          forceFallback = true;
        }
      }
    }

    if (forceFallback)
    {
      //FIXME make this static method of current skin?
      std::string str("none");
      AddonProps props(str, ADDON_SKIN);
      std::string basePath = URIUtils::AddFileToFolder(addon->Path(), "resources");
      basePath = URIUtils::AddFileToFolder(basePath, "skins");
      basePath = URIUtils::AddFileToFolder(basePath, defaultSkin);
      props.path = basePath;

      CSkinInfo skinInfo(props);
      skinInfo.Start();
      strSkinPath = skinInfo.GetSkinPath(xmlFilename, &res, basePath);

      if (!XFILE::CFile::Exists(strSkinPath))
      {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - New: %s/%s - XML File '%s' for Window is missing, contact Developer '%s' of this AddOn",
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str(), strSkinPath.c_str(), addon->Author().c_str()).c_str());
      }
    }
    // window id's 14000 - 14100 are reserved for addons
    // get first window id that is not in use
    int id = WINDOW_ADDON_START;
    // if window 14099 is in use it means addon can't create more windows
    Interface_GUIGeneral::Lock();
    if (g_windowManager.GetWindow(WINDOW_ADDON_END))
    {
      Interface_GUIGeneral::Unlock();
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - maximum number of windows reached",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }
    while(id < WINDOW_ADDON_END && g_windowManager.GetWindow(id) != nullptr) id++;
    Interface_GUIGeneral::Unlock();

    CGUIWindow *window;
    if (!asDialog)
      window = new CGUIAddonWindow(id, strSkinPath, addon);
    else
      window = new CGUIAddonWindowDialog(id, strSkinPath, addon);

    Interface_GUIGeneral::Lock();
    g_windowManager.Add(window);
    Interface_GUIGeneral::Unlock();

    window->SetCoordsRes(res);
    return window;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void Interface_GUIWindow::Delete(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return;

    Interface_GUIGeneral::Lock();
    // first change to an existing window
    if (g_windowManager.GetActiveWindow() == pAddonWindow->m_iWindowId && !g_application.m_bStop)
    {
      if(g_windowManager.GetWindow(pAddonWindow->m_iOldWindowId))
        g_windowManager.ActivateWindow(pAddonWindow->m_iOldWindowId);
      else // old window does not exist anymore, switch to home
        g_windowManager.ActivateWindow(WINDOW_HOME);
    }
    // Free any window properties
    pAddonWindow->ClearProperties();
    // free the window's resources and unload it (free all guicontrols)
    pAddonWindow->FreeResources(true);

    g_windowManager.Remove(pAddonWindow->GetID());
    delete pAddonWindow;
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIWindow::SetCallbacks(void* kodiInstance, void* handle, void* clienthandle,
                                bool (*initCB)(void*), bool (*clickCB)(void*, int),
                                bool (*focusCB)(void*, int), bool (*onActionCB)(void* handle, int))
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;

    Interface_GUIGeneral::Lock();
    pAddonWindow->m_clientHandle = clienthandle;
    pAddonWindow->CBOnInit = initCB;
    pAddonWindow->CBOnClick = clickCB;
    pAddonWindow->CBOnFocus = focusCB;
    pAddonWindow->CBOnAction = onActionCB;
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

bool Interface_GUIWindow::Show(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow::%s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return false;

    if (pAddonWindow->m_iOldWindowId != pAddonWindow->m_iWindowId && pAddonWindow->m_iWindowId != g_windowManager.GetActiveWindow())
      pAddonWindow->m_iOldWindowId = g_windowManager.GetActiveWindow();

    Interface_GUIGeneral::Lock();
    if (pAddonWindow->IsDialog())
      ((CGUIAddonWindowDialog*)pAddonWindow)->Show();
    else
      g_windowManager.ActivateWindow(pAddonWindow->m_iWindowId);
    Interface_GUIGeneral::Unlock();

    return true;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIWindow::Close(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return false;

    pAddonWindow->m_bModal = false;
    if (pAddonWindow->IsDialog())
      ((CGUIAddonWindowDialog*)pAddonWindow)->PulseActionEvent();
    else
      ((CGUIAddonWindow*)pAddonWindow)->PulseActionEvent();

    Interface_GUIGeneral::Lock();
    // if it's a dialog, we have to close it a bit different
    if (pAddonWindow->IsDialog())
      ((CGUIAddonWindowDialog*)pAddonWindow)->Show(false);
    else
      g_windowManager.ActivateWindow(pAddonWindow->m_iOldWindowId);
    pAddonWindow->m_iOldWindowId = 0;

    Interface_GUIGeneral::Unlock();

    return true;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIWindow::DoModal(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return false;

    pAddonWindow->m_bModal = true;

    if (pAddonWindow->m_iWindowId != g_windowManager.GetActiveWindow())
      Show(kodiInstance, handle);

    return true;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIWindow::SetFocusId(void* kodiInstance, void* handle, int iControlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return false;

    if (!pWindow->GetControl(iControlId))
    {
      CLog::Log(LOGERROR, "Interface_GUIWindow - %s: %s/%s - Control does not exist in window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str());
      return false;
    }

    Interface_GUIGeneral::Lock();
    CGUIMessage msg = CGUIMessage(GUI_MSG_SETFOCUS, pAddonWindow->m_iWindowId, iControlId);
    pWindow->OnMessage(msg);
    Interface_GUIGeneral::Unlock();

    return true;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

int Interface_GUIWindow::GetFocusId(void* kodiInstance, void* handle)
{
  int iControlId = -1;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return iControlId;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return iControlId;

    Interface_GUIGeneral::Lock();
    iControlId = pWindow->GetFocusedControlID();
    Interface_GUIGeneral::Unlock();

    if (iControlId == -1)
    {
      CLog::Log(LOGERROR, "Interface_GUIWindow - %s: %s/%s - No control in this window has focus",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str());
      return iControlId;
    }

    return iControlId;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return iControlId;
}

void Interface_GUIWindow::SetProperty(void* kodiInstance, void* handle, const char *key, const char *value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !key || !value)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window or nullptr key or value",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return;

    std::string lowerKey = key;
    StringUtils::ToLower(lowerKey);

    Interface_GUIGeneral::Lock();
    pWindow->SetProperty(lowerKey, value);
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIWindow::SetPropertyInt(void* kodiInstance, void* handle, const char *key, int value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !key)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return;

    std::string lowerKey = key;
    StringUtils::ToLower(lowerKey);

    Interface_GUIGeneral::Lock();
    pWindow->SetProperty(lowerKey, value);
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIWindow::SetPropertyBool(void* kodiInstance, void* handle, const char *key, bool value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !key)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return;

    std::string lowerKey = key;
    StringUtils::ToLower(lowerKey);

    Interface_GUIGeneral::Lock();
    pWindow->SetProperty(lowerKey, value);
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIWindow::SetPropertyDouble(void* kodiInstance, void* handle, const char *key, double value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !key)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window or nullptr key",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return;

    std::string lowerKey = key;
    StringUtils::ToLower(lowerKey);

    Interface_GUIGeneral::Lock();
    pWindow->SetProperty(lowerKey, value);
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIWindow::GetProperty(void* kodiInstance, void* handle, const char *key, char &property, unsigned int &iMaxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !key)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window or nullptr key",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return;

    std::string lowerKey = key;
    StringUtils::ToLower(lowerKey);

    Interface_GUIGeneral::Lock();
    std::string value = pWindow->GetProperty(lowerKey).asString();
    Interface_GUIGeneral::Unlock();

    strncpy(&property, value.c_str(), iMaxStringSize);
    iMaxStringSize = value.length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

int Interface_GUIWindow::GetPropertyInt(void* kodiInstance, void* handle, const char *key)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return -1;
  }

  try
  {
    if (!handle || !key)
    {

      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window or nullptr key",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return -1;

    std::string lowerKey = key;
    StringUtils::ToLower(lowerKey);

    Interface_GUIGeneral::Lock();
    int value = (int)pWindow->GetProperty(lowerKey).asInteger();
    Interface_GUIGeneral::Unlock();

    return value;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

bool Interface_GUIWindow::GetPropertyBool(void* kodiInstance, void* handle, const char *key)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!handle || !key)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window or nullptr key",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return false;

    std::string lowerKey = key;
    StringUtils::ToLower(lowerKey);

    Interface_GUIGeneral::Lock();
    bool value = pWindow->GetProperty(lowerKey).asBoolean();
    Interface_GUIGeneral::Unlock();

    return value;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

double Interface_GUIWindow::GetPropertyDouble(void* kodiInstance, void* handle, const char *key)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return 0.0;
  }

  try
  {
    if (!handle || !key)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window or nullptr key",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return 0.0;

    std::string lowerKey = key;
    StringUtils::ToLower(lowerKey);

    Interface_GUIGeneral::Lock();
    double value = pWindow->GetProperty(lowerKey).asDouble();
    Interface_GUIGeneral::Unlock();

    return value;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0.0;
}

void Interface_GUIWindow::ClearProperties(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return;

    Interface_GUIGeneral::Lock();
    pWindow->ClearProperties();
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIWindow::ClearProperty(void* kodiInstance, void* handle, const char *key)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;
    CGUIWindow      *pWindow = (CGUIWindow*)g_windowManager.GetWindow(pAddonWindow->m_iWindowId);
    if (!pWindow)
      return;

    std::string lowerKey = key;
    StringUtils::ToLower(lowerKey);
    Interface_GUIGeneral::Lock();
    pWindow->SetProperty(lowerKey, "");
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

int Interface_GUIWindow::GetListSize(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return -1;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    Interface_GUIGeneral::Lock();
    int listSize = static_cast<CGUIAddonWindow*>(handle)->GetListSize();
    Interface_GUIGeneral::Unlock();

    return listSize;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

void Interface_GUIWindow::ClearList(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    Interface_GUIGeneral::Lock();
    static_cast<CGUIAddonWindow*>(handle)->ClearList();
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void* Interface_GUIWindow::AddItem(void* kodiInstance, void* handle, void* item, int itemPosition)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle || !item)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CFileItemPtr pItem((CFileItem*)item);
    Interface_GUIGeneral::Lock();
    static_cast<CGUIAddonWindow*>(handle)->AddItem(pItem, itemPosition);
    Interface_GUIGeneral::Unlock();

    return item;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::AddStringItem(void* kodiInstance, void* handle, const char *itemName, int itemPosition)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle || !itemName)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CFileItemPtr item(new CFileItem(itemName));
    Interface_GUIGeneral::Lock();
    static_cast<CGUIAddonWindow*>(handle)->AddItem(item, itemPosition);
    Interface_GUIGeneral::Unlock();

    return item.get();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void Interface_GUIWindow::RemoveItem(void* kodiInstance, void* handle, int itemPosition)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    Interface_GUIGeneral::Lock();
    static_cast<CGUIAddonWindow*>(handle)->RemoveItem(itemPosition);
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIWindow::RemoveItemFile(void* kodiInstance, void* handle, void* fileItem)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    Interface_GUIGeneral::Lock();
    static_cast<CGUIAddonWindow*>(handle)->RemoveItem((CFileItem*)fileItem);
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void* Interface_GUIWindow::GetListItem(void* kodiInstance, void* handle, int listPos)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = (CGUIAddonWindow*)handle;

    Interface_GUIGeneral::Lock();
    CFileItemPtr fi = pAddonWindow->GetListItem(listPos);
    if (fi == nullptr)
    {
      Interface_GUIGeneral::Unlock();
      CLog::Log(LOGERROR, "Interface_GUIWindow - %s: %s/%s - Index out of range",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str());
      return nullptr;
    }
    Interface_GUIGeneral::Unlock();

    return fi.get();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void Interface_GUIWindow::SetCurrentListPosition(void* kodiInstance, void* handle, int listPos)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = static_cast<CGUIAddonWindow*>(handle);

    Interface_GUIGeneral::Lock();
    pAddonWindow->SetCurrentListPosition(listPos);
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

int Interface_GUIWindow::GetCurrentListPosition(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return -1;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = static_cast<CGUIAddonWindow*>(handle);

    Interface_GUIGeneral::Lock();
    int listPos = pAddonWindow->GetCurrentListPosition();
    Interface_GUIGeneral::Unlock();

    return listPos;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

void Interface_GUIWindow::SetControlLabel(void* kodiInstance, void* handle, int controlId, const char *label)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIAddonWindow *pAddonWindow = static_cast<CGUIAddonWindow*>(handle);

    CGUIMessage msg(GUI_MSG_LABEL_SET, pAddonWindow->m_iWindowId, controlId);
    msg.SetLabel(label);
    pAddonWindow->OnMessage(msg);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIWindow::MarkDirtyRegion(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    static_cast<CGUIAddonWindow*>(handle)->MarkDirtyRegion();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void* Interface_GUIWindow::GetControl_Button(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    return static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_BUTTON, "button");
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::GetControl_FadeLabel(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    return static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_FADELABEL, "fade label");
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::GetControl_Label(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    return static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_LABEL, "label");
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::GetControl_Image(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    return static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_IMAGE, "image");
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::GetControl_RadioButton(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    return static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_RADIO, "radio button");
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::GetControl_Edit(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    return static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_EDIT, "edit");
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::GetControl_Progress(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    return static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_PROGRESS, "progress");
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::GetControl_Spin(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    return static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_SPINEX, "spin");
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::GetControl_RenderAddon(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    CGUIControl* pGUIControl = static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_RENDERADDON, "renderaddon");
    if (!pGUIControl)
      return nullptr;

    CGUIAddonRenderingControl *pRenderControl = new CGUIAddonRenderingControl((CGUIRenderingControl*)pGUIControl);
    return pRenderControl;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::GetControl_Slider(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    return static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_SLIDER, "slider");
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::GetControl_SettingsSlider(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    return static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_SETTINGS_SLIDER, "settings slider");
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_GUIWindow::GetControl_TextBox(void* kodiInstance, void* handle, int controlId)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIWindow::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    if (!handle)
    {
      throw std::logic_error(StringUtils::Format("Interface_GUIWindow - %s: %s/%s - No Window",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());
    }

    return static_cast<CGUIAddonWindow*>(handle)->GetAddonControl(controlId, CGUIControl::GUICONTROL_TEXTBOX, "textbox");
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

CGUIAddonWindow::CGUIAddonWindow(int id, const std::string& strXML, CAddonDll* addon)
 : CGUIMediaWindow(id, strXML.c_str()),
   CBOnInit{nullptr},
   CBOnFocus{nullptr},
   CBOnClick{nullptr},
   CBOnAction{nullptr},
   m_clientHandle{nullptr},
   m_iWindowId(id),
   m_iOldWindowId(0),
   m_bModal(false),
   m_bIsDialog(false),
   m_actionEvent(true),
   m_addon(addon)
{
  m_loadType = LOAD_ON_GUI_INIT;
}

CGUIAddonWindow::~CGUIAddonWindow(void)
{
}

CGUIControl* CGUIAddonWindow::GetAddonControl(int controlId, CGUIControl::GUICONTROLTYPES type, std::string typeName)
{
  CGUIControl* pGUIControl = dynamic_cast<CGUIControl*>(GetControl(controlId));
  if (!pGUIControl)
  {
    CLog::Log(LOGERROR, "CGUIAddonGUI_Window::%s: %s/%s - Requested GUI control Id '%i' for '%s' not present!",
                __FUNCTION__,
                TranslateType(m_addon->Type()).c_str(),
                m_addon->Name().c_str(),
                controlId, typeName.c_str());
    return nullptr;
  }
  else if (pGUIControl->GetControlType() != type)
  {
    CLog::Log(LOGERROR, "CGUIAddonGUI_Window::%s: %s/%s - Requested GUI control Id '%i' not the type '%s'!",
                __FUNCTION__,
                TranslateType(m_addon->Type()).c_str(),
                m_addon->Name().c_str(),
                controlId, typeName.c_str());
    return nullptr;
  }

  return pGUIControl;
}

bool CGUIAddonWindow::OnAction(const CAction &action)
{
  try
  {
    // Let addon decide whether it wants to hande action first
    if (CBOnAction && CBOnAction(m_clientHandle, GetAddonGUIActionType(action.GetID())))
      return true;

    return CGUIWindow::OnAction(action);
  }
  HANDLE_ADDON_EXCEPTION(m_addon);

  return CGUIWindow::OnAction(action);
}

bool CGUIAddonWindow::OnMessage(CGUIMessage& message)
{
  // TODO: We shouldn't be dropping down to CGUIWindow in any of this ideally.
  //       We have to make up our minds about what python should be doing and
  //       what this side of things should be doing
  switch (message.GetMessage())
  {
    case GUI_MSG_WINDOW_DEINIT:
    {
      return CGUIMediaWindow::OnMessage(message);
    }
    break;

    case GUI_MSG_WINDOW_INIT:
    {
      CGUIMediaWindow::OnMessage(message);
      if (CBOnInit)
      {
        try { CBOnInit(m_clientHandle); }
        HANDLE_ADDON_EXCEPTION(m_addon);
      }
      return true;
    }
    break;

    case GUI_MSG_SETFOCUS:
    {
      if (m_viewControl.HasControl(message.GetControlId()) && m_viewControl.GetCurrentControl() != (int)message.GetControlId())
      {
        m_viewControl.SetFocused();
        return true;
      }
      // check if our focused control is one of our category buttons
      int iControl = message.GetControlId();
      if (CBOnFocus)
      {
        try { CBOnFocus(m_clientHandle, iControl); }
        HANDLE_ADDON_EXCEPTION(m_addon);
      }
    }
    break;

    case GUI_MSG_FOCUSED:
    {
      if (HasID(message.GetSenderId()) && CBOnFocus)
      {
        try { CBOnFocus(m_clientHandle, message.GetControlId()); }
        HANDLE_ADDON_EXCEPTION(m_addon);
      }
    }
    break;

    case GUI_MSG_CLICKED:
    {
      int iControl=message.GetSenderId();
      // Handle Sort/View internally. Scripters shouldn't use ID 2, 3 or 4.
      if (iControl == CONTROL_BTNSORTASC) // sort asc
      {
        CLog::Log(LOGINFO, "WindowXML: Internal asc/dsc button not implemented");
        /*if (m_guiState.get())
          m_guiState->SetNextSortOrder();
        UpdateFileList();*/
        return true;
      }
      else if (iControl == CONTROL_BTNSORTBY) // sort by
      {
        CLog::Log(LOGINFO, "WindowXML: Internal sort button not implemented");
        /*if (m_guiState.get())
          m_guiState->SetNextSortMethod();
        UpdateFileList();*/
        return true;
      }

      if (CBOnClick && iControl && iControl != (int)this->GetID())
      {
        CGUIControl* controlClicked = (CGUIControl*)this->GetControl(iControl);

        // The old python way used to check list AND SELECITEM method or if its a button, checkmark.
        // Its done this way for now to allow other controls without a python version like togglebutton to still raise a onAction event
        if (controlClicked) // Will get problems if we the id is not on the window and we try to do GetControlType on it. So check to make sure it exists
        {
          if ((controlClicked->IsContainer() && (message.GetParam1() == ACTION_SELECT_ITEM ||
                                                 message.GetParam1() == ACTION_MOUSE_LEFT_CLICK)) ||
                                                 !controlClicked->IsContainer())
          {
            try { return CBOnClick(m_clientHandle, iControl); }
            HANDLE_ADDON_EXCEPTION(m_addon);
          }
          else if (controlClicked->IsContainer() && message.GetParam1() == ACTION_MOUSE_RIGHT_CLICK)
          {
//            PyXBMCAction* inf = new PyXBMCAction;
//            inf->pObject = Action_FromAction(CAction(ACTION_CONTEXT_MENU));
//            inf->pCallbackWindow = pCallbackWindow;
//
//            // aquire lock?
//            PyXBMC_AddPendingCall(Py_XBMC_Event_OnAction, inf);
//            PulseActionEvent();
          }
        }
      }
    }
    break;
  }

  return CGUIMediaWindow::OnMessage(message);
}

void CGUIAddonWindow::AllocResources(bool forceLoad /*= FALSE */)
{
  std::string tmpDir = URIUtils::GetDirectory(GetProperty("xmlfile").asString());
  std::string fallbackMediaPath;
  URIUtils::GetParentPath(tmpDir, fallbackMediaPath);
  URIUtils::RemoveSlashAtEnd(fallbackMediaPath);
  m_mediaDir = fallbackMediaPath;

  //CLog::Log(LOGDEBUG, "CGUIPythonWindowXML::AllocResources called: %s", fallbackMediaPath.c_str());
  g_TextureManager.AddTexturePath(m_mediaDir);
  CGUIMediaWindow::AllocResources(forceLoad);
  g_TextureManager.RemoveTexturePath(m_mediaDir);
}

void CGUIAddonWindow::FreeResources(bool forceUnLoad /*= FALSE */)
{
  CGUIMediaWindow::FreeResources(forceUnLoad);
}

void CGUIAddonWindow::Render()
{
  g_TextureManager.AddTexturePath(m_mediaDir);
  CGUIMediaWindow::Render();
  g_TextureManager.RemoveTexturePath(m_mediaDir);
}

void CGUIAddonWindow::Update()
{
}

void CGUIAddonWindow::AddItem(CFileItemPtr fileItem, int itemPosition)
{
  if (itemPosition == -1 || itemPosition > m_vecItems->Size())
  {
    m_vecItems->Add(fileItem);
  }
  else if (itemPosition <  -1 &&  !(itemPosition-1 < m_vecItems->Size()))
  {
    m_vecItems->AddFront(fileItem,0);
  }
  else
  {
    m_vecItems->AddFront(fileItem, itemPosition);
  }
  m_viewControl.SetItems(*m_vecItems);
  UpdateButtons();
}

void CGUIAddonWindow::RemoveItem(int itemPosition)
{
  m_vecItems->Remove(itemPosition);
  m_viewControl.SetItems(*m_vecItems);
  UpdateButtons();
}

void CGUIAddonWindow::RemoveItem(CFileItem* fileItem)
{
  m_vecItems->Remove(fileItem);
  m_viewControl.SetItems(*m_vecItems);
  UpdateButtons();
}

int CGUIAddonWindow::GetCurrentListPosition()
{
  return m_viewControl.GetSelectedItem();
}

void CGUIAddonWindow::SetCurrentListPosition(int item)
{
  m_viewControl.SetSelectedItem(item);
}

int CGUIAddonWindow::GetListSize()
{
  return m_vecItems->Size();
}

CFileItemPtr CGUIAddonWindow::GetListItem(int position)
{
  if (position < 0 || position >= m_vecItems->Size()) return CFileItemPtr();
  return m_vecItems->Get(position);
}

void CGUIAddonWindow::ClearList()
{
  ClearFileItems();

  m_viewControl.SetItems(*m_vecItems);
  UpdateButtons();
}

void CGUIAddonWindow::GetContextButtons(int itemNumber, CContextButtons &buttons)
{
  // maybe on day we can make an easy way to do this context menu
  // with out this method overriding the MediaWindow version, it will display 'Add to Favorites'
}

void CGUIAddonWindow::WaitForActionEvent(unsigned int timeout)
{
  m_actionEvent.WaitMSec(timeout);
  m_actionEvent.Reset();
}

void CGUIAddonWindow::PulseActionEvent()
{
  m_actionEvent.Set();
}

bool CGUIAddonWindow::OnClick(int iItem, const std::string &player)
{
  // Hook Over calling  CGUIMediaWindow::OnClick(iItem) results in it trying to PLAY the file item
  // which if its not media is BAD and 99 out of 100 times undesireable.
  return false;
}

// SetupShares();
/*
 CGUIMediaWindow::OnWindowLoaded() calls SetupShares() so override it
and just call UpdateButtons();
*/
void CGUIAddonWindow::SetupShares()
{
  UpdateButtons();
}


CGUIAddonWindowDialog::CGUIAddonWindowDialog(int id, const std::string& strXML, CAddonDll* addon)
: CGUIAddonWindow(id,strXML,addon)
{
  m_bRunning = false;
  m_bIsDialog = true;
}

CGUIAddonWindowDialog::~CGUIAddonWindowDialog(void)
{
}

bool CGUIAddonWindowDialog::OnMessage(CGUIMessage &message)
{
  if (message.GetMessage() == GUI_MSG_WINDOW_DEINIT)
    return CGUIWindow::OnMessage(message);

  return CGUIAddonWindow::OnMessage(message);
}

void CGUIAddonWindowDialog::Show(bool show /* = true */)
{
  unsigned int iCount = g_graphicsContext.exit();
  CApplicationMessenger::GetInstance().SendMsg(TMSG_GUI_ADDON_DIALOG, 0, show ? 1 : 0, static_cast<void*>(this));
  g_graphicsContext.restore(iCount);
}

void CGUIAddonWindowDialog::Show_Internal(bool show /* = true */)
{
  if (show)
  {
    m_bModal = true;
    m_bRunning = true;
    g_windowManager.RegisterDialog(this);

    // active this window...
    CGUIMessage msg(GUI_MSG_WINDOW_INIT, 0, 0, WINDOW_INVALID, m_iWindowId);
    OnMessage(msg);

    // this dialog is derived from GUiMediaWindow
    // make sure it is rendered last
    m_renderOrder = RENDER_ORDER_DIALOG;
    while (m_bRunning && !g_application.m_bStop)
    {
      ProcessRenderLoop();
    }
  }
  else // hide
  {
    m_bRunning = false;

    CGUIMessage msg(GUI_MSG_WINDOW_DEINIT, 0, 0);
    OnMessage(msg);

    g_windowManager.RemoveDialog(GetID());
  }
}

} /* extern "C" */
} /* namespace ADDON */
