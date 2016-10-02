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

#include "ControlSpin.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlSpin.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUISpinControlEx.h"
#include "guilib/GUIWindowManager.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIControlSpin::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlSpin.SetVisible = SetVisible;
  funcTable->toKodi.kodi_gui->controlSpin.SetEnabled = SetEnabled;

  funcTable->toKodi.kodi_gui->controlSpin.SetText = SetText;
  funcTable->toKodi.kodi_gui->controlSpin.Reset = Reset;
  funcTable->toKodi.kodi_gui->controlSpin.SetType = SetType;

  funcTable->toKodi.kodi_gui->controlSpin.AddStringLabel = AddStringLabel;
  funcTable->toKodi.kodi_gui->controlSpin.SetStringValue = SetStringValue;
  funcTable->toKodi.kodi_gui->controlSpin.GetStringValue = GetStringValue;

  funcTable->toKodi.kodi_gui->controlSpin.AddIntLabel = AddIntLabel;
  funcTable->toKodi.kodi_gui->controlSpin.SetIntRange = SetIntRange;
  funcTable->toKodi.kodi_gui->controlSpin.SetIntValue = SetIntValue;
  funcTable->toKodi.kodi_gui->controlSpin.GetIntValue = GetIntValue;

  funcTable->toKodi.kodi_gui->controlSpin.SetFloatRange = SetFloatRange;
  funcTable->toKodi.kodi_gui->controlSpin.SetFloatValue = SetFloatValue;
  funcTable->toKodi.kodi_gui->controlSpin.GetFloatValue = GetFloatValue;
  funcTable->toKodi.kodi_gui->controlSpin.SetFloatInterval = SetFloatInterval;
}

void Interface_GUIControlSpin::SetVisible(void* kodiInstance, void* handle, bool visible)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISpinControlEx*>(handle)->SetVisible(visible);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSpin::SetEnabled(void* kodiInstance, void* handle, bool enabled)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISpinControlEx*>(handle)->SetEnabled(enabled);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSpin::SetText(void* kodiInstance, void* handle, const char *text)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !text)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISpinControlEx* pControl = static_cast<CGUISpinControlEx*>(handle);

    // create message
    CGUIMessage msg(GUI_MSG_LABEL_SET, pControl->GetParentID(), pControl->GetID());
    msg.SetLabel(text);

    // send message
    g_windowManager.SendThreadMessage(msg, pControl->GetParentID());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSpin::Reset(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    CGUISpinControlEx* pControl = static_cast<CGUISpinControlEx*>(handle);

    CGUIMessage msg(GUI_MSG_LABEL_RESET, pControl->GetParentID(), pControl->GetID());
    g_windowManager.SendThreadMessage(msg, pControl->GetParentID());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSpin::SetType(void* kodiInstance, void* handle, int type)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISpinControlEx*>(handle)->SetType(type);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}


void Interface_GUIControlSpin::AddStringLabel(void* kodiInstance, void* handle, const char* label, const char* value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !label || !value)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISpinControlEx*>(handle)->AddLabel(std::string(label), std::string(value));
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSpin::SetStringValue(void* kodiInstance, void* handle, const char* value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !value)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISpinControlEx*>(handle)->SetStringValue(std::string(value));
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSpin::GetStringValue(void* kodiInstance, void* handle, char &value, unsigned int &maxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    std::string text = static_cast<CGUISpinControlEx*>(handle)->GetStringValue();
    strncpy(&value, text.c_str(), maxStringSize);
    maxStringSize = text.length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSpin::AddIntLabel(void* kodiInstance, void* handle, const char* label, int value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !label)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISpinControlEx*>(handle)->AddLabel(std::string(label), value);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSpin::SetIntRange(void* kodiInstance, void* handle, int start, int end)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISpinControlEx*>(handle)->SetRange(start, end);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSpin::SetIntValue(void* kodiInstance, void* handle, int value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISpinControlEx*>(handle)->SetValue(value);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

int Interface_GUIControlSpin::GetIntValue(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return 0;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    return static_cast<CGUISpinControlEx*>(handle)->GetValue();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0;
}

void Interface_GUIControlSpin::SetFloatRange(void* kodiInstance, void* handle, float start, float end)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISpinControlEx*>(handle)->SetFloatRange(start, end);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlSpin::SetFloatValue(void* kodiInstance, void* handle, float value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISpinControlEx*>(handle)->SetFloatValue(value);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

float Interface_GUIControlSpin::GetFloatValue(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return 0.0f;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    return static_cast<CGUISpinControlEx*>(handle)->GetFloatValue();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0.0f;
}

void Interface_GUIControlSpin::SetFloatInterval(void* kodiInstance, void* handle, float interval)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlSpin::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlSpin::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUISpinControlEx*>(handle)->SetFloatInterval(interval);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
