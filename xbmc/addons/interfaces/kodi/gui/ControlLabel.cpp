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

#include "ControlLabel.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlLabel.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUILabelControl.h"
#include "guilib/GUIWindowManager.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIControlLabel::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlLabel.SetVisible = SetVisible;
  funcTable->toKodi.kodi_gui->controlLabel.SetLabel = SetLabel;
  funcTable->toKodi.kodi_gui->controlLabel.GetLabel = GetLabel;
}

void Interface_GUIControlLabel::SetVisible(void* kodiInstance, void* handle, bool visible)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlLabel::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlLabel::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUILabelControl*>(handle)->SetVisible(visible);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlLabel::SetLabel(void* kodiInstance, void* handle, const char *label)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlLabel::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlLabel::%s - invalid handler data", __FUNCTION__).c_str());

    CGUILabelControl* pControl = static_cast<CGUILabelControl*>(handle);

    // create message
    CGUIMessage msg(GUI_MSG_LABEL_SET, pControl->GetParentID(), pControl->GetID());
    msg.SetLabel(label);

    // send message
    g_windowManager.SendThreadMessage(msg, pControl->GetParentID());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlLabel::GetLabel(void* kodiInstance, void* handle, char &label, unsigned int &iMaxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlLabel::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlLabel::%s - invalid handler data", __FUNCTION__).c_str());

    std::string text = static_cast<CGUILabelControl*>(handle)->GetDescription(); // GetDescription = label
    strncpy(&label, text.c_str(), iMaxStringSize);
    iMaxStringSize = text.length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
