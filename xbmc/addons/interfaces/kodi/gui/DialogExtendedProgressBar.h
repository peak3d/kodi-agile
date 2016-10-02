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

  struct Interface_GUIDialogExtendedProgress
  {
    static void Init(sFuncTable_Addon* funcTable);

    static void* New(void* kodiInstance, const char *title);
    static void Delete(void* kodiInstance, void* handle);
    static void Title(void* kodiInstance, void* handle, char &title, unsigned int &iMaxStringSize);
    static void SetTitle(void* kodiInstance, void* handle, const char *title);
    static void Text(void* kodiInstance, void* handle, char &text, unsigned int &iMaxStringSize);
    static void SetText(void* kodiInstance, void* handle, const char *text);
    static bool IsFinished(void* kodiInstance, void* handle);
    static void MarkFinished(void* kodiInstance, void* handle);
    static float Percentage(void* kodiInstance, void* handle);
    static void SetPercentage(void* kodiInstance, void* handle, float fPercentage);
    static void SetProgress(void* kodiInstance, void* handle, int currentItem, int itemCount);
  };

} /* extern "C" */
} /* namespace ADDON */
