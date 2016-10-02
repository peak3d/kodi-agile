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

#include "addons/interfaces/kodi/gui/ListItem.h"
#include "General.h"

#include "FileItem.h"
#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ListItem.h"
#include "guilib/GUIListItem.h"
#include "guilib/GUIWindowManager.h"
#include "music/tags/MusicInfoTag.h"
#include "pictures/PictureInfoTag.h"
#include "video/VideoInfoTag.h"
#include "utils/log.h"
#include "utils/StringUtils.h"
#include "utils/Variant.h"

using namespace kodi; // addon-dev-kit namespace
using namespace kodi::gui; // addon-dev-kit namespace

namespace ADDON
{
extern "C"
{

void Interface_GUIListItem::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->listItem.Create = Create;
  funcTable->toKodi.kodi_gui->listItem.Destroy = Destroy;
  funcTable->toKodi.kodi_gui->listItem.GetLabel = GetLabel;
  funcTable->toKodi.kodi_gui->listItem.SetLabel = SetLabel;
  funcTable->toKodi.kodi_gui->listItem.GetLabel2 = GetLabel2;
  funcTable->toKodi.kodi_gui->listItem.SetLabel2 = SetLabel2;
  funcTable->toKodi.kodi_gui->listItem.GetIconImage = GetIconImage;
  funcTable->toKodi.kodi_gui->listItem.SetIconImage = SetIconImage;
  funcTable->toKodi.kodi_gui->listItem.GetOverlayImage = GetOverlayImage;
  funcTable->toKodi.kodi_gui->listItem.SetOverlayImage = SetOverlayImage;
  funcTable->toKodi.kodi_gui->listItem.SetThumbnailImage = SetThumbnailImage;
  funcTable->toKodi.kodi_gui->listItem.SetArt = SetArt;
  funcTable->toKodi.kodi_gui->listItem.GetArt = GetArt;
  funcTable->toKodi.kodi_gui->listItem.SetArtFallback = SetArtFallback;
  funcTable->toKodi.kodi_gui->listItem.HasArt = HasArt;
  funcTable->toKodi.kodi_gui->listItem.Select = Select;
  funcTable->toKodi.kodi_gui->listItem.IsSelected = IsSelected;
  funcTable->toKodi.kodi_gui->listItem.HasIcon = HasIcon;
  funcTable->toKodi.kodi_gui->listItem.HasOverlay = HasOverlay;
  funcTable->toKodi.kodi_gui->listItem.IsFileItem = IsFileItem;
  funcTable->toKodi.kodi_gui->listItem.IsFolder = IsFolder;
  funcTable->toKodi.kodi_gui->listItem.SetProperty = SetProperty;
  funcTable->toKodi.kodi_gui->listItem.GetProperty = GetProperty;
  funcTable->toKodi.kodi_gui->listItem.ClearProperty = ClearProperty;
  funcTable->toKodi.kodi_gui->listItem.ClearProperties = ClearProperties;
  funcTable->toKodi.kodi_gui->listItem.HasProperties = HasProperties;
  funcTable->toKodi.kodi_gui->listItem.HasProperty = HasProperty;
  funcTable->toKodi.kodi_gui->listItem.SetPath = SetPath;
  funcTable->toKodi.kodi_gui->listItem.GetPath = GetPath;
  funcTable->toKodi.kodi_gui->listItem.GetDuration = GetDuration;
  funcTable->toKodi.kodi_gui->listItem.SetSubtitles = SetSubtitles;
  funcTable->toKodi.kodi_gui->listItem.SetMimeType = SetMimeType;
  funcTable->toKodi.kodi_gui->listItem.SetContentLookup = SetContentLookup;
  funcTable->toKodi.kodi_gui->listItem.AddContextMenuItems = AddContextMenuItems;
  funcTable->toKodi.kodi_gui->listItem.AddStreamInfo = AddStreamInfo;
}

void* Interface_GUIListItem::Create(void* kodiInstance, const char *label, const char *label2, const char *iconImage, const char *thumbnailImage, const char *path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return nullptr;
  }

  try
  {
    // create CFileItem
    CFileItem *pItem = new CFileItem();
    if (!pItem)
      return nullptr;

    if (label)
      pItem->SetLabel(label);
    if (label2)
      pItem->SetLabel2(label2);
    if (iconImage)
      pItem->SetIconImage(iconImage);
    if (thumbnailImage)
      pItem->SetArt("thumb", thumbnailImage);
    if (path)
      pItem->SetPath(path);

    return pItem;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void Interface_GUIListItem::Destroy(void* kodiInstance, void* handle)
{
  // @note: Delete of CFileItem brings crash, need to check about the in related
  // window and no memleak present to confirm.
  // In old version was the same way, only the Destroy passed here to allow
  // changes and fixes without API level changes.
}

void Interface_GUIListItem::GetLabel(void* kodiInstance, void* handle, char &label, unsigned int &iMaxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    strncpy(&label, static_cast<CFileItem *>(handle)->GetLabel().c_str(), iMaxStringSize);
    iMaxStringSize = static_cast<CFileItem *>(handle)->GetLabel().length();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::SetLabel(void* kodiInstance, void* handle, const char *label)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->SetLabel(label);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::GetLabel2(void* kodiInstance, void* handle, char &label, unsigned int &iMaxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    strncpy(&label, static_cast<CFileItem *>(handle)->GetLabel2().c_str(), iMaxStringSize);
    iMaxStringSize = static_cast<CFileItem *>(handle)->GetLabel2().length();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::SetLabel2(void* kodiInstance, void* handle, const char *label)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->SetLabel2(label);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::GetIconImage(void* kodiInstance, void* handle, char &image, unsigned int &iMaxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    strncpy(&image, static_cast<CFileItem *>(handle)->GetIconImage().c_str(), iMaxStringSize);
    iMaxStringSize = static_cast<CFileItem *>(handle)->GetIconImage().length();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::SetIconImage(void* kodiInstance, void* handle, const char *image)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->SetIconImage(image);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::GetOverlayImage(void* kodiInstance, void* handle, char &image, unsigned int &iMaxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    strncpy(&image, static_cast<CFileItem *>(handle)->GetOverlayImage().c_str(), iMaxStringSize);
    iMaxStringSize = static_cast<CFileItem *>(handle)->GetOverlayImage().length();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::SetOverlayImage(void* kodiInstance, void* handle, unsigned int image, bool bOnOff /* = false*/)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->SetOverlayImage((CGUIListItem::GUIIconOverlay)image, bOnOff);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::SetThumbnailImage(void* kodiInstance, void* handle, const char *image)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->SetArt("thumb", image);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::SetArt(void* kodiInstance, void* handle, const char *type, const char *url)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->SetArt(type, url);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

char* Interface_GUIListItem::GetArt(void* kodiInstance, void* handle, const char *type)
{
  char* buffer = nullptr;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return buffer;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    buffer = strdup(static_cast<CFileItem *>(handle)->GetArt(type).c_str());
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
  return buffer;
}

void Interface_GUIListItem::SetArtFallback(void* kodiInstance, void* handle, const char *from, const char *to)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->SetArtFallback(from, to);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

bool Interface_GUIListItem::HasArt(void* kodiInstance, void* handle, const char *type)
{
  bool ret = false;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return ret;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    ret = static_cast<CFileItem *>(handle)->HasArt(type);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
  return ret;
}

void Interface_GUIListItem::Select(void* kodiInstance, void* handle, bool bOnOff)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->Select(bOnOff);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

bool Interface_GUIListItem::IsSelected(void* kodiInstance, void* handle)
{
  bool ret = false;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return ret;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    ret = static_cast<CFileItem *>(handle)->IsSelected();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();

  return ret;
}

bool Interface_GUIListItem::HasIcon(void* kodiInstance, void* handle)
{
  bool ret = false;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return ret;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    ret = static_cast<CFileItem *>(handle)->HasIcon();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();

  return ret;
}

bool Interface_GUIListItem::HasOverlay(void* kodiInstance, void* handle)
{
  bool ret = false;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return ret;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    ret = static_cast<CFileItem *>(handle)->HasOverlay();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();

  return ret;
}

bool Interface_GUIListItem::IsFileItem(void* kodiInstance, void* handle)
{
  bool ret = false;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return ret;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    ret = static_cast<CFileItem *>(handle)->IsFileItem();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
  return ret;
}

bool Interface_GUIListItem::IsFolder(void* kodiInstance, void* handle)
{
  bool ret = false;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return ret;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    ret = static_cast<CFileItem *>(handle)->m_bIsFolder;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
  return ret;
}

void Interface_GUIListItem::SetProperty(void* kodiInstance, void* handle, const char *key, const char *value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->SetProperty(key, value);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::GetProperty(void* kodiInstance, void* handle, const char *key, char &property, unsigned int &iMaxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    std::string lowerKey = key;
    StringUtils::ToLower(lowerKey);

    Interface_GUIGeneral::Lock();
    std::string value = static_cast<CFileItem *>(handle)->GetProperty(lowerKey).asString();
    Interface_GUIGeneral::Unlock();

    strncpy(&property, value.c_str(), iMaxStringSize);
    iMaxStringSize = value.length();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIListItem::ClearProperty(void* kodiInstance, void* handle, const char *key)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->ClearProperty(key);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::ClearProperties(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->ClearProperties();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

bool Interface_GUIListItem::HasProperties(void* kodiInstance, void* handle)
{
  bool ret = false;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return ret;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    ret = static_cast<CFileItem *>(handle)->HasProperties();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
  return ret;
}

bool Interface_GUIListItem::HasProperty(void* kodiInstance, void* handle, const char *key)
{
  bool ret = false;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return ret;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    ret = static_cast<CFileItem *>(handle)->HasProperty(key);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
  return ret;
}

void Interface_GUIListItem::SetPath(void* kodiInstance, void* handle, const char *path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->SetPath(path);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

char* Interface_GUIListItem::GetPath(void* kodiInstance, void* handle)
{
  char* buffer = nullptr;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return buffer;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    buffer = strdup(static_cast<CFileItem *>(handle)->GetPath().c_str());
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
  return buffer;
}

int Interface_GUIListItem::GetDuration(void* kodiInstance, void* handle)
{
  int duration = 0;

  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return duration;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    if (static_cast<CFileItem *>(handle)->LoadMusicTag())
      duration = static_cast<CFileItem *>(handle)->GetMusicInfoTag()->GetDuration();
    else if (static_cast<CFileItem *>(handle)->HasVideoInfoTag())
      duration = static_cast<CFileItem *>(handle)->GetVideoInfoTag()->GetDuration();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
  return duration;
}

void Interface_GUIListItem::SetSubtitles(void* kodiInstance, void* handle, const char** streams, unsigned int entries)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    for (unsigned int i = 0; i < entries; ++i)
    {
      std::string property = StringUtils::Format("subtitle:%u", i+1);
      static_cast<CFileItem *>(handle)->SetProperty(property, streams[i]);
    }
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::SetMimeType(void* kodiInstance, void* handle, const char* mimetype)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->SetMimeType(mimetype);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::SetContentLookup(void* kodiInstance, void* handle, bool enable)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();
  try
  {
    static_cast<CFileItem *>(handle)->SetContentLookup(enable);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::AddContextMenuItems(void* kodiInstance, void* handle, const char** menus[2], unsigned int entries, bool replaceItems)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (handle == nullptr)
      throw std::logic_error(StringUtils::Format("Interface_GUIListItem - %s: %s/%s - No handle",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());

    if (menus == nullptr)
      throw std::logic_error(StringUtils::Format("Interface_GUIListItem - %s: %s/%s - Must pass a list of strings. List self is empty.",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());

    for (unsigned int i = 0; i < entries; ++i)
    {
      if (menus[i][0] == nullptr || menus[i][1] == nullptr)
        throw std::logic_error(StringUtils::Format("Interface_GUIListItem - %s: %s/%s - Must pass in a list of tuples of pairs of strings. One entry in the list is empty.",
                  __FUNCTION__,
                  TranslateType(addon->Type()).c_str(),
                  addon->Name().c_str()).c_str());

      std::string uText = menus[i][0];
      std::string uAction = menus[i][1];

      Interface_GUIGeneral::Lock();
      std::string property = StringUtils::Format("contextmenulabel(%i)", i);
      static_cast<CFileItem *>(handle)->SetProperty(property, uText);

      property = StringUtils::Format("contextmenuaction(%i)", i);
      static_cast<CFileItem *>(handle)->SetProperty(property, uAction);
      Interface_GUIGeneral::Unlock();
    }

    // set our replaceItems status
    if (replaceItems)
      static_cast<CFileItem *>(handle)->SetProperty("pluginreplacecontextitems", replaceItems);
    // end addContextMenuItems
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIListItem::AddStreamInfo(void* kodiInstance, void* handle, const char* cType, const char** dictionary[2], unsigned int entries)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();

  try
  {
    if (strcmpi(cType, "video") == 0)
    {
      CStreamDetailVideo* video = new CStreamDetailVideo;
      for (unsigned int i = 0; i < entries; ++i)
      {
        const std::string key = dictionary[i][0];
        const std::string value = dictionary[i][1];

        if (key == "codec")
          video->m_strCodec = value;
        else if (key == "aspect")
          video->m_fAspect = (float)atof(value.c_str());
        else if (key == "width")
          video->m_iWidth = strtol(value.c_str(), NULL, 10);
        else if (key == "height")
          video->m_iHeight = strtol(value.c_str(), NULL, 10);
        else if (key == "duration")
          video->m_iDuration = strtol(value.c_str(), NULL, 10);
        else if (key == "stereomode")
          video->m_strStereoMode = value;
      }
      static_cast<CFileItem *>(handle)->GetVideoInfoTag()->m_streamDetails.AddStream(video);
    }
    else if (strcmpi(cType, "audio") == 0)
    {
      CStreamDetailAudio* audio = new CStreamDetailAudio;
      for (unsigned int i = 0; i < entries; ++i)
      {
        const std::string key = dictionary[i][0];
        const std::string value = dictionary[i][1];

        if (key == "codec")
          audio->m_strCodec = value;
        else if (key == "language")
          audio->m_strLanguage = value;
        else if (key == "channels")
          audio->m_iChannels = strtol(value.c_str(), NULL, 10);
      }
      static_cast<CFileItem *>(handle)->GetVideoInfoTag()->m_streamDetails.AddStream(audio);
    }
    else if (strcmpi(cType, "subtitle") == 0)
    {
      CStreamDetailSubtitle* subtitle = new CStreamDetailSubtitle;
      for (unsigned int i = 0; i < entries; ++i)
      {
        const std::string key = dictionary[i][0];
        const std::string value = dictionary[i][1];

        if (key == "language")
          subtitle->m_strLanguage = value;
      }
      static_cast<CFileItem *>(handle)->GetVideoInfoTag()->m_streamDetails.AddStream(subtitle);
    }
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::SetMusicInfo(void* kodiInstance, void* handle, unsigned int type, void* data, unsigned int entries)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();

  try
  {
    CFileItem* fileItem = static_cast<CFileItem *>(handle);
    switch (type)
    {
      case ADDON_MusicInfoTag____url_______________________STRING:
        fileItem->GetMusicInfoTag()->SetURL((const char*)data);
        break;
      case ADDON_MusicInfoTag____title_____________________STRING:
        fileItem->GetMusicInfoTag()->SetTitle((const char*)data);
        break;
      case ADDON_MusicInfoTag____artist____________________STRING_LIST:
      {
        std::vector<std::string> artists;
        for (unsigned int i = 0; i < entries; ++i)
          artists.push_back(((const char**)data)[i]);
        fileItem->GetMusicInfoTag()->SetArtist(artists);
        break;
      }
      case ADDON_MusicInfoTag____album_____________________STRING:
        fileItem->GetMusicInfoTag()->SetAlbum((const char*)data);
        break;
      case ADDON_MusicInfoTag____albumartist_______________STRING_LIST:
      {
        std::vector<std::string> artists;
        for (unsigned int i = 0; i < entries; ++i)
          artists.push_back(((const char**)data)[i]);
        fileItem->GetMusicInfoTag()->SetAlbumArtist(artists);
        break;
      }
      case ADDON_MusicInfoTag____genre_____________________STRING_LIST:
      {
        std::vector<std::string> genre;
        for (unsigned int i = 0; i < entries; ++i)
          genre.push_back(((const char**)data)[i]);
        fileItem->GetMusicInfoTag()->SetGenre(genre);
        break;
      }
      case ADDON_MusicInfoTag____duration__________________INT:
        fileItem->GetMusicInfoTag()->SetDuration(*(int*)data);
        break;
      case ADDON_MusicInfoTag____track_number______________INT:
        fileItem->GetMusicInfoTag()->SetTrackNumber(*(int*)data);
        break;
      case ADDON_MusicInfoTag____disc_number_______________INT:
        fileItem->GetMusicInfoTag()->SetDiscNumber(*(int*)data);
        break;
      case ADDON_MusicInfoTag____loaded____________________BOOL:
        fileItem->GetMusicInfoTag()->SetLoaded(*(bool*)data);
        break;
      case ADDON_MusicInfoTag____year______________________UINT:
        fileItem->GetMusicInfoTag()->SetYear(*(unsigned int*)data);
        break;
      case ADDON_MusicInfoTag____musicbrainztrackid________STRING:
        fileItem->GetMusicInfoTag()->SetMusicBrainzTrackID((const char*)data);
        break;
      case ADDON_MusicInfoTag____musicbrainzartistid_______STRING_LIST:
      {
        std::vector<std::string> musicbrainzartistid;
        for (unsigned int i = 0; i < entries; ++i)
          musicbrainzartistid.push_back(((const char**)data)[i]);
        fileItem->GetMusicInfoTag()->SetMusicBrainzArtistID(musicbrainzartistid);
        break;
      }
      case ADDON_MusicInfoTag____musicbrainzalbumid________STRING:
        fileItem->GetMusicInfoTag()->SetMusicBrainzAlbumID((const char*)data);
        break;
      case ADDON_MusicInfoTag____musicbrainzalbumartistid__STRING_LIST:
      {
        std::vector<std::string> musicbrainzalbumartistid;
        for (unsigned int i = 0; i < entries; ++i)
          musicbrainzalbumartistid.push_back(((const char**)data)[i]);
        fileItem->GetMusicInfoTag()->SetMusicBrainzAlbumArtistID(musicbrainzalbumartistid);
        break;
      }
      case ADDON_MusicInfoTag____mediatype_________________STRING:
      {
        if (CMediaTypes::IsValidMediaType((const char*)data))
          fileItem->GetMusicInfoTag()->SetType((const char*)data);
        else
          CLog::Log(LOGWARNING, "Invalid media type \"%s\"", (const char*)data);
        break;
      }
      case ADDON_MusicInfoTag____comment___________________STRING:
        fileItem->GetMusicInfoTag()->SetComment((const char*)data);
        break;
      case ADDON_MusicInfoTag____mood______________________STRING:
        fileItem->GetMusicInfoTag()->SetMood((const char*)data);
        break;
      case ADDON_MusicInfoTag____rating____________________FLOAT:
        fileItem->GetMusicInfoTag()->SetRating(*(float*)data);
        break;
      case ADDON_MusicInfoTag____userrating________________INT:
        fileItem->GetMusicInfoTag()->SetUserrating(*(int*)data);
        break;
      case ADDON_MusicInfoTag____votes_____________________INT:
        fileItem->GetMusicInfoTag()->SetVotes(*(int*)data);
        break;
      case ADDON_MusicInfoTag____playcount_________________INT:
        fileItem->GetMusicInfoTag()->SetPlayCount(*(int*)data);
        break;
      case ADDON_MusicInfoTag____lastplayed________________STRING_DATE_TIME:
        if (strlen((const char*)data) == 10)
        {
          SYSTEMTIME time;
          CDateTime date;
          date.SetFromDBDateTime((const char*)data);
          date.GetAsSystemTime(time);
          fileItem->GetMusicInfoTag()->SetLastPlayed(time);
        }
        break;
      case ADDON_MusicInfoTag____dateadded_________________STRING_DATE_TIME:
        if (strlen((const char*)data) == 10)
        {
          SYSTEMTIME time;
          CDateTime date;
          date.SetFromDBDateTime((const char*)data);
          date.GetAsSystemTime(time);
          fileItem->GetMusicInfoTag()->SetDateAdded(time);
        }
        break;
      case ADDON_MusicInfoTag____lyrics____________________STRING:
        fileItem->GetMusicInfoTag()->SetLyrics((const char*)data);
        break;
      case ADDON_MusicInfoTag____albumid___________________INT:
        fileItem->GetMusicInfoTag()->SetAlbumId(*(int*)data);
        break;
      case ADDON_MusicInfoTag____compilation_______________BOOL:
        fileItem->GetMusicInfoTag()->SetCompilation(*(bool*)data);
        break;
      case ADDON_MusicInfoTag____releasedate_______________STRING_DATE_TIME:
        if (strlen((const char*)data) == 10)
        {
          SYSTEMTIME time;
          CDateTime date;
          date.SetFromDBDateTime((const char*)data);
          date.GetAsSystemTime(time);
          fileItem->GetMusicInfoTag()->SetReleaseDate(time);
        }
        break;
      case ADDON_MusicInfoTag____albumreleasetype__________UINT:
        fileItem->GetMusicInfoTag()->SetAlbumReleaseType(*(CAlbum::ReleaseType*)data);
        break;
      default:
        break;
    }
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::SetVideoInfo(void* kodiInstance, void* handle, unsigned int type, void*data, unsigned int entries)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();

  try
  {
    CFileItem* fileItem = static_cast<CFileItem *>(handle);
    switch (type)
    {
    case ADDON_VideoInfoTag____director__________________STRING_LIST:
    {
      std::vector<std::string> list;
      for (unsigned int i = 0; i < entries; ++i)
      {
        const std::string value = ((const char**)data)[i];
        list.push_back(value);
      }
      fileItem->GetVideoInfoTag()->SetDirector(list);
      break;
    }
    case ADDON_VideoInfoTag____writing_credits___________STRING_LIST:
    {
      std::vector<std::string> list;
      for (unsigned int i = 0; i < entries; ++i)
      {
        const std::string value = ((const char**)data)[i];
        list.push_back(value);
      }
      fileItem->GetVideoInfoTag()->SetWritingCredits(list);
      break;
    }
    case ADDON_VideoInfoTag____genre_____________________STRING_LIST:
    {
      std::vector<std::string> list;
      for (unsigned int i = 0; i < entries; ++i)
      {
        const std::string value = ((const char**)data)[i];
        list.push_back(value);
      }
      fileItem->GetVideoInfoTag()->SetGenre(list);
      break;
    }
    case ADDON_VideoInfoTag____country___________________STRING_LIST:
    {
      std::vector<std::string> list;
      for (unsigned int i = 0; i < entries; ++i)
      {
        const std::string value = ((const char**)data)[i];
        list.push_back(value);
      }
      fileItem->GetVideoInfoTag()->SetCountry(list);
      break;
    }
    case ADDON_VideoInfoTag____tagline___________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetTagLine(value);
      break;
    }
    case ADDON_VideoInfoTag____plot_outline______________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetPlotOutline(value);
      break;
    }
    case ADDON_VideoInfoTag____plot______________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetPlot(value);
      break;
    }
    case ADDON_VideoInfoTag____title_____________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetTitle(value);
      break;
    }
    case ADDON_VideoInfoTag____votes_____________________INT:
    {
      fileItem->GetVideoInfoTag()->SetVotes(*(int*)data);
      break;
    }
    case ADDON_VideoInfoTag____studio____________________STRING_LIST:
    {
      std::vector<std::string> list;
      for (unsigned int i = 0; i < entries; ++i)
      {
        const std::string value = ((const char**)data)[i];
        list.push_back(value);
      }
      fileItem->GetVideoInfoTag()->SetStudio(list);
      break;
    }
    case ADDON_VideoInfoTag____trailer___________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetTitle(value);
      break;
    }
    case ADDON_VideoInfoTag____cast______________________DATA_LIST:
    {
      fileItem->GetVideoInfoTag()->m_cast.clear();
      for (unsigned int i = 0; i < entries; ++i)
      {
        SActorInfo info;
        info.strName  = ((ADDON_VideoInfoTag__cast__DATA_STRUCT*)data)[i].name;
        info.strRole  = ((ADDON_VideoInfoTag__cast__DATA_STRUCT*)data)[i].role;
        info.thumb = ((ADDON_VideoInfoTag__cast__DATA_STRUCT*)data)[i].thumbnail;
        info.order = ((ADDON_VideoInfoTag__cast__DATA_STRUCT*)data)[i].order;
        fileItem->GetVideoInfoTag()->m_cast.push_back(info);
      }
      break;
    }
    case ADDON_VideoInfoTag____set_______________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetSet(value);
      break;
    }
    case ADDON_VideoInfoTag____setid_____________________INT:
    {
      fileItem->GetVideoInfoTag()->m_iSetId = *(int*)data;
      break;
    }
    case ADDON_VideoInfoTag____setoverview_______________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetSetOverview(value);
      break;
    }
    case ADDON_VideoInfoTag____tag_______________________STRING_LIST:
    {
      std::vector<std::string> list;
      for (unsigned int i = 0; i < entries; ++i)
      {
        const std::string value = ((const char**)data)[i];
        list.push_back(value);
      }
      fileItem->GetVideoInfoTag()->SetTags(list);
      break;
    }
    case ADDON_VideoInfoTag____duration__________________UINT:
      fileItem->GetVideoInfoTag()->m_duration = (*(unsigned int*)data);
      break;
    case ADDON_VideoInfoTag____file______________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetFile(value);
      break;
    }
    case ADDON_VideoInfoTag____path______________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetPath(value);
      break;
    }
    case ADDON_VideoInfoTag____imdbnumber________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetUniqueID(value);
      break;
    }
    case ADDON_VideoInfoTag____mpaa_rating_______________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetMPAARating(value);
      break;
    }
    case ADDON_VideoInfoTag____filename_and_path_________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetFileNameAndPath(value);
      break;
    }
    case ADDON_VideoInfoTag____original_title____________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetOriginalTitle(value);
      break;
    }
    case ADDON_VideoInfoTag____sorttitle_________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetSortTitle(value);
      break;
    }
    case ADDON_VideoInfoTag____episode_guide_____________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetEpisodeGuide(value);
      break;
    }
    case ADDON_VideoInfoTag____premiered_________________STRING_DATE:
    {
      CDateTime date;
      date.SetFromDBDate((const char*)data);
      fileItem->GetVideoInfoTag()->m_premiered = date;
      break;
    }
    case ADDON_VideoInfoTag____status____________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetStatus(value);
      break;
    }
    case ADDON_VideoInfoTag____production_code___________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetProductionCode(value);
    }
    case ADDON_VideoInfoTag____first_aired_______________STRING_DATE:
    {
      CDateTime date;
      date.SetFromDBDate((const char*)data);
      fileItem->GetVideoInfoTag()->m_firstAired = date;
      break;
    }
    case ADDON_VideoInfoTag____show_title________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetShowTitle(value);
      break;
    }
    case ADDON_VideoInfoTag____album_____________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetAlbum(value);
      break;
    }
    case ADDON_VideoInfoTag____artist____________________STRING_LIST:
    {
      std::vector<std::string> list;
      for (unsigned int i = 0; i < entries; ++i)
      {
        const std::string value = ((const char**)data)[i];
        list.push_back(value);
      }
      fileItem->GetVideoInfoTag()->SetArtist(list);
      break;
    }
    case ADDON_VideoInfoTag____playcount_________________INT:
      fileItem->GetVideoInfoTag()->m_playCount = *(int*)data;
      break;
    case ADDON_VideoInfoTag____lastplayed________________STRING_DATE_TIME:
    {
      CDateTime date;
      date.SetFromDBDateTime((const char*)data);
      fileItem->GetVideoInfoTag()->m_lastPlayed = date;
      break;
    }
    case ADDON_VideoInfoTag____top250____________________INT:
      fileItem->GetVideoInfoTag()->m_iTop250 = *(int*)data;
      break;
    case ADDON_VideoInfoTag____dbid______________________INT:
      fileItem->GetVideoInfoTag()->m_iDbId = *(int*)data;
      break;
    case ADDON_VideoInfoTag____season____________________INT:
      fileItem->GetVideoInfoTag()->m_iSeason = *(int*)data;
      break;
    case ADDON_VideoInfoTag____episode___________________INT:
      fileItem->GetVideoInfoTag()->m_iEpisode = *(int*)data;
      break;
    case ADDON_VideoInfoTag____unique_id_________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->SetUniqueID(value);
      break;
    }
    case ADDON_VideoInfoTag____rating____________________FLOAT:
      fileItem->GetVideoInfoTag()->SetRating(*(float*)data);
      break;
    case ADDON_VideoInfoTag____user_rating_______________INT:
      fileItem->GetVideoInfoTag()->SetUserrating(*(int*)data);
      break;
    case ADDON_VideoInfoTag____db_id_____________________INT:
      fileItem->GetVideoInfoTag()->m_iDbId = *(int*)data;
      break;
    case ADDON_VideoInfoTag____file_id___________________INT:
      fileItem->GetVideoInfoTag()->m_iFileId = *(int*)data;
      break;
    case ADDON_VideoInfoTag____track_____________________INT:
      fileItem->GetVideoInfoTag()->m_iTrack = *(int*)data;
      break;
    case ADDON_VideoInfoTag____show_link_________________STRING_LIST:
    {
      std::vector<std::string> list;
      for (unsigned int i = 0; i < entries; ++i)
      {
        const std::string value = ((const char**)data)[i];
        list.push_back(value);
      }
      fileItem->GetVideoInfoTag()->SetShowLink(list);
      break;
    }
    case ADDON_VideoInfoTag____resume____________________DATA :
      fileItem->GetVideoInfoTag()->m_resumePoint.timeInSeconds = ((ADDON_VideoInfoTag_Resume*)data)->position;
      fileItem->GetVideoInfoTag()->m_resumePoint.totalTimeInSeconds = ((ADDON_VideoInfoTag_Resume*)data)->total;
      break;
    case ADDON_VideoInfoTag____tvshow_id_________________INT:
      fileItem->GetVideoInfoTag()->m_iIdShow = *(int*)data;
      break;
    case ADDON_VideoInfoTag____date_added________________STRING_DATE_TIME:
    {
      CDateTime date;
      date.SetFromDBDateTime((const char*)data);
      fileItem->GetVideoInfoTag()->m_dateAdded = date;
      break;
    }
    case ADDON_VideoInfoTag____type______________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->GetVideoInfoTag()->m_type = CMediaTypes::FromString(value);
      break;
    }
    case ADDON_VideoInfoTag____season_id_________________INT:
      fileItem->GetVideoInfoTag()->m_iIdSeason = *(int*)data;
      break;
    case ADDON_VideoInfoTag____special_sort_season_______INT:
      fileItem->GetVideoInfoTag()->m_iSpecialSortSeason = *(int*)data;
      break;
    case ADDON_VideoInfoTag____special_sort_episode______INT:
      fileItem->GetVideoInfoTag()->m_iSpecialSortEpisode = *(int*)data;
      break;
    default:
      break;
    }
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

void Interface_GUIListItem::SetPictureInfo(void* kodiInstance, void* handle, unsigned int type, void* data, unsigned int entries)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIListItem::%s - invalid data", __FUNCTION__);
    return;
  }

  Interface_GUIGeneral::Lock();

  try
  {
    CFileItem* fileItem = static_cast<CFileItem *>(handle);
    switch (type)
    {
    case ADDON_PictureInfoTag__count_____________________INT:
      fileItem->m_iprogramCount = *(int*)data;
      break;
    case ADDON_PictureInfoTag__size______________________INT:
      fileItem->m_dwSize = *(int*)data;
      break;
    case ADDON_PictureInfoTag__title_____________________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->m_strTitle = value;
      break;
    }
    case ADDON_PictureInfoTag__picturepath_______________STRING:
    {
      const std::string value = (const char*)data;
      fileItem->SetPath(value);
      break;
    }
    case ADDON_PictureInfoTag__date______________________STRING_DATE_TIME:
    {
      const std::string value = (const char*)data;
      if (strlen(value.c_str()) == 10)
      {
        int year = atoi(value.substr(value.size() - 4).c_str());
        int month = atoi(value.substr(3, 4).c_str());
        int day = atoi(value.substr(0, 2).c_str());
        fileItem->m_dateTime.SetDate(year, month, day);
      }
      break;
    }
    case ADDON_PictureInfoTag__datetime__________________STRING_DATE_TIME:
    {
      const std::string value = (const char*)data;
      fileItem->GetPictureInfoTag()->SetInfo(SLIDE_EXIF_DATE_TIME, value);
      break;
    }
    case ADDON_PictureInfoTag__resolution________________DATA:
    {
      std::string dimension = StringUtils::Format("%i,%i", ((int*)data)[0], ((int*)data)[1]);
      fileItem->GetPictureInfoTag()->SetInfo(SLIDE_RESOLUTION, dimension);
      break;
    }
    default:
      break;
    }
  }
  HANDLE_ADDON_EXCEPTION(addon);

  Interface_GUIGeneral::Unlock();
}

} /* extern "C" */
} /* namespace ADDON */
