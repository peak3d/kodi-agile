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

  struct Interface_GUIControlFadeLabel
  {
    static void Init(sFuncTable_Addon* funcTable);

    static void SetVisible(void* kodiInstance, void* handle, bool visible);
    static void SetEnabled(void* kodiInstance, void* spinhandle, bool enabled);
    static void SetSelected(void* kodiInstance, void* spinhandle, bool selected);

    static void AddLabel(void* kodiInstance, void* handle, const char *label);
    static void GetLabel(void* kodiInstance, void* handle, char &label, unsigned int &iMaxStringSize);
    static void SetScrolling(void* kodiInstance, void* handle, bool scroll);
    static void Reset(void* kodiInstance, void* handle);
  };

} /* extern "C" */
} /* namespace ADDON */
