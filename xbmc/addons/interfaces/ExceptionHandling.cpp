/*
 *      Copyright (C) 2016 Team KODI
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

#include "ExceptionHandling.h"

#include "addons/AddonDll.h"
#include "addons/AddonManager.h"
#include "addons/kodi-addon-dev-kit/include/kodi/General.h"
#include "dialogs/GUIDialogOK.h"
#include "utils/log.h"

namespace ADDON
{

void Exception::LogStdException(const CAddonDll* addon, const std::exception &e, const char *strFunctionName)
{
  CLog::Log(LOGERROR,
            "Addon - %s - exception '%s' caught while trying to call '%s' on add-on '%s'. Please contact the developer of this add-on: %s",
              TranslateType(addon->Type()).c_str(),
              e.what(),
              strFunctionName,
              addon->Name().c_str(),
              addon->Author().c_str());
}

void Exception::LogErrException(const CAddonDll* addon, int e, const char *name)
{
  CLog::Log(LOGERROR,
            "Addon - %s - exception '%s' caught while trying to call '%s' on add-on '%s'. Please contact the developer of this add-on: %s",
              TranslateType(addon->Type()).c_str(),
              kodi::TranslateAddonStatus((ADDON_STATUS)e).c_str(),
              name,
              addon->Name().c_str(),
              addon->Author().c_str());
}

void Exception::LogUnkException(const CAddonDll* addon, const char *name)
{
  CLog::Log(LOGERROR,
            "Addon - %s - unknown exception caught while trying to call '%s' on add-on '%s'. Please contact the developer of this add-on: %s",
              TranslateType(addon->Type()).c_str(),
              name,
              addon->Name().c_str(),
              addon->Author().c_str());
}

void Exception::LogStdException(const AddonDllPtr addon, const std::exception &e, const char *strFunctionName)
{
  CLog::Log(LOGERROR,
            "Addon - %s - exception '%s' caught while trying to call '%s' on add-on '%s'. Please contact the developer of this add-on: %s",
              TranslateType(addon->Type()).c_str(),
              e.what(),
              strFunctionName,
              addon->Name().c_str(),
              addon->Author().c_str());
}

void Exception::LogErrException(const AddonDllPtr addon, int e, const char *name)
{
  CLog::Log(LOGERROR,
            "Addon - %s - exception '%s' caught while trying to call '%s' on add-on '%s'. Please contact the developer of this add-on: %s",
              TranslateType(addon->Type()).c_str(),
              kodi::TranslateAddonStatus((ADDON_STATUS)e).c_str(),
              name,
              addon->Name().c_str(),
              addon->Author().c_str());
}

void Exception::LogUnkException(const AddonDllPtr addon, const char *name)
{
  CLog::Log(LOGERROR,
            "Addon - %s - unknown exception caught while trying to call '%s' on add-on '%s'. Please contact the developer of this add-on: %s",
              TranslateType(addon->Type()).c_str(),
              name,
              addon->Name().c_str(),
              addon->Author().c_str());
}

void Exception::ShowExceptionErrorDialog(const CAddonDll* addon)
{
  std::string heading = StringUtils::Format("%s: %s", TranslateType(addon->Type(), true).c_str(), addon->Name().c_str());
  CGUIDialogOK::ShowAndGetInput(CVariant{heading}, CVariant{g_localizeStrings.Get(24094)});
}

void Exception::ShowExceptionErrorDialog(AddonDllPtr addon)
{
  std::string heading = StringUtils::Format("%s: %s", TranslateType(addon->Type(), true).c_str(), addon->Name().c_str());
  CGUIDialogOK::ShowAndGetInput(CVariant{heading}, CVariant{g_localizeStrings.Get(24094)});
}

};
