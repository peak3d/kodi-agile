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

#include "ControlSlider.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlSlider.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUISliderControl.h"
#include "guilib/GUIWindowManager.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIControlSlider::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlSlider.SetVisible = SetVisible;
  funcTable->toKodi.kodi_gui->controlSlider.SetEnabled = SetEnabled;

  funcTable->toKodi.kodi_gui->controlSlider.Reset = Reset;
  funcTable->toKodi.kodi_gui->controlSlider.GetDescription = GetDescription;

  funcTable->toKodi.kodi_gui->controlSlider.SetIntRange = SetIntRange;
  funcTable->toKodi.kodi_gui->controlSlider.SetIntValue = SetIntValue;
  funcTable->toKodi.kodi_gui->controlSlider.GetIntValue = GetIntValue;
  funcTable->toKodi.kodi_gui->controlSlider.SetIntInterval = SetIntInterval;

  funcTable->toKodi.kodi_gui->controlSlider.SetPercentage = SetPercentage;
  funcTable->toKodi.kodi_gui->controlSlider.GetPercentage = GetPercentage;

  funcTable->toKodi.kodi_gui->controlSlider.SetFloatRange = SetFloatRange;
  funcTable->toKodi.kodi_gui->controlSlider.SetFloatValue = SetFloatValue;
  funcTable->toKodi.kodi_gui->controlSlider.GetFloatValue = GetFloatValue;
  funcTable->toKodi.kodi_gui->controlSlider.SetFloatInterval = SetFloatInterval;
}

void Interface_GUIControlSlider::SetVisible(void* kodiInstance, void* handle, bool visible)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISliderControl*>(handle)->SetVisible(visible);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSlider::SetEnabled(void* kodiInstance, void* handle, bool enabled)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISliderControl*>(handle)->SetEnabled(enabled);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSlider::Reset(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISliderControl* pControl = static_cast<CGUISliderControl*>(handle);

    CGUIMessage msg(GUI_MSG_LABEL_RESET, pControl->GetParentID(), pControl->GetID());
    g_windowManager.SendThreadMessage(msg, pControl->GetParentID());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSlider::GetDescription(void* kodiInstance, void* handle, char &label, unsigned int &maxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    std::string text = static_cast<CGUISliderControl*>(handle)->GetDescription();
    strncpy(&label, text.c_str(), maxStringSize);
    maxStringSize = text.length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSlider::SetIntRange(void* kodiInstance, void* handle, int iStart, int iEnd)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISliderControl* pControl = static_cast<CGUISliderControl *>(handle);
    pControl->SetType(SLIDER_CONTROL_TYPE_INT);
    pControl->SetRange(iStart, iEnd);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSlider::SetIntValue(void* kodiInstance, void* handle, int value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISliderControl* pControl = static_cast<CGUISliderControl *>(handle);
    pControl->SetType(SLIDER_CONTROL_TYPE_INT);
    pControl->SetIntValue(value);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

int Interface_GUIControlSlider::GetIntValue(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return 0;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    return static_cast<CGUISliderControl*>(handle)->GetIntValue();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0;
}

void Interface_GUIControlSlider::SetIntInterval(void* kodiInstance, void* handle, int interval)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISliderControl*>(handle)->SetIntInterval(interval);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSlider::SetPercentage(void* kodiInstance, void* handle, float percent)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISliderControl* pControl = static_cast<CGUISliderControl *>(handle);
    pControl->SetType(SLIDER_CONTROL_TYPE_PERCENTAGE);
    pControl->SetPercentage(percent);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

float Interface_GUIControlSlider::GetPercentage(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return 0.0f;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    return static_cast<CGUISliderControl*>(handle)->GetPercentage();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0.0f;
}

void Interface_GUIControlSlider::SetFloatRange(void* kodiInstance, void* handle, float start, float end)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISliderControl* pControl = static_cast<CGUISliderControl *>(handle);
    pControl->SetType(SLIDER_CONTROL_TYPE_FLOAT);
    pControl->SetFloatRange(start, end);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSlider::SetFloatValue(void* kodiInstance, void* handle, float value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISliderControl* pControl = static_cast<CGUISliderControl *>(handle);
    pControl->SetType(SLIDER_CONTROL_TYPE_FLOAT);
    pControl->SetFloatValue(value);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

float Interface_GUIControlSlider::GetFloatValue(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return 0.0f;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    return static_cast<CGUISliderControl*>(handle)->GetFloatValue();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0.0f;
}

void Interface_GUIControlSlider::SetFloatInterval(void* kodiInstance, void* handle, float interval)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSlider::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSlider::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISliderControl*>(handle)->SetFloatInterval(interval);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
