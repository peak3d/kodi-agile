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

#include "ControlTextBox.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlTextBox.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUITextBox.h"
#include "guilib/GUIWindowManager.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIControlTextBox::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlTextBox.SetVisible = SetVisible;
  funcTable->toKodi.kodi_gui->controlTextBox.Reset = Reset;
  funcTable->toKodi.kodi_gui->controlTextBox.SetText = SetText;
  funcTable->toKodi.kodi_gui->controlTextBox.GetText = GetText;
  funcTable->toKodi.kodi_gui->controlTextBox.Scroll = Scroll;
  funcTable->toKodi.kodi_gui->controlTextBox.SetAutoScrolling = SetAutoScrolling;
}

void Interface_GUIControlTextBox::SetVisible(void* kodiInstance, void* handle, bool visible)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlTextBox::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlTextBox::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUITextBox*>(handle)->SetVisible(visible);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlTextBox::Reset(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlTextBox::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlTextBox::%s - invalid handler data", __FUNCTION__).c_str());

    CGUITextBox* textBox = static_cast<CGUITextBox*>(handle);

    CGUIMessage msg(GUI_MSG_LABEL_RESET, textBox->GetParentID(), textBox->GetID());
    g_windowManager.SendThreadMessage(msg, textBox->GetParentID());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlTextBox::SetText(void* kodiInstance, void* handle, const char *text)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlTextBox::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !text)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlTextBox::%s - invalid handler data", __FUNCTION__).c_str());

    CGUITextBox* textBox = static_cast<CGUITextBox*>(handle);

    // create message
    CGUIMessage msg(GUI_MSG_LABEL_SET, textBox->GetParentID(), textBox->GetID());
    msg.SetLabel(text);

    // send message
    g_windowManager.SendThreadMessage(msg, textBox->GetParentID());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlTextBox::GetText(void* kodiInstance, void* handle, char &text, unsigned int &maxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlTextBox::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlTextBox::%s - invalid handler data", __FUNCTION__).c_str());

    std::string tmpText = static_cast<CGUITextBox*>(handle)->GetDescription();
    strncpy(&text, tmpText.c_str(), maxStringSize);
    maxStringSize = tmpText.size();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlTextBox::Scroll(void* kodiInstance, void* handle, unsigned int position)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlTextBox::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlTextBox::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUITextBox*>(handle)->Scroll(position);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlTextBox::SetAutoScrolling(void* kodiInstance, void* handle, int delay, int time, int repeat)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlTextBox::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlTextBox::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUITextBox*>(handle)->SetAutoScrolling(delay, time, repeat);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
