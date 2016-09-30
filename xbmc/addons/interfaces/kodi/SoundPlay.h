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

  struct Interface_Audio
  {
    static void Init(sFuncTable_Addon* funcTable);
    static void DeInit(sFuncTable_Addon* funcTable);

    static void* soundplay_get_handle(void* kodiInstance, const char* filename);
    static void soundplay_release_handle(void* kodiInstance, void* sndHandle);
    static void soundplay_play(void* kodiInstance, void* sndHandle);
    static void soundplay_stop(void* kodiInstance, void* sndHandle);
    static void soundplay_set_channel(void* kodiInstance, void* sndHandle, int channel);
    static int soundplay_get_channel(void* kodiInstance, void* sndHandle);
    static void soundplay_set_volume(void* kodiInstance, void* sndHandle, float volume);
    static float soundplay_get_volume(void* kodiInstance, void* sndHandle);
  };

} /* namespace ADDON */
