#pragma once
/*
 *      Copyright (C) 2005-2017 Team Kodi
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
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef TARGET_WINDOWS
#include <windows.h>
#else
#ifndef __cdecl
#define __cdecl
#endif
#ifndef __declspec
#define __declspec(X)
#endif
#endif

#include "versions.h"

namespace kodi { namespace addon { class CAddonBase; }}
namespace kodi { namespace addon { class IAddonInstance; }}

extern "C" {

//==============================================================================
/// Standard undefined pointer handle
typedef void* KODI_HANDLE;
//------------------------------------------------------------------------------

//==============================================================================
///
typedef enum ADDON_STATUS
{
  ///
  ADDON_STATUS_OK,

  ///
  ADDON_STATUS_LOST_CONNECTION,

  ///
  ADDON_STATUS_NEED_RESTART,

  ///
  ADDON_STATUS_NEED_SETTINGS,

  ///
  ADDON_STATUS_UNKNOWN,

  ///
  ADDON_STATUS_NEED_SAVEDSETTINGS,

  /// permanent failure, like failing to resolve methods
  ADDON_STATUS_PERMANENT_FAILURE,

  /* internal used return error if function becomes not used from child on
   * addon */
  ADDON_STATUS_NOT_IMPLEMENTED
} ADDON_STATUS;
//------------------------------------------------------------------------------

//==============================================================================
/// @todo remove start with ADDON_* after old way on libXBMC_addon.h is removed
/// 
typedef enum AddonLog
{
  /// 
  ADDON_LOG_DEBUG = 0,

  /// 
  ADDON_LOG_INFO = 1,

  /// 
  ADDON_LOG_NOTICE = 2,

  /// 
  ADDON_LOG_WARNING = 3,

  /// 
  ADDON_LOG_ERROR = 4,

  /// 
  ADDON_LOG_SEVERE = 5,

  ///
  ADDON_LOG_FATAL = 6
} AddonLog;
//------------------------------------------------------------------------------

/*
 *
 */
typedef struct
{
  int           type;
  char*         id;
  char*         label;
  int           current;
  char**        entry;
  unsigned int  entry_elements;
} ADDON_StructSetting;

/*!
 * @brief Handle used to return data from the PVR add-on to CPVRClient
 */
struct ADDON_HANDLE_STRUCT
{
  void *callerAddress;  /*!< address of the caller */
  void *dataAddress;    /*!< address to store data in */
  int   dataIdentifier; /*!< parameter to pass back when calling the callback */
};
typedef ADDON_HANDLE_STRUCT *ADDON_HANDLE;

/*
 * To have a on add-on and kodi itself handled string always on known size!
 */
#define ADDON_STANDARD_STRING_LENGTH 1024
#define ADDON_STANDARD_STRING_LENGTH_SMALL 256

/*
 * Callback function tables from addon to Kodi
 * Set complete from Kodi!
 */
struct AddonToKodiFuncTable_kodi;
struct AddonToKodiFuncTable_kodi_gui;
typedef struct AddonToKodiFuncTable_Addon
{
  // Pointer inside Kodi, used on callback functions to give related handle
  // class, for this ADDON::CAddonDll inside Kodi.
  KODI_HANDLE kodiBase;

  // Function addresses used for callbacks from addon to Kodi
  char* (*get_addon_path)(void* kodiBase);
  char* (*get_base_user_path)(void* kodiBase);
  void (*addon_log_msg)(void* kodiBase, const int loglevel, const char *msg);
  void (*free_string)(void* kodiBase, char* str);

  AddonToKodiFuncTable_kodi* kodi;
  AddonToKodiFuncTable_kodi_gui* kodi_gui;
} AddonToKodiFuncTable_Addon;

/*
 * Function tables from Kodi to addon
 */
typedef struct KodiToAddonFuncTable_Addon
{
  ADDON_STATUS (*set_setting)(const char *settingName, const void *settingValue, bool lastSetting);
} KodiToAddonFuncTable_Addon;

/*
 * Main structure passed from kodi to addon with basic information needed to
 * create add-on.
 */
typedef struct AddonGlobalInterface
{
  // String with full path where add-on is installed (without his name on end)
  // Set from Kodi!
  const char* libBasePath;

  // Pointer of first created instance, used in case this add-on goes with single way
  // Set from Kodi!
  KODI_HANDLE firstKodiInstance;

  // Pointer to master base class inside add-on
  // Set from addon header!
  kodi::addon::CAddonBase* addonBase;

  // Pointer to a instance used on single way (together with this class)
  // Set from addon header!
  kodi::addon::IAddonInstance* globalSingleInstance;

  // Callback function tables from addon to Kodi
  // Set from Kodi!
  AddonToKodiFuncTable_Addon toKodi;

  // Function tables from Kodi to addon
  // Set from addon header!
  KodiToAddonFuncTable_Addon toAddon;
} AddonGlobalInterface;

} /* extern "C" */

//==============================================================================
namespace kodi {
namespace addon {
/*
 * Internal class to control various instance types with general parts defined
 * here.
 *
 * Mainly is this currently used to identify requested instance types.
 *
 * @note This class is not need to know during add-on development thats why
 * commented with "*".
 */
class IAddonInstance
{
public:
  IAddonInstance(ADDON_TYPE type) : m_type(type) { }
  virtual ~IAddonInstance() { }

  virtual ADDON_STATUS CreateInstance(int instanceType, std::string instanceID, KODI_HANDLE instance, KODI_HANDLE& addonInstance)
  {
    return ADDON_STATUS_NOT_IMPLEMENTED;
  }

  const ADDON_TYPE m_type;
};
} /* namespace addon */
} /* namespace kodi */
//------------------------------------------------------------------------------

//==============================================================================
namespace kodi {
/// 
class CSettingValue
{
public:
  CSettingValue(const void *settingValue) : m_settingValue(settingValue) {}

  bool empty() const { return (m_settingValue == nullptr) ? true : false; }
  std::string GetString() const { return (char*)m_settingValue; }
  int GetInt() const { return *(int*)m_settingValue; }
  unsigned int GetUInt() const { return *(unsigned int*)m_settingValue; }
  bool GetBoolean() const { return *(bool*)m_settingValue; }
  float GetFloat() const { return *(float*)m_settingValue; }

private:
  const void *m_settingValue;
};
} /* namespace kodi */
//------------------------------------------------------------------------------

//==============================================================================
namespace kodi {
namespace addon {
/// Add-on main instance class.
/// @todo need bigger improvement and rework
class CAddonBase
{
public:
  CAddonBase()
  {
    CAddonBase::m_interface->toAddon.set_setting = ADDONBASE_SetSetting;
  }

  virtual ~CAddonBase()
  {
  }

  virtual ADDON_STATUS Create() { return ADDON_STATUS_OK; }

  virtual ADDON_STATUS GetStatus() { return ADDON_STATUS_OK; }

  virtual ADDON_STATUS SetSetting(const std::string& settingName, const CSettingValue& settingValue, bool lastSetting) { return ADDON_STATUS_UNKNOWN; }

  virtual ADDON_STATUS CreateInstance(int instanceType, std::string instanceID, KODI_HANDLE instance, KODI_HANDLE& addonInstance)
  {
    /* The handling below is intended for the case of the add-on only one
      * instance and this is integrated in the add-on base class.
      */

    /* Check about single instance usage */
    if (CAddonBase::m_interface->firstKodiInstance == instance && // the kodi side instance pointer must be equal to first one
        CAddonBase::m_interface->globalSingleInstance &&  // the addon side instance pointer must be set
        CAddonBase::m_interface->globalSingleInstance->m_type == instanceType) // and the requested type must be equal with used add-on class
    {
      addonInstance = CAddonBase::m_interface->globalSingleInstance;
      return ADDON_STATUS_OK;
    }

    return ADDON_STATUS_UNKNOWN;
  }

  /* Global variables of class */
  static AddonGlobalInterface* m_interface; // Interface function table to hold addresses on add-on and from kodi

/*private:*/ /* Needed public as long the old call functions becomes used! */
  static inline void ADDONBASE_Destroy()
  {
    delete CAddonBase::m_interface->addonBase;
    CAddonBase::m_interface->addonBase = nullptr;
  }

  static inline ADDON_STATUS ADDONBASE_GetStatus() { return CAddonBase::m_interface->addonBase->GetStatus(); }

  static inline ADDON_STATUS ADDONBASE_SetSetting(const char *settingName, const void *settingValue, bool lastSetting)
  {
    return CAddonBase::m_interface->addonBase->SetSetting(settingName, CSettingValue(settingValue), lastSetting);
  }

  static inline ADDON_STATUS ADDONBASE_CreateInstance(int instanceType, const char* instanceID, KODI_HANDLE instance, KODI_HANDLE* addonInstance, ::kodi::addon::IAddonInstance* parent = nullptr)
  {
    ADDON_STATUS status = ADDON_STATUS_NOT_IMPLEMENTED;
    if (parent != nullptr)
      status = parent->CreateInstance(instanceType, instanceID, instance, *addonInstance);
    if (status == ADDON_STATUS_NOT_IMPLEMENTED)
      status = CAddonBase::m_interface->addonBase->CreateInstance(instanceType, instanceID, instance, *addonInstance);
    if (*addonInstance == nullptr)
      throw std::logic_error("kodi::addon::CAddonBase CreateInstance returns a empty instance pointer!");

    if (static_cast<::kodi::addon::IAddonInstance*>(*addonInstance)->m_type != instanceType)
      throw std::logic_error("kodi::addon::CAddonBase CreateInstance with difference on given and returned instance type!");

    return status;
  }

  static inline void ADDONBASE_DestroyInstance(int instanceType, KODI_HANDLE instance)
  {
    if (CAddonBase::m_interface->globalSingleInstance == nullptr && instance != CAddonBase::m_interface->addonBase)
    {
      if (static_cast<::kodi::addon::IAddonInstance*>(instance)->m_type == instanceType)
        delete static_cast<::kodi::addon::IAddonInstance*>(instance);
      else
        throw std::logic_error("kodi::addon::CAddonBase DestroyInstance called with difference on given and present instance type!");
    }
  }
};
} /* namespace addon */
} /* namespace kodi */
//------------------------------------------------------------------------------

//==============================================================================
namespace kodi {
/// 
inline std::string GetAddonPath()
{
  char* str = ::kodi::addon::CAddonBase::m_interface->toKodi.get_addon_path(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase);
  std::string ret = str;
  ::kodi::addon::CAddonBase::m_interface->toKodi.free_string(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, str);
  return ret;
}
} /* namespace kodi */
//------------------------------------------------------------------------------

//==============================================================================
namespace kodi {
/// 
inline std::string GetBaseUserPath()
{
  char* str = ::kodi::addon::CAddonBase::m_interface->toKodi.get_base_user_path(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase);
  std::string ret = str;
  ::kodi::addon::CAddonBase::m_interface->toKodi.free_string(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, str);
  return ret;
}
} /* namespace kodi */
//------------------------------------------------------------------------------

//==============================================================================
namespace kodi {
/// 
inline std::string GetLibPath()
{
  return ::kodi::addon::CAddonBase::m_interface->libBasePath;
}
} /* namespace kodi */
//------------------------------------------------------------------------------

//==============================================================================
namespace kodi {
/// 
inline void Log(const AddonLog loglevel, const char* format, ...)
{
  char buffer[16384];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  ::kodi::addon::CAddonBase::m_interface->toKodi.addon_log_msg(::kodi::addon::CAddonBase::m_interface->toKodi.kodiBase, loglevel, buffer);
}
} /* namespace kodi */
//------------------------------------------------------------------------------



/*! addon creation macro
 * @todo cleanup this stupid big macro
 * This macro includes now all for add-on's needed functions. This becomes a bigger
 * rework after everything is done on Kodi itself, currently is this way needed
 * to have compatibility with not reworked interfaces.
 *
 * Becomes really cleaned up soon :D
 */
#define ADDONCREATOR(AddonClass) \
  extern "C" __declspec(dllexport) void get_addon(void* pAddon) {} \
  extern "C" __declspec(dllexport) ADDON_STATUS ADDON_Create(KODI_HANDLE addonInterface, void *unused) \
  { \
    kodi::addon::CAddonBase::m_interface = static_cast<AddonGlobalInterface*>(addonInterface); \
    kodi::addon::CAddonBase::m_interface->addonBase = new AddonClass; \
    return kodi::addon::CAddonBase::m_interface->addonBase->Create(); \
  } \
  extern "C" __declspec(dllexport) void ADDON_Destroy() \
  { \
    kodi::addon::CAddonBase::ADDONBASE_Destroy(); \
  } \
  extern "C" __declspec(dllexport) ADDON_STATUS ADDON_GetStatus() \
  { \
    return kodi::addon::CAddonBase::ADDONBASE_GetStatus(); \
  } \
  extern "C" __declspec(dllexport) ADDON_STATUS ADDON_SetSetting(const char *settingName, const void *settingValue) { return ADDON_STATUS_OK; } \
  extern "C" __declspec(dllexport) const char* ADDON_GetTypeVersion(int type) \
  { \
    return kodi::addon::GetTypeVersion(type); \
  } \
  extern "C" __declspec(dllexport) ADDON_STATUS ADDON_CreateInstance(int instanceType, const char* instanceID, KODI_HANDLE instance, KODI_HANDLE* addonInstance, ::kodi::addon::IAddonInstance* parent) \
  { \
    return kodi::addon::CAddonBase::ADDONBASE_CreateInstance(instanceType, instanceID, instance, addonInstance, parent); \
  } \
  extern "C" __declspec(dllexport) void ADDON_DestroyInstance(int instanceType, KODI_HANDLE instance) \
  { \
    kodi::addon::CAddonBase::ADDONBASE_DestroyInstance(instanceType, instance); \
  } \
  AddonGlobalInterface* kodi::addon::CAddonBase::m_interface = nullptr;
