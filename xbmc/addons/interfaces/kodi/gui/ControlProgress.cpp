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

#include "ControlProgress.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlProgress.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUIProgressControl.h"
#include "guilib/GUIWindowManager.h"
#include "utils/log.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIControlProgress::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlProgress.SetVisible = SetVisible;
  funcTable->toKodi.kodi_gui->controlProgress.SetPercentage = SetPercentage;
  funcTable->toKodi.kodi_gui->controlProgress.GetPercentage = GetPercentage;
}

void Interface_GUIControlProgress::SetVisible(void* kodiInstance, void* handle, bool visible)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlProgress::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIProgressControl*>(handle)->SetVisible(visible);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlProgress::SetPercentage(void* kodiInstance, void* handle, float percent)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlProgress::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlProgress::%s - invalid handler data", __FUNCTION__).c_str());

    if (handle)
      static_cast<CGUIProgressControl*>(handle)->SetPercentage(percent);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

float Interface_GUIControlProgress::GetPercentage(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlProgress::%s - invalid data", __FUNCTION__);
    return 0.0f;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlProgress::%s - invalid handler data", __FUNCTION__).c_str());

    return static_cast<CGUIProgressControl*>(handle)->GetPercentage();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0.0f;
}

} /* extern "C" */
} /* namespace ADDON */
