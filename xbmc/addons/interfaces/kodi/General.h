#pragma once
/*
 *      Copyright (C) 2012-2013 Team XBMC
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

  struct Interface_General
  {
    static void Init(sFuncTable_Addon* funcTable);
    static void DeInit(sFuncTable_Addon* funcTable);

    static char* get_addon_info(void* kodiInstance, const char* id);
    static bool get_setting(void* kodiInstance, const char* settingName, void* settingValue, bool global);
    static void open_settings_dialog(void* kodiInstance);
    static void queue_notification(void* kodiInstance, const int type, const char* message);
    static void queue_notification_from_type(void* kodiInstance, const int type, const char* aCaption, const char* aDescription, unsigned int displayTime, bool withSound, unsigned int messageTime);
    static void queue_notification_with_image(void* kodiInstance, const char* aImageFile, const char* aCaption, const char* aDescription, unsigned int displayTime, bool withSound, unsigned int messageTime);
    static void get_md5(void* kodiInstance, const char* text, char& md5);
    static char* unknown_to_utf8(void* kodiInstance, const char* source, bool& ret, bool failOnBadChar);
    static char* get_localized_string(void* kodiInstance, long dwCode);
    static void get_language(void* kodiInstance, char& language, unsigned int& iMaxStringSize, int format, bool region);
    static void get_dvd_menu_language(void* kodiInstance, char &language, unsigned int &iMaxStringSize);
    static bool start_server(void* kodiInstance, int typ, bool start, bool wait);
    static void audio_suspend(void* kodiInstance);
    static void audio_resume(void* kodiInstance);
    static float get_volume(void* kodiInstance, bool percentage);
    static void set_volume(void* kodiInstance, float value, bool isPercentage);
    static bool is_muted(void* kodiInstance);
    static void toggle_mute(void* kodiInstance);
    static void enable_nav_sounds(void* kodiInstance, bool yesNo);
    static long get_optical_state(void* kodiInstance);
    static bool eject_optical_drive(void* kodiInstance);
    static void kodi_version(void* kodiInstance, char*& compile_name, int& major, int& minor, char*& revision, char*& tag, char*& tagversion);
    static void kodi_quit(void* kodiInstance);
    static void htpc_shutdown(void* kodiInstance);
    static void htpc_restart(void* kodiInstance);
    static void execute_script(void* kodiInstance, const char* script);
    static void execute_builtin(void* kodiInstance, const char* function, bool wait);
    static char* execute_jsonrpc(void* kodiInstance, const char* jsonrpccommand);
    static char* get_region(void* kodiInstance, const char* id);
    static long get_free_mem(void* kodiInstance);
    static int  get_global_idle_time(void* kodiInstance);
    static char* translate_path(void* kodiInstance, const char* path);
  };

} /* namespace ADDON */
