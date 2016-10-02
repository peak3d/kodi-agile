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

#include "DialogExtendedProgressBar.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/DialogExtendedProgress.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "dialogs/GUIDialogExtendedProgressBar.h"
#include "guilib/GUIWindowManager.h"
#include "utils/log.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIDialogExtendedProgress::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->dialogExtendedProgress.New  = New;
  funcTable->toKodi.kodi_gui->dialogExtendedProgress.Delete = Delete;
  funcTable->toKodi.kodi_gui->dialogExtendedProgress.Title = Title;
  funcTable->toKodi.kodi_gui->dialogExtendedProgress.SetTitle = SetTitle;
  funcTable->toKodi.kodi_gui->dialogExtendedProgress.Text = Text;
  funcTable->toKodi.kodi_gui->dialogExtendedProgress.SetText = SetText;
  funcTable->toKodi.kodi_gui->dialogExtendedProgress.IsFinished = IsFinished;
  funcTable->toKodi.kodi_gui->dialogExtendedProgress.MarkFinished = MarkFinished;
  funcTable->toKodi.kodi_gui->dialogExtendedProgress.Percentage = Percentage;
  funcTable->toKodi.kodi_gui->dialogExtendedProgress.SetPercentage = SetPercentage;
  funcTable->toKodi.kodi_gui->dialogExtendedProgress.SetProgress = SetProgress;
}

void* Interface_GUIDialogExtendedProgress::New(void* kodiInstance, const char *title)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogExtendedProgress::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    // setup the progress dialog
    CGUIDialogExtendedProgressBar* dialog = dynamic_cast<CGUIDialogExtendedProgressBar*>(g_windowManager.GetWindow(WINDOW_DIALOG_EXT_PROGRESS));
    if (!title || !dialog)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid handler data (title='%p', dialog='%p')", __FUNCTION__, title, dialog).c_str());

    CGUIDialogProgressBarHandle* dlgProgressHandle = dialog->GetHandle(title);
    return dlgProgressHandle;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void Interface_GUIDialogExtendedProgress::Delete(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogExtendedProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid add-on data", __FUNCTION__).c_str());

    CGUIDialogProgressBarHandle* dlgProgressHandle = static_cast<CGUIDialogProgressBarHandle *>(handle);
    dlgProgressHandle->MarkFinished();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogExtendedProgress::Title(void* kodiInstance, void* handle, char &title, unsigned int &maxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogExtendedProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid add-on data", __FUNCTION__).c_str());

    CGUIDialogProgressBarHandle* dlgProgressHandle = static_cast<CGUIDialogProgressBarHandle *>(handle);
    strncpy(&title, dlgProgressHandle->Title().c_str(), maxStringSize);
    maxStringSize = dlgProgressHandle->Title().length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogExtendedProgress::SetTitle(void* kodiInstance, void* handle, const char *title)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogExtendedProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !title)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid handler data (handle='%p', title='%p')", __FUNCTION__, handle, title).c_str());

    CGUIDialogProgressBarHandle* dlgProgressHandle = static_cast<CGUIDialogProgressBarHandle *>(handle);
    dlgProgressHandle->SetTitle(title);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogExtendedProgress::Text(void* kodiInstance, void* handle, char &text, unsigned int &maxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogExtendedProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid add-on data", __FUNCTION__).c_str());

    CGUIDialogProgressBarHandle* dlgProgressHandle = static_cast<CGUIDialogProgressBarHandle *>(handle);
    strncpy(&text, dlgProgressHandle->Text().c_str(), maxStringSize);
    maxStringSize = dlgProgressHandle->Text().length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogExtendedProgress::SetText(void* kodiInstance, void* handle, const char *text)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogExtendedProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !text)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid handler data (handle='%p', text='%p')", __FUNCTION__, handle, text).c_str());

    CGUIDialogProgressBarHandle* dlgProgressHandle = static_cast<CGUIDialogProgressBarHandle *>(handle);
    dlgProgressHandle->SetText(text);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

bool Interface_GUIDialogExtendedProgress::IsFinished(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogExtendedProgress::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid add-on data", __FUNCTION__).c_str());

    CGUIDialogProgressBarHandle* dlgProgressHandle = static_cast<CGUIDialogProgressBarHandle *>(handle);
    return dlgProgressHandle->IsFinished();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

void Interface_GUIDialogExtendedProgress::MarkFinished(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogExtendedProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid add-on data", __FUNCTION__).c_str());

    CGUIDialogProgressBarHandle* dlgProgressHandle = static_cast<CGUIDialogProgressBarHandle *>(handle);
    dlgProgressHandle->MarkFinished();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

float Interface_GUIDialogExtendedProgress::Percentage(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogExtendedProgress::%s - invalid data", __FUNCTION__);
    return 0.0f;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid add-on data", __FUNCTION__).c_str());

    CGUIDialogProgressBarHandle* dlgProgressHandle = static_cast<CGUIDialogProgressBarHandle *>(handle);
    return dlgProgressHandle->Percentage();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0.0f;
}

void Interface_GUIDialogExtendedProgress::SetPercentage(void* kodiInstance, void* handle, float percentage)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogExtendedProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid add-on data", __FUNCTION__).c_str());

    CGUIDialogProgressBarHandle* dlgProgressHandle = static_cast<CGUIDialogProgressBarHandle *>(handle);
    dlgProgressHandle->SetPercentage(percentage);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogExtendedProgress::SetProgress(void* kodiInstance, void* handle, int currentItem, int itemCount)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogExtendedProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid add-on data", __FUNCTION__).c_str());

    CGUIDialogProgressBarHandle* dlgProgressHandle = static_cast<CGUIDialogProgressBarHandle *>(handle);
    dlgProgressHandle->SetProgress(currentItem, itemCount);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
