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

#include "General.h"

#include "addons/kodi-addon-dev-kit/include/kodi/General.h"

#include "Application.h"
#include "CompileInfo.h"
#include "LangInfo.h"
#include "addons/AddonDll.h"
#include "addons/GUIDialogAddonSettings.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "cores/AudioEngine/AEFactory.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/GUIAudioManager.h"
#include "interfaces/builtins/Builtins.h"
#include "interfaces/legacy/aojsonrpc.h" //<! @todo: On next cleanup to ./addons
#ifdef TARGET_POSIX
#include "linux/XMemUtils.h"
#endif
#include "messaging/ApplicationMessenger.h"
#include "settings/Settings.h"
#include "settings/lib/Setting.h"
#include "storage/MediaManager.h"
#include "utils/CharsetConverter.h"
#include "utils/LangCodeExpander.h"
#include "utils/log.h"
#include "utils/md5.h"
#include "utils/StringUtils.h"
#include "utils/XMLUtils.h"

using namespace kodi; // addon-dev-kit namespace
using namespace XFILE;
using namespace KODI::MESSAGING;

namespace ADDON
{

void Interface_General::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi = (sAddonToKodiFuncTable_kodi*)malloc(sizeof(sAddonToKodiFuncTable_kodi));
  funcTable->toKodi.kodi->get_addon_info = get_addon_info;
  funcTable->toKodi.kodi->get_setting = get_setting;
  funcTable->toKodi.kodi->open_settings_dialog = open_settings_dialog;
  funcTable->toKodi.kodi->queue_notification = queue_notification;
  funcTable->toKodi.kodi->queue_notification_from_type = queue_notification_from_type;
  funcTable->toKodi.kodi->queue_notification_with_image = queue_notification_with_image;
  funcTable->toKodi.kodi->get_md5 = get_md5;
  funcTable->toKodi.kodi->unknown_to_utf8 = unknown_to_utf8;
  funcTable->toKodi.kodi->get_localized_string = get_localized_string;
  funcTable->toKodi.kodi->get_language = get_language;
  funcTable->toKodi.kodi->get_dvd_menu_language = get_dvd_menu_language;
  funcTable->toKodi.kodi->start_server = start_server;
  funcTable->toKodi.kodi->audio_suspend = audio_suspend;
  funcTable->toKodi.kodi->audio_resume = audio_resume;
  funcTable->toKodi.kodi->get_volume = get_volume;
  funcTable->toKodi.kodi->set_volume = set_volume;
  funcTable->toKodi.kodi->is_muted = is_muted;
  funcTable->toKodi.kodi->toggle_mute = toggle_mute;
  funcTable->toKodi.kodi->enable_nav_sounds = enable_nav_sounds;
  funcTable->toKodi.kodi->get_optical_state = get_optical_state;
  funcTable->toKodi.kodi->eject_optical_drive = eject_optical_drive;
  funcTable->toKodi.kodi->kodi_version = kodi_version;
  funcTable->toKodi.kodi->kodi_quit = kodi_quit;
  funcTable->toKodi.kodi->htpc_shutdown = htpc_shutdown;
  funcTable->toKodi.kodi->htpc_restart = htpc_restart;
  funcTable->toKodi.kodi->execute_script = execute_script;
  funcTable->toKodi.kodi->execute_builtin = execute_builtin;
  funcTable->toKodi.kodi->execute_jsonrpc = execute_jsonrpc;
  funcTable->toKodi.kodi->get_region = get_region;
  funcTable->toKodi.kodi->get_free_mem = get_free_mem;
  funcTable->toKodi.kodi->get_global_idle_time = get_global_idle_time;
  funcTable->toKodi.kodi->translate_path = translate_path;
}

void Interface_General::DeInit(sFuncTable_Addon* funcTable)
{
  if (funcTable->toKodi.kodi)
  {
    free(funcTable->toKodi.kodi);
    funcTable->toKodi.kodi = nullptr;
  }
}

bool Interface_General::get_setting(
        void*                     kodiInstance,
        const char*               settingName,
        void*                     settingValue,
        bool                      global)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || settingName == nullptr || settingValue == nullptr)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p', settingName='%p', settingValue='%p')",
                                        __FUNCTION__, addon, settingName, settingValue);

    return false;
  }

  try
  {
    CLog::Log(LOGDEBUG, "Interface_General - %s - add-on '%s' requests setting '%s' from '%s'",
                          __FUNCTION__, addon->Name().c_str(), settingName, global ? "global" : "add-on");

    if (global)
    {
      std::string settingName = settingName;
      if (StringUtils::EqualsNoCase(settingName, CSettings::SETTING_PVRPARENTAL_PIN) ||
          StringUtils::EqualsNoCase(settingName, CSettings::SETTING_MASTERLOCK_LOCKCODE) ||
          StringUtils::EqualsNoCase(settingName, CSettings::SETTING_SYSTEM_PLAYLISTSPATH) ||
          StringUtils::StartsWithNoCase(settingName, "services.") || // CSettings::SETTING_SERVICES_...)
          StringUtils::StartsWithNoCase(settingName, "smb.")      || // CSettings::SETTING_SMB_...)
          StringUtils::StartsWithNoCase(settingName, "network.")  || // CSettings::SETTING_NETWORK_...)
          StringUtils::StartsWithNoCase(settingName, "cache"))       // CSettings::SETTING_CACHE...)
      {
        CLog::Log(LOGERROR, "Interface_General - %s - add-on '%s' requests not allowed global setting '%s'!",
                              __FUNCTION__, addon->Name().c_str(), settingName.c_str());
        return false;
      }

      const CSetting* setting = CSettings::GetInstance().GetSetting(settingName);
      if (setting == nullptr)
      {
        CLog::Log(LOGERROR, "Interface_General - %s - can't find global setting '%s'", __FUNCTION__, settingName.c_str());
        return false;
      }

      switch (setting->GetType())
      {
      case SettingTypeBool:
        *(bool*) settingValue = dynamic_cast<const CSettingBool*>(setting)->GetValue();
      break;
      case SettingTypeInteger:
        *(int*) settingValue = dynamic_cast<const CSettingInt*>(setting)->GetValue();
      break;
      case SettingTypeNumber:
        *(float*) settingValue = dynamic_cast<const CSettingNumber*>(setting)->GetValue();
      break;
      case SettingTypeString:
        strcpy((char*) settingValue, dynamic_cast<const CSettingString*>(setting)->GetValue().c_str());
      break;
      default:
        CLog::Log(LOGERROR, "Interface_General - %s - not supported type for global setting '%s'", __FUNCTION__, settingName.c_str());
        return false;
      }
      return true;
    }

    if (strcasecmp(settingName, "__addonpath__") == 0)
    {
      strcpy((char*) settingValue, addon->Path().c_str());
      return true;
    }

    if (!addon->ReloadSettings())
    {
      CLog::Log(LOGERROR, "Interface_General - %s - could't get settings for add-on '%s'", __FUNCTION__, addon->Name().c_str());
      return false;
    }

    const TiXmlElement *category = addon->GetSettingsXML()->FirstChildElement("category");
    if (!category) // add a default one...
      category = addon->GetSettingsXML();

    while (category)
    {
      const TiXmlElement *setting = category->FirstChildElement("setting");
      while (setting)
      {
        const std::string   id = XMLUtils::GetAttribute(setting, "id");
        const std::string type = XMLUtils::GetAttribute(setting, "type");

        if (id == settingName && !type.empty())
        {
          if (type == "text"     || type == "ipaddress" ||
              type == "folder"   || type == "action"    ||
              type == "music"    || type == "pictures"  ||
              type == "programs" || type == "fileenum"  ||
              type == "file"     || type == "labelenum")
          {
            strcpy((char*) settingValue, addon->GetSetting(id).c_str());
            return true;
          }
          else if (type == "number" || type == "enum")
          {
            *(int*) settingValue = (int) atoi(addon->GetSetting(id).c_str());
            return true;
          }
          else if (type == "bool")
          {
            *(bool*) settingValue = (bool) (addon->GetSetting(id) == "true" ? true : false);
            return true;
          }
          else if (type == "slider")
          {
            const char *option = setting->Attribute("option");
            if (option && strcmpi(option, "int") == 0)
            {
              *(int*) settingValue = (int) atoi(addon->GetSetting(id).c_str());
              return true;
            }
            else
            {
              *(float*) settingValue = (float) atof(addon->GetSetting(id).c_str());
              return true;
            }
          }
        }
        setting = setting->NextSiblingElement("setting");
      }
      category = category->NextSiblingElement("category");
    }
    CLog::Log(LOGERROR, "Interface_General - %s - can't find setting '%s' in '%s'", __FUNCTION__, settingName, addon->Name().c_str());
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }

  return false;
}

void Interface_General::open_settings_dialog(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - called with a null pointer", __FUNCTION__);
    return;
  }
  
  try
  {
    // show settings dialog
    CGUIDialogAddonSettings::ShowAndGetInput(ADDON::AddonPtr(addon));
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}

void Interface_General::queue_notification(
        void*                     kodiInstance,
        const int                 type,
        const char*               message)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || message == nullptr)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - called with a null pointer", __FUNCTION__);
    return;
  }

  try
  {
    switch (type)
    {
      case QUEUE_WARNING:
        CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Warning, addon->Name(), message, 3000, true);
        CLog::Log(LOGDEBUG, "Interface_General - %s - %s - Warning Message: '%s'", __FUNCTION__, addon->Name().c_str(), message);
        break;
    
      case QUEUE_ERROR:
        CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Error, addon->Name(), message, 3000, true);
        CLog::Log(LOGDEBUG, "Interface_General - %s - %s - Error Message : '%s'", __FUNCTION__, addon->Name().c_str(), message);
        break;
    
      case QUEUE_INFO:
      default:
        CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info, addon->Name(), message, 3000, false);
        CLog::Log(LOGDEBUG, "Interface_General - %s - %s - Info Message : '%s'", __FUNCTION__, addon->Name().c_str(), message);
        break;
    }
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}

void Interface_General::queue_notification_from_type(
        void*                     kodiInstance,
        const int                 type,
        const char*               aCaption,
        const char*               aDescription,
        unsigned int              displayTime,
        bool                      withSound,
        unsigned int              messageTime)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || aCaption == nullptr || aDescription == nullptr)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - called with a null pointer", __FUNCTION__);
    return;
  }
  
  try
  {
    CGUIDialogKaiToast::eMessageType usedType;
    switch (type)
    {
    case QUEUE_WARNING:
      usedType = CGUIDialogKaiToast::Warning;
      break;
    case QUEUE_ERROR:
      usedType = CGUIDialogKaiToast::Error;
      break;
    case QUEUE_INFO:
    default:
      usedType = CGUIDialogKaiToast::Info;
      break;
    }
    CGUIDialogKaiToast::QueueNotification(usedType, aCaption, aDescription, displayTime, withSound, messageTime);
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}

void Interface_General::queue_notification_with_image(
        void*                     kodiInstance,
        const char*               aImageFile,
        const char*               aCaption,
        const char*               aDescription,
        unsigned int              displayTime,
        bool                      withSound,
        unsigned int              messageTime)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || aImageFile == nullptr || aCaption == nullptr  || aDescription == nullptr)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - called with a null pointer", __FUNCTION__);
    return;
  }

  try
  {
    CGUIDialogKaiToast::QueueNotification(aImageFile, aCaption, aDescription, displayTime, withSound, messageTime);
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}

void Interface_General::get_md5(
        void*                     kodiInstance,
        const char*               text,
        char&                     md5)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || text == nullptr)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p', text='%p')", __FUNCTION__, addon, text);
    return;
  }

  try
  {
    std::string md5Int = XBMC::XBMC_MD5::GetMD5(std::string(text));
    strncpy(&md5, md5Int.c_str(), 40);
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}
      
char* Interface_General::unknown_to_utf8(
        void*                     kodiInstance,
        const char*               source,
        bool&                     ret,
        bool                      failOnBadChar)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon || !source)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p', source='%p')", __FUNCTION__, addon, source);
    return nullptr;
  }
  
  try
  {        
    std::string string;
    ret = g_charsetConverter.unknownToUTF8(source, string, failOnBadChar);
    char* buffer = strdup(string.c_str());
    return buffer;
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }

  return nullptr;
}

char* Interface_General::get_localized_string(
        void*                     kodiInstance,
        long                      dwCode)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return nullptr;
  }

  try
  {
    if (g_application.m_bStop)
      return nullptr;

    std::string string;
    if ((dwCode >= 30000 && dwCode <= 30999) || (dwCode >= 32000 && dwCode <= 32999))
      string = g_localizeStrings.GetAddonString(addon->ID(), dwCode).c_str();
    else
      string = g_localizeStrings.Get(dwCode).c_str();

    char* buffer = strdup(string.c_str());
    return buffer;
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }

  return nullptr;
}

void Interface_General::get_language(void* kodiInstance, char& language, unsigned int& iMaxStringSize, int format, bool region)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return;
  }

  try
  {
    std::string str = g_langInfo.GetEnglishLanguageName();
    switch (format)
    {
      case LANG_FMT_ISO_639_1:
      {
        std::string langCode;
        g_LangCodeExpander.ConvertToISO6391(str, langCode);
        str = langCode;
        if (region)
        {
          std::string region2Code;
          g_LangCodeExpander.ConvertToISO6391(g_langInfo.GetRegionLocale(), region2Code);
          if (!region2Code.empty())
            str += "-" + region2Code;
        }
        break;
      }
      case LANG_FMT_ISO_639_2:
      {
        std::string langCode;
        g_LangCodeExpander.ConvertToISO6392T(str, langCode);
        str = langCode;
        if (region)
        {
          std::string region3Code;
          g_LangCodeExpander.ConvertToISO6392T(g_langInfo.GetRegionLocale(), region3Code);
          if (!region3Code.empty())
            str += "-" + region3Code;
        }
        break;
      }
      case LANG_FMT_ENGLISH_NAME:
      default:
      {
        if (region)
          str += "-" + g_langInfo.GetCurrentRegion();
        break;
      }
    }

    strncpy(&language, str.c_str(), iMaxStringSize);
    iMaxStringSize = str.length();
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}

void Interface_General::get_dvd_menu_language(void* kodiInstance, char& language, unsigned int& iMaxStringSize)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return;
  }

  try
  {
    std::string str = g_langInfo.GetDVDMenuLanguage();
    strncpy(&language, str.c_str(), iMaxStringSize);
    iMaxStringSize = str.length();
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}

bool Interface_General::start_server(void* kodiInstance, int typ, bool bStart, bool bWait)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return false;
  }

  try
  {
    CApplication::ESERVERS iTyp;
    switch (typ)
    {
    case ADDON_ES_WEBSERVER:     iTyp = CApplication::ES_WEBSERVER;     break;
    case ADDON_ES_AIRPLAYSERVER: iTyp = CApplication::ES_AIRPLAYSERVER; break;
    case ADDON_ES_JSONRPCSERVER: iTyp = CApplication::ES_JSONRPCSERVER; break;
    case ADDON_ES_UPNPRENDERER:  iTyp = CApplication::ES_UPNPRENDERER;  break;
    case ADDON_ES_UPNPSERVER:    iTyp = CApplication::ES_UPNPSERVER;    break;
    case ADDON_ES_EVENTSERVER:   iTyp = CApplication::ES_EVENTSERVER;   break;
    case ADDON_ES_ZEROCONF:      iTyp = CApplication::ES_ZEROCONF;      break;
    default:
      CLog::Log(LOGERROR, "Interface_General - %s - %s - Error Message : Not supported server type '%i'",
                            __FUNCTION__, addon->Name().c_str(), typ);
      return false;
    }
    return g_application.StartServer(iTyp, bStart, bWait);
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
  
  return false;
}
  
void Interface_General::audio_suspend(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return;
  }

  try
  { 
    CAEFactory::Suspend();
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}
  
void Interface_General::audio_resume(void* kodiInstance)
{ 
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return;
  }

  try
  { 
    CAEFactory::Resume();
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}
  
float Interface_General::get_volume(void* kodiInstance, bool percentage)
{ 
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return 0.0f;
  }

  try
  { 
    return g_application.GetVolume(percentage);
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }

  return 0.0f;
}
  
void Interface_General::set_volume(
        void*                     kodiInstance,
        float                     value,
        bool                      isPercentage)
{ 
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return;
  }

  try
  {
    g_application.SetVolume(value, isPercentage);
  }                                     
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
} 

bool Interface_General::is_muted(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return false;
  }

  try
  { 
    return g_application.IsMutedInternal();
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
  
  return false;
} 
  
void Interface_General::toggle_mute(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return;
  }

  try
  { 
    g_application.ToggleMute();         
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
} 
  
void Interface_General::enable_nav_sounds(void* kodiInstance, bool yesNo)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return;
  }

  try
  { 
    g_audioManager.Enable(yesNo);
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
} 
  
long Interface_General::get_optical_state(void* kodiInstance)
{ 
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return 0;
  }

  try
  { 
    return g_mediaManager.GetDriveStatus();
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
  
  return 0;
}
  
bool Interface_General::eject_optical_drive(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return false;
  }

  try
  { 
    return CBuiltins::GetInstance().Execute("EjectTray") == 0 ? true : false;
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }

  return false;
}

void Interface_General::kodi_version(
      void*                     kodiInstance,
      char*&                    compile_name,
      int&                      major,
      int&                      minor,
      char*&                    revision,
      char*&                    tag,
      char*&                    tagversion)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return;
  }

  try
  { 
    compile_name = strdup(CCompileInfo::GetAppName());
    major        = CCompileInfo::GetMajor();
    minor        = CCompileInfo::GetMinor();
    revision     = strdup(CCompileInfo::GetSCMID());
    std::string tagStr = CCompileInfo::GetSuffix();
    if (StringUtils::StartsWithNoCase(tagStr, "alpha"))
    {
      tag = strdup("alpha");
      tagversion = strdup(StringUtils::Mid(tagStr, 5).c_str());
    }
    else if (StringUtils::StartsWithNoCase(tagStr, "beta"))
    {
      tag = strdup("beta");
      tagversion = strdup(StringUtils::Mid(tagStr, 4).c_str());
    }
    else if (StringUtils::StartsWithNoCase(tagStr, "rc"))
    {
      tag = strdup("releasecandidate");
      tagversion = strdup(StringUtils::Mid(tagStr, 2).c_str());
    }
    else if (tagStr.empty())
      tag = strdup("stable");
    else
      tag = strdup("prealpha");
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}

void Interface_General::kodi_quit(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return;
  }

  try
  { 
    CApplicationMessenger::GetInstance().PostMsg(TMSG_QUIT);
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}
      
void Interface_General::htpc_shutdown(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return;
  }

  try
  { 
    CApplicationMessenger::GetInstance().PostMsg(TMSG_SHUTDOWN);
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}
      
void Interface_General::htpc_restart(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return;
  }

  try
  { 
    CApplicationMessenger::GetInstance().PostMsg(TMSG_RESTART);
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}
      
void Interface_General::execute_script(void* kodiInstance, const char* script)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon || !script)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p', script='%p')", __FUNCTION__, addon, script);
    return;
  }

  try
  { 
    CApplicationMessenger::GetInstance().PostMsg(TMSG_EXECUTE_SCRIPT, -1, -1, nullptr, script);
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}
      
void Interface_General::execute_builtin(void* kodiInstance, const char* function, bool wait)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon || !function)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p', function='%p')", __FUNCTION__, addon, function);
    return;
  }

  try
  { 
    if (wait)
      CApplicationMessenger::GetInstance().SendMsg(TMSG_EXECUTE_BUILT_IN, -1, -1, nullptr, function);
    else
      CApplicationMessenger::GetInstance().PostMsg(TMSG_EXECUTE_BUILT_IN, -1, -1, nullptr, function);
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
}
    
char* Interface_General::execute_jsonrpc(void* kodiInstance, const char* jsonrpccommand)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon || !jsonrpccommand)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p', jsonrpccommand='%p')", __FUNCTION__, addon, jsonrpccommand);
    return nullptr;
  }

  try
  {
    CAddOnTransport transport;
    CAddOnTransport::CAddOnClient client;
    std::string string = JSONRPC::CJSONRPC::MethodCall(/*method*/ jsonrpccommand, &transport, &client);
    char* buffer = strdup(string.c_str());
    return buffer;
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }

  return nullptr;
}

char* Interface_General::get_region(void* kodiInstance, const char* id)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon || !id)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p', id='%p')", __FUNCTION__, addon, id);
    return nullptr;
  }

  try
  {
    std::string result;
    if (strcmpi(id, "datelong") == 0)
    {
      result = g_langInfo.GetDateFormat(true);
      StringUtils::Replace(result, "DDDD", "%A");
      StringUtils::Replace(result, "MMMM", "%B");
      StringUtils::Replace(result, "D", "%d");
      StringUtils::Replace(result, "YYYY", "%Y");
    }
    else if (strcmpi(id, "dateshort") == 0)
    {
      result = g_langInfo.GetDateFormat(false);
      StringUtils::Replace(result, "MM", "%m");
      StringUtils::Replace(result, "DD", "%d");
#ifdef TARGET_WINDOWS
      StringUtils::Replace(result, "M", "%#m");
      StringUtils::Replace(result, "D", "%#d");
#else
      StringUtils::Replace(result, "M", "%-m");
      StringUtils::Replace(result, "D", "%-d");
#endif
      StringUtils::Replace(result, "YYYY", "%Y");
    }
    else if (strcmpi(id, "tempunit") == 0)
      result = g_langInfo.GetTemperatureUnitString();
    else if (strcmpi(id, "speedunit") == 0)
      result = g_langInfo.GetSpeedUnitString();
    else if (strcmpi(id, "time") == 0)
    {
      result = g_langInfo.GetTimeFormat();
      StringUtils::Replace(result, "H", "%H");
      StringUtils::Replace(result, "h", "%I");
      StringUtils::Replace(result, "mm", "%M");
      StringUtils::Replace(result, "ss", "%S");
      StringUtils::Replace(result, "xx", "%p");
    }
    else if (strcmpi(id, "meridiem") == 0)
      result = StringUtils::Format("%s/%s",
                                   g_langInfo.GetMeridiemSymbol(MeridiemSymbolAM).c_str(),
                                   g_langInfo.GetMeridiemSymbol(MeridiemSymbolPM).c_str());
    else
    {
      CLog::Log(LOGERROR, "kodi::General::%s -  add-on '%s' requests invalid id '%s'",
                            __FUNCTION__, addon->Name().c_str(), id);
      return nullptr;
    }

    char* buffer = strdup(result.c_str());
    return buffer;
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }
    
  return nullptr;
}

long Interface_General::get_free_mem(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return -1;
  }

  try
  {
    MEMORYSTATUSEX stat;
    stat.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&stat);
    return (long)(stat.ullAvailPhys  / ( 1024 * 1024 ));
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }

  return -1;
}

int Interface_General::get_global_idle_time(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return -1;
  }

  try
  {
    return g_application.GlobalIdleTime();
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }

  return -1;
}

char* Interface_General::get_addon_info(void* kodiInstance, const char* id)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon || !id)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p', id='%p')", __FUNCTION__, addon, id);
    return nullptr;
  }

  try
  {
    std::string str;
    if (strcmpi(id, "author") == 0)
      str = addon->Author();
    else if (strcmpi(id, "changelog") == 0)
      str = addon->ChangeLog();
    else if (strcmpi(id, "description") == 0)
      str = addon->Description();
    else if (strcmpi(id, "disclaimer") == 0)
      str = addon->Disclaimer();
    else if (strcmpi(id, "fanart") == 0)
      str = addon->FanArt();
    else if (strcmpi(id, "icon") == 0)
      str = addon->Icon();
    else if (strcmpi(id, "id") == 0)
      str = addon->ID();
    else if (strcmpi(id, "name") == 0)
      str = addon->Name();
    else if (strcmpi(id, "path") == 0)
      str = addon->Path();
    else if (strcmpi(id, "profile") == 0)
      str = addon->Profile();
    else if (strcmpi(id, "summary") == 0)
      str = addon->Summary();
    else if (strcmpi(id, "type") == 0)
      str = ADDON::TranslateType(addon->Type());
    else if (strcmpi(id, "version") == 0)
      str = addon->Version().asString();
    else
    {
      CLog::Log(LOGERROR, "kodi::General::%s -  add-on '%s' requests invalid id '%s'",
                            __FUNCTION__, addon->Name().c_str(), id);
      return nullptr;
    }

    char* buffer = strdup(str.c_str());
    return buffer;
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }

  return nullptr;
}
      
char* Interface_General::translate_path(void* kodiInstance, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon || !path)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p', path='%p')", __FUNCTION__, addon, path);
    return nullptr;
  }

  try
  {
    std::string string = CSpecialProtocol::TranslatePath(path);
    char* buffer = strdup(string.c_str());
    return buffer;
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, __FUNCTION__);
  }

  return nullptr;
}

} /* namespace ADDON */
