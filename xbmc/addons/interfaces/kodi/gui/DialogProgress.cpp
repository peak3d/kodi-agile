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

#include "DialogProgress.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/DialogProgress.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "dialogs/GUIDialogProgress.h"
#include "guilib/GUIWindowManager.h"
#include "utils/log.h"
#include "utils/Variant.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIDialogProgress::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->dialogProgress.New = New;
  funcTable->toKodi.kodi_gui->dialogProgress.Delete = Delete;
  funcTable->toKodi.kodi_gui->dialogProgress.Open = Open;
  funcTable->toKodi.kodi_gui->dialogProgress.SetHeading = SetHeading;
  funcTable->toKodi.kodi_gui->dialogProgress.SetLine = SetLine;
  funcTable->toKodi.kodi_gui->dialogProgress.SetCanCancel = SetCanCancel;
  funcTable->toKodi.kodi_gui->dialogProgress.IsCanceled = IsCanceled;
  funcTable->toKodi.kodi_gui->dialogProgress.SetPercentage = SetPercentage;
  funcTable->toKodi.kodi_gui->dialogProgress.GetPercentage = GetPercentage;
  funcTable->toKodi.kodi_gui->dialogProgress.ShowProgressBar = ShowProgressBar;
  funcTable->toKodi.kodi_gui->dialogProgress.SetProgressMax = SetProgressMax;
  funcTable->toKodi.kodi_gui->dialogProgress.SetProgressAdvance = SetProgressAdvance;
  funcTable->toKodi.kodi_gui->dialogProgress.Abort = Abort;
}

void* Interface_GUIDialogProgress::New(void *kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    CGUIDialogProgress *dialog = dynamic_cast<CGUIDialogProgress*>(g_windowManager.GetWindow(WINDOW_DIALOG_PROGRESS));
    if (!dialog)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid handler data (dialog='%p')", __FUNCTION__, dialog).c_str());

    return dialog;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void Interface_GUIDialogProgress::Delete(void *kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid add-on data", __FUNCTION__).c_str());

    static_cast<CGUIDialogProgress*>(handle)->Close();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogProgress::Open(void *kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid add-on data", __FUNCTION__).c_str());

    static_cast<CGUIDialogProgress*>(handle)->Open();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogProgress::SetHeading(void *kodiInstance, void* handle, const char *heading)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !heading)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid handler data (handle='%p', heading='%p')", __FUNCTION__, handle, heading).c_str());

    static_cast<CGUIDialogProgress*>(handle)->SetHeading(heading);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogProgress::SetLine(void *kodiInstance, void* handle, unsigned int iLine, const char *line)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !line)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid handler data (handle='%p', line='%p')", __FUNCTION__, handle, line).c_str());

    static_cast<CGUIDialogProgress*>(handle)->SetLine(iLine, line);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogProgress::SetCanCancel(void *kodiInstance, void* handle, bool canCancel)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid handler data (handle='%p')", __FUNCTION__, handle).c_str());

    static_cast<CGUIDialogProgress*>(handle)->SetCanCancel(canCancel);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

bool Interface_GUIDialogProgress::IsCanceled(void *kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid handler data (handle='%p')", __FUNCTION__, handle).c_str());

    return static_cast<CGUIDialogProgress*>(handle)->IsCanceled();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

void Interface_GUIDialogProgress::SetPercentage(void *kodiInstance, void* handle, int percentage)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid handler data (handle='%p')", __FUNCTION__, handle).c_str());

    static_cast<CGUIDialogProgress*>(handle)->SetPercentage(percentage);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

int Interface_GUIDialogProgress::GetPercentage(void *kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return 0;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid handler data (handle='%p')", __FUNCTION__, handle).c_str());

    return static_cast<CGUIDialogProgress*>(handle)->GetPercentage();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0;
}

void Interface_GUIDialogProgress::ShowProgressBar(void *kodiInstance, void* handle, bool onOff)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid handler data (handle='%p')", __FUNCTION__, handle).c_str());

    static_cast<CGUIDialogProgress*>(handle)->ShowProgressBar(onOff);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogProgress::SetProgressMax(void *kodiInstance, void* handle, int max)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid handler data (handle='%p')", __FUNCTION__, handle).c_str());

    static_cast<CGUIDialogProgress*>(handle)->SetProgressMax(max);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogProgress::SetProgressAdvance(void *kodiInstance, void* handle, int nSteps)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid handler data (handle='%p')", __FUNCTION__, handle).c_str());

    static_cast<CGUIDialogProgress*>(handle)->SetProgressAdvance(nSteps);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

bool Interface_GUIDialogProgress::Abort(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogProgress::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogProgress::%s - invalid handler data (handle='%p')", __FUNCTION__, handle).c_str());

    return static_cast<CGUIDialogProgress*>(handle)->Abort();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

} /* extern "C" */
} /* namespace ADDON */
