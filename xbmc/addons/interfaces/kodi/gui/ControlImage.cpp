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

#include "ControlImage.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlImage.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUIImage.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIControlImage::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlImage.SetVisible = SetVisible;
  funcTable->toKodi.kodi_gui->controlImage.SetFileName = SetFileName;
  funcTable->toKodi.kodi_gui->controlImage.SetColorDiffuse = SetColorDiffuse;
}

void Interface_GUIControlImage::SetVisible(void *kodiInstance, void* handle, bool visible)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlImage::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlImage::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIImage*>(handle)->SetVisible(visible);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlImage::SetFileName(void *kodiInstance, void* handle, const char* strFileName, const bool useCache)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlImage::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlImage::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIImage*>(handle)->SetFileName(strFileName, false, useCache);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlImage::SetColorDiffuse(void *kodiInstance, void* handle, uint32_t colorDiffuse)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlImage::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlImage::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIImage*>(handle)->SetColorDiffuse(colorDiffuse);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
