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

#include "ControlEdit.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlEdit.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUIEditControl.h"
#include "guilib/GUIWindowManager.h"
#include "utils/log.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIControlEdit::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlEdit.SetVisible = SetVisible;
  funcTable->toKodi.kodi_gui->controlEdit.SetEnabled = SetEnabled;
  funcTable->toKodi.kodi_gui->controlEdit.SetLabel = SetLabel;
  funcTable->toKodi.kodi_gui->controlEdit.GetLabel = GetLabel;
  funcTable->toKodi.kodi_gui->controlEdit.SetText = SetText;
  funcTable->toKodi.kodi_gui->controlEdit.GetText = GetText;
  funcTable->toKodi.kodi_gui->controlEdit.SetCursorPosition = SetCursorPosition;
  funcTable->toKodi.kodi_gui->controlEdit.GetCursorPosition = GetCursorPosition;
  funcTable->toKodi.kodi_gui->controlEdit.SetInputType = SetInputType;
}

void Interface_GUIControlEdit::SetVisible(void* kodiInstance, void* handle, bool yesNo)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlEdit::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIEditControl*>(handle)->SetVisible(yesNo);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlEdit::SetEnabled(void* kodiInstance, void* handle, bool yesNo)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlEdit::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIEditControl*>(handle)->SetEnabled(yesNo);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlEdit::SetLabel(void* kodiInstance, void* handle, const char *label)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlEdit::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIEditControl*>(handle)->SetLabel(label);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlEdit::GetLabel(void* kodiInstance, void* handle, char &label, unsigned int &maxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlEdit::%s - invalid handler data", __FUNCTION__).c_str());

    std::string text = static_cast<CGUIEditControl*>(handle)->GetLabel();
    strncpy(&label, text.c_str(), maxStringSize);
    maxStringSize = text.length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlEdit::SetText(void* kodiInstance, void* handle, const char* text)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlEdit::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIEditControl*>(handle)->SetLabel2(text);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlEdit::GetText(void* kodiInstance, void* handle, char& text, unsigned int &maxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlEdit::%s - invalid handler data", __FUNCTION__).c_str());

    std::string textTmp = static_cast<CGUIEditControl*>(handle)->GetLabel2();
    strncpy(&text, textTmp.c_str(), maxStringSize);
    maxStringSize = textTmp.length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

unsigned int Interface_GUIControlEdit::GetCursorPosition(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return 0;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlEdit::%s - invalid handler data", __FUNCTION__).c_str());

    return static_cast<CGUIEditControl*>(handle)->GetCursorPosition();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0;
}

void Interface_GUIControlEdit::SetCursorPosition(void* kodiInstance, void* handle, unsigned int position)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlEdit::%s - invalid handler data", __FUNCTION__).c_str());

    static_cast<CGUIEditControl*>(handle)->SetCursorPosition(position);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlEdit::SetInputType(void* kodiInstance, void* handle, int type, const char *heading)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIGeneral::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlEdit::%s - invalid handler data", __FUNCTION__).c_str());

    CGUIEditControl::INPUT_TYPE kodiType;
    switch (type)
    {
      case ADDON_INPUT_TYPE_TEXT:
        kodiType = CGUIEditControl::INPUT_TYPE_TEXT;
        break;
      case ADDON_INPUT_TYPE_NUMBER:
        kodiType = CGUIEditControl::INPUT_TYPE_NUMBER;
        break;
      case ADDON_INPUT_TYPE_SECONDS:
        kodiType = CGUIEditControl::INPUT_TYPE_SECONDS;
        break;
      case ADDON_INPUT_TYPE_TIME:
        kodiType = CGUIEditControl::INPUT_TYPE_TIME;
        break;
      case ADDON_INPUT_TYPE_DATE:
        kodiType = CGUIEditControl::INPUT_TYPE_DATE;
        break;
      case ADDON_INPUT_TYPE_IPADDRESS:
        kodiType = CGUIEditControl::INPUT_TYPE_IPADDRESS;
        break;
      case ADDON_INPUT_TYPE_PASSWORD:
        kodiType = CGUIEditControl::INPUT_TYPE_PASSWORD;
        break;
      case ADDON_INPUT_TYPE_PASSWORD_MD5:
        kodiType = CGUIEditControl::INPUT_TYPE_PASSWORD_MD5;
        break;
      case ADDON_INPUT_TYPE_SEARCH:
        kodiType = CGUIEditControl::INPUT_TYPE_SEARCH;
        break;
      case ADDON_INPUT_TYPE_FILTER:
        kodiType = CGUIEditControl::INPUT_TYPE_FILTER;
        break;
      case ADDON_INPUT_TYPE_READONLY:
      default:
        kodiType = CGUIEditControl::INPUT_TYPE_PASSWORD_NUMBER_VERIFY_NEW;
    }

    static_cast<CGUIEditControl*>(handle)->SetInputType(kodiType, heading);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
