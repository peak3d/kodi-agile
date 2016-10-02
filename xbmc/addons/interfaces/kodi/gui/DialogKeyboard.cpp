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

#include "DialogKeyboard.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/DialogKeyboard.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUIKeyboardFactory.h"
#include "utils/Variant.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIDialogKeyboard::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->dialogKeyboard.ShowAndGetInputWithHead = ShowAndGetInputWithHead;
  funcTable->toKodi.kodi_gui->dialogKeyboard.ShowAndGetInput = ShowAndGetInput;
  funcTable->toKodi.kodi_gui->dialogKeyboard.ShowAndGetNewPasswordWithHead = ShowAndGetNewPasswordWithHead;
  funcTable->toKodi.kodi_gui->dialogKeyboard.ShowAndGetNewPassword = ShowAndGetNewPassword;
  funcTable->toKodi.kodi_gui->dialogKeyboard.ShowAndVerifyNewPasswordWithHead = ShowAndVerifyNewPasswordWithHead;
  funcTable->toKodi.kodi_gui->dialogKeyboard.ShowAndVerifyNewPassword = ShowAndVerifyNewPassword;
  funcTable->toKodi.kodi_gui->dialogKeyboard.ShowAndVerifyPassword = ShowAndVerifyPassword;
  funcTable->toKodi.kodi_gui->dialogKeyboard.ShowAndGetFilter = ShowAndGetFilter;
  funcTable->toKodi.kodi_gui->dialogKeyboard.SendTextToActiveKeyboard = SendTextToActiveKeyboard;
  funcTable->toKodi.kodi_gui->dialogKeyboard.isKeyboardActivated   = isKeyboardActivated;
}

bool Interface_GUIDialogKeyboard::ShowAndGetInputWithHead(void* kodiInstance, char &aTextString, unsigned int &maxStringSize, const char *strHeading, bool allowEmptyResult, bool hiddenInput, unsigned int autoCloseMs)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogKeyboard::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    std::string str = &aTextString;
    bool bRet = CGUIKeyboardFactory::ShowAndGetInput(str, CVariant{strHeading}, allowEmptyResult, hiddenInput, autoCloseMs);
    if (bRet)
      strncpy(&aTextString, str.c_str(), maxStringSize);
    maxStringSize = str.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogKeyboard::ShowAndGetInput(void* kodiInstance, char &aTextString, unsigned int &maxStringSize, bool allowEmptyResult, unsigned int autoCloseMs)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogKeyboard::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    std::string str = &aTextString;
    bool bRet = CGUIKeyboardFactory::ShowAndGetInput(str, allowEmptyResult, autoCloseMs);
    if (bRet)
      strncpy(&aTextString, str.c_str(), maxStringSize);
    maxStringSize = str.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogKeyboard::ShowAndGetNewPasswordWithHead(void* kodiInstance, char &strNewPassword, unsigned int &maxStringSize, const char *strHeading, bool allowEmptyResult, unsigned int autoCloseMs)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogKeyboard::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    std::string str = &strNewPassword;
    bool bRet = CGUIKeyboardFactory::ShowAndGetNewPassword(str, strHeading, allowEmptyResult, autoCloseMs);
    if (bRet)
      strncpy(&strNewPassword, str.c_str(), maxStringSize);
    maxStringSize = str.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogKeyboard::ShowAndGetNewPassword(void* kodiInstance, char &strNewPassword, unsigned int &maxStringSize, unsigned int autoCloseMs)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogKeyboard::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    std::string str = &strNewPassword;
    bool bRet = CGUIKeyboardFactory::ShowAndGetNewPassword(str, autoCloseMs);
    if (bRet)
      strncpy(&strNewPassword, str.c_str(), maxStringSize);
    maxStringSize = str.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogKeyboard::ShowAndVerifyNewPasswordWithHead(void* kodiInstance, char &strNewPassword, unsigned int &maxStringSize, const char *strHeading, bool allowEmptyResult, unsigned int autoCloseMs)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogKeyboard::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    std::string str = &strNewPassword;
    bool bRet = CGUIKeyboardFactory::ShowAndVerifyNewPassword(str, strHeading, allowEmptyResult, autoCloseMs);
    if (bRet)
      strncpy(&strNewPassword, str.c_str(), maxStringSize);
    maxStringSize = str.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogKeyboard::ShowAndVerifyNewPassword(void* kodiInstance, char &strNewPassword, unsigned int &maxStringSize, unsigned int autoCloseMs)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogKeyboard::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    std::string str = &strNewPassword;
    bool bRet = CGUIKeyboardFactory::ShowAndVerifyNewPassword(str, autoCloseMs);
    if (bRet)
      strncpy(&strNewPassword, str.c_str(), maxStringSize);
    maxStringSize = str.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

int Interface_GUIDialogKeyboard::ShowAndVerifyPassword(void* kodiInstance, char &strPassword, unsigned int &maxStringSize, const char *strHeading, int iRetries, unsigned int autoCloseMs)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogKeyboard::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    std::string str = &strPassword;
    int iRet = CGUIKeyboardFactory::ShowAndVerifyPassword(str, strHeading, iRetries, autoCloseMs);
    if (iRet)
      strncpy(&strPassword, str.c_str(), maxStringSize);
    maxStringSize = str.length();
    return iRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

bool Interface_GUIDialogKeyboard::ShowAndGetFilter(void* kodiInstance, char &aTextString, unsigned int &maxStringSize, bool searching, unsigned int autoCloseMs)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogKeyboard::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    std::string str = &aTextString;
    bool bRet = CGUIKeyboardFactory::ShowAndGetFilter(str, searching, autoCloseMs);
    if (bRet)
      strncpy(&aTextString, str.c_str(), maxStringSize);
    maxStringSize = str.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogKeyboard::SendTextToActiveKeyboard(void* kodiInstance, const char *aTextString, bool closeKeyboard)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogKeyboard::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    return CGUIKeyboardFactory::SendTextToActiveKeyboard(aTextString, closeKeyboard);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogKeyboard::isKeyboardActivated(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogKeyboard::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    return CGUIKeyboardFactory::isKeyboardActivated();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

} /* extern "C" */
} /* namespace ADDON */
