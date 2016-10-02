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

#include "ControlFadeLabel.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlFadeLabel.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUIFadeLabelControl.h"
#include "guilib/GUIWindowManager.h"
#include "utils/log.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIControlFadeLabel::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlFadeLabel.SetVisible = SetVisible;
  funcTable->toKodi.kodi_gui->controlFadeLabel.AddLabel = AddLabel;
  funcTable->toKodi.kodi_gui->controlFadeLabel.GetLabel = GetLabel;
  funcTable->toKodi.kodi_gui->controlFadeLabel.SetScrolling = SetScrolling;
  funcTable->toKodi.kodi_gui->controlFadeLabel.Reset = Reset;
}

void Interface_GUIControlFadeLabel::SetVisible(void *kodiInstance, void* handle, bool visible)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlFadeLabel::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlFadeLabel::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIFadeLabelControl*>(handle)->SetVisible(visible);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlFadeLabel::AddLabel(void *kodiInstance, void* handle, const char *label)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlFadeLabel::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle || !label)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlFadeLabel::%s - invalid handler data", __FUNCTION__).c_str());

    CGUIMessage msg(GUI_MSG_LABEL_ADD, static_cast<CGUIFadeLabelControl*>(handle)->GetParentID(), static_cast<CGUIFadeLabelControl*>(handle)->GetID());
    msg.SetLabel(label);
    static_cast<CGUIFadeLabelControl*>(handle)->OnMessage(msg);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlFadeLabel::GetLabel(void *kodiInstance, void* handle, char &label, unsigned int &maxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlFadeLabel::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlFadeLabel::%s - invalid handler data", __FUNCTION__).c_str());

    CGUIMessage msg(GUI_MSG_ITEM_SELECTED, static_cast<CGUIFadeLabelControl*>(handle)->GetParentID(), static_cast<CGUIFadeLabelControl*>(handle)->GetID());
    static_cast<CGUIFadeLabelControl*>(handle)->OnMessage(msg);
    std::string text = msg.GetLabel();
    strncpy(&label, text.c_str(), maxStringSize);
    maxStringSize = text.length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlFadeLabel::SetScrolling(void *kodiInstance, void* handle, bool scroll)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlFadeLabel::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlFadeLabel::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIFadeLabelControl*>(handle)->SetScrolling(scroll);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlFadeLabel::Reset(void *kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlFadeLabel::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlFadeLabel::%s - invalid handler data", __FUNCTION__).c_str());

    CGUIMessage msg(GUI_MSG_LABEL_RESET, static_cast<CGUIFadeLabelControl*>(handle)->GetParentID(), static_cast<CGUIFadeLabelControl*>(handle)->GetID());
    static_cast<CGUIFadeLabelControl*>(handle)->OnMessage(msg);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
