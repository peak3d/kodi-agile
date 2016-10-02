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

#include "ControlRadioButton.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlRadioButton.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUIRadioButtonControl.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIControlRadioButton::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlRadioButton.SetVisible = SetVisible;
  funcTable->toKodi.kodi_gui->controlRadioButton.SetEnabled = SetEnabled;

  funcTable->toKodi.kodi_gui->controlRadioButton.SetLabel = SetLabel;
  funcTable->toKodi.kodi_gui->controlRadioButton.GetLabel = GetLabel;

  funcTable->toKodi.kodi_gui->controlRadioButton.SetSelected = SetSelected;
  funcTable->toKodi.kodi_gui->controlRadioButton.IsSelected = IsSelected;
}

void Interface_GUIControlRadioButton::SetVisible(void* kodiInstance, void* handle, bool visible)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlRadioButton::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlRadioButton::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIRadioButtonControl*>(handle)->SetVisible(visible);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlRadioButton::SetEnabled(void* kodiInstance, void* handle, bool enabled)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlRadioButton::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlRadioButton::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIRadioButtonControl*>(handle)->SetEnabled(enabled);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlRadioButton::SetLabel(void* kodiInstance, void* handle, const char *label)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlRadioButton::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !label)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlRadioButton::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIRadioButtonControl*>(handle)->SetLabel(label);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlRadioButton::GetLabel(void* kodiInstance, void* handle, char &text, unsigned int &maxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlRadioButton::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlRadioButton::%s - invalid handler data", __FUNCTION__).c_str());

    CGUIRadioButtonControl* pRadioButton = static_cast<CGUIRadioButtonControl *>(handle);
    strncpy(&text, pRadioButton->GetLabel().c_str(), maxStringSize);
    maxStringSize = pRadioButton->GetLabel().length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlRadioButton::SetSelected(void* kodiInstance, void* handle, bool selected)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlRadioButton::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlRadioButton::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIRadioButtonControl*>(handle)->SetSelected(selected);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

bool Interface_GUIControlRadioButton::IsSelected(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlRadioButton::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlRadioButton::%s - invalid handler data", __FUNCTION__).c_str());

    return static_cast<CGUIRadioButtonControl *>(handle)->IsSelected();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

} /* extern "C" */
} /* namespace ADDON */
