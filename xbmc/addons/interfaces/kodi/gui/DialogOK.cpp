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

#include "DialogOK.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/DialogOK.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "dialogs/GUIDialogOK.h"
#include "utils/Variant.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIDialogOK::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->dialogOK.ShowAndGetInputSingleText = ShowAndGetInputSingleText;
  funcTable->toKodi.kodi_gui->dialogOK.ShowAndGetInputLineText = ShowAndGetInputLineText;
}

void Interface_GUIDialogOK::ShowAndGetInputSingleText(void* kodiInstance, const char *heading, const char *text)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogOK::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!heading || !text)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogOK::%s - invalid handler data (heading='%p', text='%p')", __FUNCTION__, heading, text).c_str());

    CGUIDialogOK::ShowAndGetInput(CVariant{heading}, CVariant{text});
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogOK::ShowAndGetInputLineText(void* kodiInstance, const char *heading, const char *line0, const char *line1, const char *line2)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogOK::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!heading || !line0 || !line1 || !line2)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogOK::%s - invalid handler data (heading='%p', line0='%p', line1='%p', line2='%p')", __FUNCTION__, heading, line0, line1, line2).c_str());

    CGUIDialogOK::ShowAndGetInput(CVariant{heading}, CVariant{line0}, CVariant{line1}, CVariant{line2});
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
