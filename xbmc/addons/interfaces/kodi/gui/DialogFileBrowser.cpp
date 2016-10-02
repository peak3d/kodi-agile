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

#include "DialogFileBrowser.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/DialogFileBrowser.h"

#include "URL.h"
#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "dialogs/GUIDialogFileBrowser.h"
#include "settings/MediaSourceSettings.h"
#include "storage/MediaManager.h"
#include "utils/URIUtils.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIDialogFileBrowser::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->dialogFileBrowser.ShowAndGetDirectory = ShowAndGetDirectory;
  funcTable->toKodi.kodi_gui->dialogFileBrowser.ShowAndGetFile = ShowAndGetFile;
  funcTable->toKodi.kodi_gui->dialogFileBrowser.ShowAndGetFileFromDir = ShowAndGetFileFromDir;
  funcTable->toKodi.kodi_gui->dialogFileBrowser.ShowAndGetFileList = ShowAndGetFileList;
  funcTable->toKodi.kodi_gui->dialogFileBrowser.ShowAndGetSource = ShowAndGetSource;
  funcTable->toKodi.kodi_gui->dialogFileBrowser.ShowAndGetImage = ShowAndGetImage;
  funcTable->toKodi.kodi_gui->dialogFileBrowser.ShowAndGetImageList = ShowAndGetImageList;
  funcTable->toKodi.kodi_gui->dialogFileBrowser.ClearList = ClearList;
}

bool Interface_GUIDialogFileBrowser::ShowAndGetDirectory(void* kodiInstance, const char *shares, const char *heading, char &path, unsigned int &maxStringSize, bool writeOnly)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogFileBrowser::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!shares || !heading)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid handler data (shares='%p', heading='%p')", __FUNCTION__, shares, heading).c_str());

    std::string strPath = &path;

    VECSOURCES vecShares;
    GetVECShares(vecShares, shares, strPath);
    bool bRet = CGUIDialogFileBrowser::ShowAndGetDirectory(vecShares, heading, strPath, writeOnly);
    if (bRet)
      strncpy(&path, strPath.c_str(), maxStringSize);
    maxStringSize = strPath.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogFileBrowser::ShowAndGetFile(void* kodiInstance, const char *shares, const char *mask, const char *heading, char &path, unsigned int &maxStringSize, bool useThumbs, bool useFileDirectories)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogFileBrowser::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!shares || !mask || !heading)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid handler data (shares='%p', mask='%p', heading='%p')", __FUNCTION__, shares, mask, heading).c_str());

    std::string strPath = &path;

    VECSOURCES vecShares;
    GetVECShares(vecShares, shares, strPath);
    bool bRet = CGUIDialogFileBrowser::ShowAndGetFile(vecShares, mask, heading, strPath, useThumbs, useFileDirectories);
    if (bRet)
      strncpy(&path, strPath.c_str(), maxStringSize);
    maxStringSize = strPath.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogFileBrowser::ShowAndGetFileFromDir(void* kodiInstance, const char *directory, const char *mask, const char *heading, char &path, unsigned int &maxStringSize, bool useThumbs, bool useFileDirectories, bool singleList)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogFileBrowser::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!directory || !mask || !heading)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid handler data (directory='%p', mask='%p', heading='%p')", __FUNCTION__, directory, mask, heading).c_str());

    std::string strPath = &path;
    bool bRet = CGUIDialogFileBrowser::ShowAndGetFile(directory, mask, heading, strPath, useThumbs, useFileDirectories, singleList);
    if (bRet)
      strncpy(&path, strPath.c_str(), maxStringSize);
    maxStringSize = strPath.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogFileBrowser::ShowAndGetFileList(void* kodiInstance, const char* shares, const char* mask, const char* heading, char**& path, unsigned int& entries, bool useThumbs, bool useFileDirectories)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogFileBrowser::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!shares || !mask || !heading)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid handler data (shares='%p', mask='%p', heading='%p')", __FUNCTION__, shares, mask, heading).c_str());

    VECSOURCES vecShares;
    GetVECShares(vecShares, shares, "");

    std::vector<std::string> pathsInt;
    bool bRet = CGUIDialogFileBrowser::ShowAndGetFileList(vecShares, mask, heading, pathsInt, useThumbs, useFileDirectories);
    if (bRet)
    {
      entries = pathsInt.size();
      path = (char**)malloc(entries*sizeof(char*));
      for (unsigned int i = 0; i < entries; ++i)
        path[i] = strdup(pathsInt[i].c_str());
    }
    else
      entries = 0;
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogFileBrowser::ShowAndGetSource(void* kodiInstance, char &path, unsigned int &maxStringSize, bool allowNetworkShares, const char *additionalShare, const char *strType)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogFileBrowser::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!strType || !additionalShare)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid handler data (additionalShare='%p', strType='%p')", __FUNCTION__, additionalShare, strType).c_str());

    std::string strPath = &path;

    VECSOURCES vecShares;
    if (additionalShare)
      GetVECShares(vecShares, additionalShare, strPath);
    bool bRet = CGUIDialogFileBrowser::ShowAndGetSource(strPath, allowNetworkShares, &vecShares, strType);
    if (bRet)
      strncpy(&path, strPath.c_str(), maxStringSize);
    maxStringSize = strPath.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogFileBrowser::ShowAndGetImage(void* kodiInstance, const char *shares, const char *heading, char &path, unsigned int &maxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogFileBrowser::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!shares || !heading)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid handler data (shares='%p', heading='%p')", __FUNCTION__, shares, heading).c_str());

    std::string strPath = &path;

    VECSOURCES vecShares;
    GetVECShares(vecShares, shares, strPath);
    bool bRet = CGUIDialogFileBrowser::ShowAndGetImage(vecShares, heading, strPath);
    if (bRet)
      strncpy(&path, strPath.c_str(), maxStringSize);
    maxStringSize = strPath.length();
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_GUIDialogFileBrowser::ShowAndGetImageList(void* kodiInstance, const char* shares, const char* heading, char**& path, unsigned int& entries)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogFileBrowser::%s - invalid data", __FUNCTION__);
    return false;
  }

  try
  {
    if (!shares || !heading)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid handler data (shares='%p', heading='%p')", __FUNCTION__, shares, heading).c_str());

    VECSOURCES vecShares;
    GetVECShares(vecShares, shares, "");

    std::vector<std::string> pathsInt;
    bool bRet = CGUIDialogFileBrowser::ShowAndGetImageList(vecShares, heading, pathsInt);
    if (bRet)
    {
      entries = pathsInt.size();
      path = (char**)malloc(entries*sizeof(char*));
      for (unsigned int i = 0; i < entries; ++i)
        path[i] = strdup(pathsInt[i].c_str());
    }
    else
      entries = 0;
    return bRet;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

void Interface_GUIDialogFileBrowser::ClearList(void* kodiInstance, char**& path, unsigned int entries)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogFileBrowser::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (path)
    {
      for (unsigned int i = 0; i < entries; ++i)
      {
        if (path[i])
          free(path[i]);
      }
      free(path);
    }
    else
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogExtendedProgress::%s - invalid handler data (path='%p')", __FUNCTION__, path).c_str());
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIDialogFileBrowser::GetVECShares(VECSOURCES &vecShares, std::string strShares, std::string strPath)
{
  std::size_t found;
  found = strShares.find("local");
  if (found!=std::string::npos)
    g_mediaManager.GetLocalDrives(vecShares);
  found = strShares.find("network");
  if (found!=std::string::npos)
    g_mediaManager.GetNetworkLocations(vecShares);
  found = strShares.find("removable");
  if (found!=std::string::npos)
    g_mediaManager.GetRemovableDrives(vecShares);
  found = strShares.find("programs");
  if (found!=std::string::npos)
  {
    VECSOURCES *sources = CMediaSourceSettings::GetInstance().GetSources("programs");
    if (sources != nullptr)
      vecShares.insert(vecShares.end(), sources->begin(), sources->end());
  }
  found = strShares.find("files");
  if (found!=std::string::npos)
  {
    VECSOURCES *sources = CMediaSourceSettings::GetInstance().GetSources("files");
    if (sources != nullptr)
      vecShares.insert(vecShares.end(), sources->begin(), sources->end());
  }
  found = strShares.find("music");
  if (found!=std::string::npos)
  {
    VECSOURCES *sources = CMediaSourceSettings::GetInstance().GetSources("music");
    if (sources != nullptr)
      vecShares.insert(vecShares.end(), sources->begin(), sources->end());
  }
  found = strShares.find("video");
  if (found!=std::string::npos)
  {
    VECSOURCES *sources = CMediaSourceSettings::GetInstance().GetSources("video");
    if (sources != nullptr)
      vecShares.insert(vecShares.end(), sources->begin(), sources->end());
  }
  found = strShares.find("pictures");
  if (found!=std::string::npos)
  {
    VECSOURCES *sources = CMediaSourceSettings::GetInstance().GetSources("pictures");
    if (sources != nullptr)
      vecShares.insert(vecShares.end(), sources->begin(), sources->end());
  }

  if (vecShares.empty())
  {
    CMediaSource share;
    std::string basePath = strPath;
    std::string tempPath;
    while (URIUtils::GetParentPath(basePath, tempPath))
      basePath = tempPath;
    share.strPath = basePath;
    // don't include the user details in the share name
    CURL url(share.strPath);
    share.strName = url.GetWithoutUserDetails();
    vecShares.push_back(share);
  }
}

} /* extern "C" */
} /* namespace ADDON */
