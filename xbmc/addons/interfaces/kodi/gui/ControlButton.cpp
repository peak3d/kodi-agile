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

#include "ControlButton.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlButton.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUIButtonControl.h"
#include "utils/StringUtils.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIControlButton::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlButton.SetVisible = SetVisible;
  funcTable->toKodi.kodi_gui->controlButton.SetEnabled = SetEnabled;

  funcTable->toKodi.kodi_gui->controlButton.SetLabel = SetLabel;
  funcTable->toKodi.kodi_gui->controlButton.GetLabel = GetLabel;

  funcTable->toKodi.kodi_gui->controlButton.SetLabel2 = SetLabel2;
  funcTable->toKodi.kodi_gui->controlButton.GetLabel2 = GetLabel2;
}

void Interface_GUIControlButton::SetVisible(void* kodiInstance, void* handle, bool yesNo)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (handle)
      static_cast<CGUIButtonControl*>(handle)->SetVisible(yesNo);
    else
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlButton::%s - invalid handler data", __FUNCTION__).c_str());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlButton::SetEnabled(void* kodiInstance, void* handle, bool yesNo)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (handle)
      static_cast<CGUIButtonControl*>(handle)->SetEnabled(yesNo);
    else
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlButton::%s - invalid handler data", __FUNCTION__).c_str());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlButton::SetLabel(void* kodiInstance, void* handle, const char *label)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (handle)
      static_cast<CGUIButtonControl *>(handle)->SetLabel(label);
    else
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlButton::%s - invalid handler data", __FUNCTION__).c_str());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlButton::GetLabel(void* kodiInstance, void* handle, char &label, unsigned int &iMaxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlButton::%s - invalid handler data", __FUNCTION__).c_str());

    CGUIButtonControl* pButton = static_cast<CGUIButtonControl *>(handle);
    std::string text = pButton->GetLabel();
    strncpy(&label, text.c_str(), iMaxStringSize);
    iMaxStringSize = text.length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlButton::SetLabel2(void* kodiInstance, void* handle, const char *label)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (handle)
      static_cast<CGUIButtonControl *>(handle)->SetLabel2(label);
    else
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlButton::%s - invalid handler data", __FUNCTION__).c_str());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlButton::GetLabel2(void* kodiInstance, void* handle, char &label, unsigned int &iMaxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlButton::%s - invalid handler data", __FUNCTION__).c_str());

    CGUIButtonControl* pButton = static_cast<CGUIButtonControl *>(handle);
    std::string text = pButton->GetLabel2();
    strncpy(&label, text.c_str(), iMaxStringSize);
    iMaxStringSize = text.length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
