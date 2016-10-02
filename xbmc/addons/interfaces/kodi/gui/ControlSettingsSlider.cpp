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

#include "ControlSettingsSlider.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlSettingsSlider.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUISettingsSliderControl.h"
#include "guilib/GUIWindowManager.h"

using namespace ADDON;

namespace ADDON
{
extern "C"
{

void Interface_GUIControlSettingsSlider::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlSettingsSlider.SetVisible = SetVisible;
  funcTable->toKodi.kodi_gui->controlSettingsSlider.SetEnabled = SetEnabled;

  funcTable->toKodi.kodi_gui->controlSettingsSlider.SetText = SetText;
  funcTable->toKodi.kodi_gui->controlSettingsSlider.Reset = Reset;

  funcTable->toKodi.kodi_gui->controlSettingsSlider.SetIntRange = SetIntRange;
  funcTable->toKodi.kodi_gui->controlSettingsSlider.SetIntValue = SetIntValue;
  funcTable->toKodi.kodi_gui->controlSettingsSlider.GetIntValue = GetIntValue;
  funcTable->toKodi.kodi_gui->controlSettingsSlider.SetIntInterval = SetIntInterval;

  funcTable->toKodi.kodi_gui->controlSettingsSlider.SetPercentage = SetPercentage;
  funcTable->toKodi.kodi_gui->controlSettingsSlider.GetPercentage = GetPercentage;

  funcTable->toKodi.kodi_gui->controlSettingsSlider.SetFloatRange = SetFloatRange;
  funcTable->toKodi.kodi_gui->controlSettingsSlider.SetFloatValue = SetFloatValue;
  funcTable->toKodi.kodi_gui->controlSettingsSlider.GetFloatValue = GetFloatValue;
  funcTable->toKodi.kodi_gui->controlSettingsSlider.SetFloatInterval = SetFloatInterval;
}

void Interface_GUIControlSettingsSlider::SetVisible(void* kodiInstance, void* handle, bool visible)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISettingsSliderControl*>(handle)->SetVisible(visible);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSettingsSlider::SetEnabled(void* kodiInstance, void* handle, bool enabled)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISettingsSliderControl*>(handle)->SetEnabled(enabled);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSettingsSlider::SetText(void* kodiInstance, void* handle, const char *text)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !text)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISettingsSliderControl* pControl = static_cast<CGUISettingsSliderControl*>(handle);

    // create message
    CGUIMessage msg(GUI_MSG_LABEL_SET, pControl->GetParentID(), pControl->GetID());
    msg.SetLabel(text);

    // send message
    g_windowManager.SendThreadMessage(msg, pControl->GetParentID());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSettingsSlider::Reset(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISettingsSliderControl* pControl = static_cast<CGUISettingsSliderControl*>(handle);

    CGUIMessage msg(GUI_MSG_LABEL_RESET, pControl->GetParentID(), pControl->GetID());
    g_windowManager.SendThreadMessage(msg, pControl->GetParentID());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSettingsSlider::SetIntRange(void* kodiInstance, void* handle, int start, int end)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISettingsSliderControl* pControl = static_cast<CGUISettingsSliderControl*>(handle);
    pControl->SetType(SLIDER_CONTROL_TYPE_INT);
    pControl->SetRange(start, end);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSettingsSlider::SetIntValue(void* kodiInstance, void* handle, int value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISettingsSliderControl* pControl = static_cast<CGUISettingsSliderControl*>(handle);
    pControl->SetType(SLIDER_CONTROL_TYPE_INT);
    pControl->SetIntValue(value);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

int Interface_GUIControlSettingsSlider::GetIntValue(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return -1;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISettingsSliderControl*>(handle)->GetIntValue();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

void Interface_GUIControlSettingsSlider::SetIntInterval(void* kodiInstance, void* handle, int interval)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISettingsSliderControl*>(handle)->SetIntInterval(interval);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSettingsSlider::SetPercentage(void* kodiInstance, void* handle, float percent)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISettingsSliderControl* pControl = static_cast<CGUISettingsSliderControl*>(handle);
    pControl->SetType(SLIDER_CONTROL_TYPE_PERCENTAGE);
    pControl->SetPercentage(percent);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

float Interface_GUIControlSettingsSlider::GetPercentage(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return 0.0f;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    return static_cast<CGUISettingsSliderControl*>(handle)->GetPercentage();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0.0f;
}

void Interface_GUIControlSettingsSlider::SetFloatRange(void* kodiInstance, void* handle, float start, float end)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISettingsSliderControl* pControl = static_cast<CGUISettingsSliderControl*>(handle);
    pControl->SetType(SLIDER_CONTROL_TYPE_FLOAT);
    pControl->SetFloatRange(start, end);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSettingsSlider::SetFloatValue(void* kodiInstance, void* handle, float value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISettingsSliderControl* pControl = static_cast<CGUISettingsSliderControl*>(handle);
    pControl->SetType(SLIDER_CONTROL_TYPE_FLOAT);
    pControl->SetFloatValue(value);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

float Interface_GUIControlSettingsSlider::GetFloatValue(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return 0.0f;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    return static_cast<CGUISettingsSliderControl*>(handle)->GetFloatValue();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0.0f;
}

void Interface_GUIControlSettingsSlider::SetFloatInterval(void* kodiInstance, void* handle, float interval)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSettingsSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSettingsSlider::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISettingsSliderControl*>(handle)->SetFloatInterval(interval);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
