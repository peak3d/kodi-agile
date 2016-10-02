#pragma once
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

struct sFuncTable_Addon;

namespace ADDON
{
extern "C"
{

  struct Interface_GUIListItem
  {
    static void Init(sFuncTable_Addon* funcTable);

    static void* Create(void* kodiInstance, const char *label, const char *label2, const char *iconImage, const char *thumbnailImage, const char *path);
    static void Destroy(void* kodiInstance, void* handle);
    static void GetLabel(void* kodiInstance, void* handle, char &label, unsigned int &iMaxStringSize);
    static void SetLabel(void* kodiInstance, void* handle, const char *label);
    static void GetLabel2(void* kodiInstance, void* handle, char &label, unsigned int &iMaxStringSize);
    static void SetLabel2(void* kodiInstance, void* handle, const char *label);
    static void GetIconImage(void* kodiInstance, void* handle, char &image, unsigned int &iMaxStringSize);
    static void SetIconImage(void* kodiInstance, void* handle, const char *image);
    static void GetOverlayImage(void* kodiInstance, void* handle, char &image, unsigned int &iMaxStringSize);
    static void SetOverlayImage(void* kodiInstance, void* handle, unsigned int image, bool bOnOff /* = false*/);
    static void SetThumbnailImage(void* kodiInstance, void* handle, const char *image);
    static void SetArt(void* kodiInstance, void* handle, const char *type, const char *url);
    static char* GetArt(void* kodiInstance, void* handle, const char *type);
    static void SetArtFallback(void* kodiInstance, void* handle, const char *from, const char *to);
    static bool HasArt(void* kodiInstance, void* handle, const char *type);
    static void Select(void* kodiInstance, void* handle, bool bOnOff);
    static bool IsSelected(void* kodiInstance, void* handle);
    static bool HasIcon(void* kodiInstance, void* handle);
    static bool HasOverlay(void* kodiInstance, void* handle);
    static bool IsFileItem(void* kodiInstance, void* handle);
    static bool IsFolder(void* kodiInstance, void* handle);
    static void SetProperty(void* kodiInstance, void* handle, const char *key, const char *value);
    static void GetProperty(void* kodiInstance, void* handle, const char *key, char &property, unsigned int &iMaxStringSize);
    static void ClearProperty(void* kodiInstance, void* handle, const char *key);
    static void ClearProperties(void* kodiInstance, void* handle);
    static bool HasProperties(void* kodiInstance, void* handle);
    static bool HasProperty(void* kodiInstance, void* handle, const char *key);
    static void SetPath(void* kodiInstance, void* handle, const char *path);
    static char* GetPath(void* kodiInstance, void* handle);
    static int  GetDuration(void* kodiInstance, void* handle);
    static void SetSubtitles(void* kodiInstance, void* handle, const char** streams, unsigned int entries);
    static void SetMimeType(void* kodiInstance, void* handle, const char* mimetype);
    static void SetContentLookup(void* kodiInstance, void* handle, bool enable);
    static void AddContextMenuItems(void* kodiInstance, void* handle, const char** menus[2], unsigned int entries, bool replaceItems);
    static void AddStreamInfo(void* kodiInstance, void* handle, const char* cType, const char** dictionary[2], unsigned int entries);
    static void SetMusicInfo(void* kodiInstance, void* handle, unsigned int type, void* data, unsigned int entries);
    static void SetVideoInfo(void* kodiInstance, void* handle, unsigned int type, void* data, unsigned int entries);
    static void SetPictureInfo(void* kodiInstance, void* handle, unsigned int type, void* data, unsigned int entries);
  };

} /* extern "C" */
} /* namespace ADDON */
