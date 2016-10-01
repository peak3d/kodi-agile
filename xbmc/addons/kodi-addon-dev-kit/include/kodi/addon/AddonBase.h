  #pragma once

/*
 *      Copyright (C) 2005-2015 Team Kodi
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
  
#ifdef __cplusplus
extern "C" {
#endif

typedef void* KODI_HANDLE;

typedef enum ADDON_INSTANCE_TYPE
{
  ADDON_INSTANCE_ADSP = 1,
  ADDON_INSTANCE_AUDIODECODER = 2,
  ADDON_INSTANCE_AUDIOENCODER = 3,
  ADDON_INSTANCE_INPUTSTREAM = 4,
  ADDON_INSTANCE_PERIPHERAL = 5,
  ADDON_INSTANCE_PVR = 6,
  ADDON_INSTANCE_SCREENSAVER = 7,
  ADDON_INSTANCE_VISUALIZATION = 8,
} ADDON_INSTANCE_TYPE;

typedef enum ADDON_STATUS
{
  ADDON_STATUS_OK,
  ADDON_STATUS_LOST_CONNECTION,
  ADDON_STATUS_NEED_RESTART,
  ADDON_STATUS_NEED_SETTINGS,
  ADDON_STATUS_UNKNOWN,
  ADDON_STATUS_NEED_SAVEDSETTINGS,
  ADDON_STATUS_PERMANENT_FAILURE,   /**< permanent failure, like failing to resolve methods */
  ADDON_STATUS_NOT_IMPLEMENTED
} ADDON_STATUS;

typedef struct ADDON_StructSetting
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

// To have a on add-on and kodi itself handled string always on known size!
#define ADDON_STANDARD_STRING_LENGTH 1024
#define ADDON_STANDARD_STRING_LENGTH_SMALL 256

struct sAddonToKodiFuncTable_kodi;
struct sAddonToKodiFuncTable_kodi_network;
struct sAddonToKodiFuncTable_kodi_audio;
struct sAddonToKodiFuncTable_kodi_codec;
struct sAddonToKodiFuncTable_kodi_vfs;
struct sAddonToKodiFuncTable_kodi_audioengine;

typedef struct sAddonToKodiFuncTable_Addon
{
  void* kodiInstance;
  void (*Log)(void* kodiInstance, const int loglevel, const char *msg);
  void (*free_string)(void* kodiInstance, char* str);
  sAddonToKodiFuncTable_kodi* kodi;
  sAddonToKodiFuncTable_kodi_network* kodi_network;
  sAddonToKodiFuncTable_kodi_audio* kodi_audio;
  sAddonToKodiFuncTable_kodi_codec* kodi_codec;
  sAddonToKodiFuncTable_kodi_vfs* kodi_vfs;
  sAddonToKodiFuncTable_kodi_audioengine* kodi_audioengine;
} sAddonToKodiFuncTable_Addon;

typedef struct sKodiToAddonFuncTable_Addon
{
  void (__cdecl* Destroy)();
  void (__cdecl* Stop)();
  ADDON_STATUS (__cdecl* GetStatus)();
  bool (__cdecl* HasSettings)();
  unsigned int (__cdecl* GetSettings)(ADDON_StructSetting ***sSet);
  ADDON_STATUS (__cdecl* SetSetting)(const char *settingName, const void *settingValue);
  void (__cdecl* FreeSettings)(unsigned int elements, ADDON_StructSetting*** set);
  ADDON_STATUS (__cdecl* CreateInstance)(int instanceType, const char* instanceID, void* instance, void** addonInstance);
  void (__cdecl* DestroyInstance)(int instanceType, void* instance);
} sKodiToAddonFuncTable_Addon;

typedef struct sFuncTable_Addon
{
  const char* libBasePath;  ///< Never, never change this!!!
  sAddonToKodiFuncTable_Addon toKodi;
  sKodiToAddonFuncTable_Addon toAddon;
} sFuncTable_Addon;

#ifdef __cplusplus
namespace kodi {
namespace addon {
  
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  //
  class IAddonInstance
  {
  public:
    IAddonInstance(ADDON_INSTANCE_TYPE type) : m_type(type) {}
    virtual ~IAddonInstance() { }

    const ADDON_INSTANCE_TYPE m_type;
  };
  //
  //=-----=------=------=------=------=------=------=------=------=------=-----=


  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Add-on settings handle class
  //
  class CAddonSetting
  {
  public:
    enum SETTING_TYPE { NONE=0, CHECK, SPIN };

    CAddonSetting(SETTING_TYPE type, std::string id, std::string label)
      : Type(type),
        Id(id),
        Label(label),
        Current(0)
    {
    }

    CAddonSetting(const CAddonSetting &rhs) // copy constructor
    {
      Id = rhs.Id;
      Label = rhs.Label;
      Current = rhs.Current;
      Type = rhs.Type;
      for (unsigned int i = 0; i < rhs.Entries.size(); ++i)
        Entries.push_back(rhs.Entries[i]);
    }

    void AddEntry(std::string label)
    {
      if (Label.empty() || Type != SPIN)
        return;
      Entries.push_back(Label);
    }

    // data members
    SETTING_TYPE Type;
    std::string Id;
    std::string Label;
    int Current;
    std::vector<std::string> Entries;
  };
  //
  //=-----=------=------=------=------=------=------=------=------=------=-----=

  
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Function used on Kodi itself to transfer back from add-on given data with
  // "ADDON_StructSetting***" to "std::vector<CAddonSetting>"
  //
  // Note: Not needed on add-on itself, only here to have all related parts on
  // same place!
  //
  static inline void StructToVec(unsigned int elements, ADDON_StructSetting*** sSet, std::vector<CAddonSetting> *vecSet)
  {
    if (elements == 0)
      return;
      
    vecSet->clear();
    for(unsigned int i = 0; i < elements; i++)
    {
      CAddonSetting vSet((CAddonSetting::SETTING_TYPE)(*sSet)[i]->type, (*sSet)[i]->id, (*sSet)[i]->label);
      if((*sSet)[i]->type == CAddonSetting::SPIN)
      {
        for(unsigned int j=0;j<(*sSet)[i]->entry_elements;j++)
        {
          vSet.AddEntry((*sSet)[i]->entry[j]);
        }
      }
      vSet.Current = (*sSet)[i]->current;
      vecSet->push_back(vSet);
    }
  }
  //
  //=-----=------=------=------=------=------=------=------=------=------=-----=


  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Add-on main instance class.
  //
  class CAddonBase
  {
  public:
    CAddonBase()
    {
      if (m_createdAddon != nullptr)
        throw std::logic_error("kodi::addon::CAddonBase class creation called independent from add-on interface!");

      m_instance->toAddon.Destroy = ADDONBASE_Destroy;
      m_instance->toAddon.Stop = ADDONBASE_Stop;
      m_instance->toAddon.GetStatus = ADDONBASE_GetStatus;
      m_instance->toAddon.HasSettings = ADDONBASE_HasSettings;
      m_instance->toAddon.GetSettings = ADDONBASE_GetSettings;
      m_instance->toAddon.SetSetting = ADDONBASE_SetSetting;
      m_instance->toAddon.FreeSettings = ADDONBASE_FreeSettings;
      m_instance->toAddon.CreateInstance = ADDONBASE_CreateInstance;
      m_instance->toAddon.DestroyInstance = ADDONBASE_DestroyInstance;
    }
    
    virtual ADDON_STATUS Create() { return ADDON_STATUS_OK; }
    virtual void Stop() { }
    virtual ADDON_STATUS GetStatus() { return ADDON_STATUS_OK; }

    virtual bool HasSettings() { return false; }
    virtual bool GetSettings(std::vector<CAddonSetting>& settings) { return false; }
    virtual ADDON_STATUS SetSetting(std::string& settingName, const void *settingValue) { return ADDON_STATUS_UNKNOWN; }
    
    virtual ADDON_STATUS CreateInstance(int instanceType, std::string instanceID, KODI_HANDLE instance, KODI_HANDLE& addonInstance) { return ADDON_STATUS_UNKNOWN; }
    
    static sFuncTable_Addon* m_instance;
    static CAddonBase* m_createdAddon;

  private:
    static inline void ADDONBASE_Destroy()
    {
      delete CAddonBase::m_createdAddon;
      CAddonBase::m_createdAddon = nullptr;
    }

    static inline void ADDONBASE_Stop() { CAddonBase::m_createdAddon->Stop(); }

    static inline ADDON_STATUS ADDONBASE_GetStatus() { return CAddonBase::m_createdAddon->GetStatus(); }

    static inline bool ADDONBASE_HasSettings() { return CAddonBase::m_createdAddon->HasSettings(); }

    static inline unsigned int ADDONBASE_GetSettings(ADDON_StructSetting ***sSet)
    {
      std::vector<CAddonSetting> settings;
      if (CAddonBase::m_createdAddon->GetSettings(settings))
      {
        *sSet = nullptr;
        if (settings.empty())
          return 0;

        *sSet = (ADDON_StructSetting**)malloc(settings.size()*sizeof(ADDON_StructSetting*));
        for (unsigned int i = 0;i < settings.size(); ++i)
        {
          (*sSet)[i] = nullptr;
          (*sSet)[i] = (ADDON_StructSetting*)malloc(sizeof(ADDON_StructSetting));
          (*sSet)[i]->id = nullptr;
          (*sSet)[i]->label = nullptr;

          if (!settings[i].Id.empty() && !settings[i].Label.empty())
          {
            (*sSet)[i]->id = strdup(settings[i].Id.c_str());
            (*sSet)[i]->label = strdup(settings[i].Label.c_str());
            (*sSet)[i]->type = settings[i].Type;
            (*sSet)[i]->current = settings[i].Current;
            (*sSet)[i]->entry_elements = 0;
            (*sSet)[i]->entry = nullptr;
            if (settings[i].Type == CAddonSetting::SPIN && !settings[i].Entries.empty())
            {
              (*sSet)[i]->entry = (char**)malloc(settings[i].Entries.size()*sizeof(char**));
              for (unsigned int j = 0; j < settings[i].Entries.size(); ++j)
              {
                if (!settings[i].Entries[j].empty())
                {
                  (*sSet)[i]->entry[j] = strdup(settings[i].Entries[j].c_str());
                  (*sSet)[i]->entry_elements++;
                }
              }
            }
          }
        }

        return settings.size();
      }
      return 0;
    }

    static inline ADDON_STATUS ADDONBASE_SetSetting(const char *settingName, const void *settingValue)
    {
      std::string name = settingName;
      ADDON_STATUS ret = CAddonBase::m_createdAddon->SetSetting(name, settingValue);
      std::strcpy((char*)settingName, name.c_str());
      return ret;
    }

    static inline void ADDONBASE_FreeSettings(unsigned int elements, ADDON_StructSetting*** set)
    {
      if (elements == 0)
        return;

      for (unsigned int i = 0; i < elements; ++i)
      {
        if ((*set)[i]->type == CAddonSetting::SPIN)
        {
          for (unsigned int j = 0; j < (*set)[i]->entry_elements; ++j)
          {
            free((*set)[i]->entry[j]);
          }
          free((*set)[i]->entry);
        }
        free((*set)[i]->id);
        free((*set)[i]->label);
        free((*set)[i]);
      }
      free(*set);
    }

    static inline ADDON_STATUS ADDONBASE_CreateInstance(int instanceType, const char* instanceID, KODI_HANDLE instance, KODI_HANDLE* addonInstance)
    {
      ADDON_STATUS status = CAddonBase::m_createdAddon->CreateInstance(instanceType, instanceID, instance, *addonInstance);
      if (*addonInstance == nullptr)
        throw std::logic_error("kodi::addon::CAddonBase CreateInstance returns a empty instance pointer!");
      if (static_cast<::kodi::addon::IAddonInstance*>(*addonInstance)->m_type != instanceType)
        throw std::logic_error("kodi::addon::CAddonBase CreateInstance with difference on given and returned instance type!");
      return status;
    }

    static inline void ADDONBASE_DestroyInstance(int instanceType, KODI_HANDLE instance)
    {
      if (instance != CAddonBase::m_createdAddon)
      {
        if (static_cast<::kodi::addon::IAddonInstance*>(instance)->m_type == instanceType)
          delete static_cast<::kodi::addon::IAddonInstance*>(instance);
        else
          throw std::logic_error("kodi::addon::CAddonBase DestroyInstance called with difference on given and present instance type!");
      }
    }
  };
  //
  //=-----=------=------=------=------=------=------=------=------=------=-----=

} /* namespace addon */
} /* namespace kodi */
} /* extern "C" */
#endif /* __cplusplus */

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Add-on creation macro
//
// Used to define the needed static values one time on add-on
//
#define ADDONCREATOR(AddonClass) \
  extern "C" __declspec(dllexport) ADDON_STATUS ADDON_Create(void* instance) \
  { \
    kodi::addon::CAddonBase::m_instance = static_cast<sFuncTable_Addon*>(instance); \
    kodi::addon::CAddonBase::m_createdAddon = new AddonClass; \
    return kodi::addon::CAddonBase::m_createdAddon->Create(); \
  } \
  sFuncTable_Addon* kodi::addon::CAddonBase::m_instance = nullptr; \
  kodi::addon::CAddonBase* kodi::addon::CAddonBase::m_createdAddon = nullptr;
//
//=------=------=------=------=------=------=------=------=------=------=------=


//==============================================================================
/// \ingroup cpp_kodi_Defs
/// @brief To identify used audio stream format
///
typedef enum AudioDataFormat
{
  /// Invalid audio format
  AUDIO_FMT_INVALID = -1,

  /// PCM unsigned 8-bit
  AUDIO_FMT_U8,
  
  /// PCM signed 16-bit big-endian
  AUDIO_FMT_S16BE,
  /// PCM signed 16-bit little-endian
  AUDIO_FMT_S16LE,
  /// PCM signed 16-bit native-endian
  AUDIO_FMT_S16NE,
  
  /// PCM signed 32-bit big-endian
  AUDIO_FMT_S32BE,
  /// PCM signed 32-bit little-endian
  AUDIO_FMT_S32LE,
  /// PCM signed 32-bit native-endian
  AUDIO_FMT_S32NE,

  /// PCM signed 24-bit big-endian in 4 bytes
  AUDIO_FMT_S24BE4,
  /// PCM signed 24-bit little-endian in 4 bytes
  AUDIO_FMT_S24LE4,
  /// PCM signed 24-bit native-endian in 4 bytes
  AUDIO_FMT_S24NE4,
  /// S32 with bits_per_sample < 32
  AUDIO_FMT_S24NE4MSB,

  /// PCM signed 24-bit big-endian in 3 bytes
  AUDIO_FMT_S24BE3,
  /// PCM signed 24-bit little-endian in 3 bytes
  AUDIO_FMT_S24LE3,
  /// PCM signed 24-bit native-endian in 3 bytes
  AUDIO_FMT_S24NE3,

  /// Double floating point
  AUDIO_FMT_DOUBLE,
  /// Floating point
  AUDIO_FMT_FLOAT,

  /// Bitstream
  AUDIO_FMT_RAW,

  /// PCM planar format unsigned 8-bit
  AUDIO_FMT_U8P,
  /// PCM planar format signed 16-bit native-endian
  AUDIO_FMT_S16NEP,
  /// PCM planar format signed 32-bit native-endian
  AUDIO_FMT_S32NEP,
  /// PCM planar format signed 24-bit native-endian in 4 bytes
  AUDIO_FMT_S24NE4P,
  /// PCM planar format 
  AUDIO_FMT_S24NE4MSBP,
  /// PCM planar format signed 24-bit native-endian in 3 bytes
  AUDIO_FMT_S24NE3P,
  /// PCM planar format double floating point
  AUDIO_FMT_DOUBLEP,
  /// PCM planar format floating point
  AUDIO_FMT_FLOATP,

  /// Max value of enum
  AUDIO_FMT_MAX
} AudioDataFormat;
//------------------------------------------------------------------------------

//==============================================================================
/// \ingroup cpp_kodi_Defs
/// @brief Used to identify on general streams the channel.
///
///
typedef enum AudioChannel
{
  /// To mark no channel used or selected
  AUDIO_CH_NULL = -1,
  /// Front left
  AUDIO_CH_FL = 0 ,
  /// Front right
  AUDIO_CH_FR = 1,
  /// Front center
  AUDIO_CH_FC = 2,
  /// LFE / Subwoofer
  AUDIO_CH_LFE = 3,
  /// Back left
  AUDIO_CH_BL = 4,
  /// Back right
  AUDIO_CH_BR = 5,
  /// Front left over center
  AUDIO_CH_FLOC = 6,
  /// Front right over center
  AUDIO_CH_FROC = 7,
  /// Back center
  AUDIO_CH_BC = 8,
  /// Side left
  AUDIO_CH_SL = 9,
  /// Side right
  AUDIO_CH_SR = 10,
  /// Top front left
  AUDIO_CH_TFL = 11,
  /// Top front right
  AUDIO_CH_TFR = 12,
  /// Top front center
  AUDIO_CH_TFC = 13,
  /// Top center
  AUDIO_CH_TC = 14,
  /// Top back left
  AUDIO_CH_TBL = 15,
  /// Top back right
  AUDIO_CH_TBR = 16,
  /// Top back center
  AUDIO_CH_TBC = 17,
  /// Back left over center
  AUDIO_CH_BLOC = 18,
  /// Back right over center
  AUDIO_CH_BROC = 19,
  /// Max value for audio DSP system
  /// @note Used to arrange arrays on the DSP system
  AUDIO_CH_DSP_MAX = 20,
  /// Raw audio stream
  AUDIO_CH_RAW = 20,
  
  /* p16v devices */
  AUDIO_CH_UNKNOWN1 ,
  AUDIO_CH_UNKNOWN2 ,
  AUDIO_CH_UNKNOWN3 ,
  AUDIO_CH_UNKNOWN4 ,
  AUDIO_CH_UNKNOWN5 ,
  AUDIO_CH_UNKNOWN6 ,
  AUDIO_CH_UNKNOWN7 ,
  AUDIO_CH_UNKNOWN8 ,
  AUDIO_CH_UNKNOWN9 ,
  AUDIO_CH_UNKNOWN10,
  AUDIO_CH_UNKNOWN11,
  AUDIO_CH_UNKNOWN12,
  AUDIO_CH_UNKNOWN13,
  AUDIO_CH_UNKNOWN14,
  AUDIO_CH_UNKNOWN15,
  AUDIO_CH_UNKNOWN16,
  AUDIO_CH_UNKNOWN17,
  AUDIO_CH_UNKNOWN18,
  AUDIO_CH_UNKNOWN19,
  AUDIO_CH_UNKNOWN20,
  AUDIO_CH_UNKNOWN21,
  AUDIO_CH_UNKNOWN22,
  AUDIO_CH_UNKNOWN23,
  AUDIO_CH_UNKNOWN24,
  AUDIO_CH_UNKNOWN25,
  AUDIO_CH_UNKNOWN26,
  AUDIO_CH_UNKNOWN27, 
  AUDIO_CH_UNKNOWN28,
  AUDIO_CH_UNKNOWN29,
  AUDIO_CH_UNKNOWN30,
  AUDIO_CH_UNKNOWN31,
  AUDIO_CH_UNKNOWN32,
  AUDIO_CH_UNKNOWN33,
  AUDIO_CH_UNKNOWN34,
  AUDIO_CH_UNKNOWN35,
  AUDIO_CH_UNKNOWN36,
  AUDIO_CH_UNKNOWN37,
  AUDIO_CH_UNKNOWN38,
  AUDIO_CH_UNKNOWN39,
  AUDIO_CH_UNKNOWN40,
  AUDIO_CH_UNKNOWN41,
  AUDIO_CH_UNKNOWN42,
  AUDIO_CH_UNKNOWN43,
  AUDIO_CH_UNKNOWN44,
  AUDIO_CH_UNKNOWN45,
  AUDIO_CH_UNKNOWN46,
  AUDIO_CH_UNKNOWN47,
  AUDIO_CH_UNKNOWN48,
  AUDIO_CH_UNKNOWN49,
  AUDIO_CH_UNKNOWN50,
  AUDIO_CH_UNKNOWN51,
  AUDIO_CH_UNKNOWN52,
  AUDIO_CH_UNKNOWN53,
  AUDIO_CH_UNKNOWN54,
  AUDIO_CH_UNKNOWN55,
  AUDIO_CH_UNKNOWN56,
  AUDIO_CH_UNKNOWN57, 
  AUDIO_CH_UNKNOWN58,
  AUDIO_CH_UNKNOWN59,
  AUDIO_CH_UNKNOWN60,
  AUDIO_CH_UNKNOWN61,
  AUDIO_CH_UNKNOWN62,
  AUDIO_CH_UNKNOWN63,
  AUDIO_CH_UNKNOWN64,

  /// Max value for general systems
  AUDIO_CH_MAX
} AudioChannel;
//------------------------------------------------------------------------------

//==============================================================================
/// \ingroup cpp_kodi_Defs
/// @brief Log file
///
/// At some point during your foray into Kodi, you will likely come up against
/// a problem that isn't made 100% clear from errors in the GUI. This is where
/// the log file comes into play. Kodi writes all sorts of useful stuff to its
/// log, which is why it should be included with every bug/problem report.
/// Don't be afraid of its contents! Often a quick glance through the log will
/// turn up a simple typo or missing file which you can easily fix on your own.
///
/// Each event is logged to one line of the Kodi log file which is formatted as
/// follows...
///
/// ~~~~~~~~~~~~~
/// [TIMESTAMP] T:[THREADID] M:[FREEMEM] [LEVEL]: [MESSAGE]
/// ~~~~~~~~~~~~~
///
/// | Identification name   | Description                                      |
/// |-----------------------|--------------------------------------------------|
/// | TIMESTAMP             | The wall time at which the event occurred.
/// | THREADID              | The thread identification number of the thread in which the event occurred.
/// | FREEMEM               | The amount of memory (in bytes) free at the time of the event.
/// | LEVEL                 | The severity of the event.
/// | MESSAGE               | A brief description and/or important information about the event.
///
typedef enum AddonLog
{
  /// @brief Log level "Debug"
  ///
  /// In  depth  informatio n about  the  status  of  Kodi.  This  information
  /// can  pretty  much only be  deciphered  by a developer or  long time Kodi
  /// power user.
  ///
  LOG_DEBUG = 0,

  /// @brief Log level "Info"
  ///
  /// Something  has happened. It's not  a problem, we just  thought you might
  /// want to know. Fairly excessive output that most people won't care about.
  ///
  LOG_INFO = 1,

  /// @brief Log level "Notice"
  ///
  /// Similar  to  INFO but  the average  Joe might  want to  know about these
  /// events. This level and above are logged by default.
  ///
  LOG_NOTICE = 2,

  /// @brief Log level "Warning"
  ///
  /// Something potentially bad has happened. If Kodi did something you didn't
  /// expect, this is probably why. Watch for errors to follow.
  ///
  LOG_WARNING = 3,

  /// @brief Log level "Error"
  ///
  /// This event is bad.  Something has failed.  You  likely noticed  problems
  /// with the application be it skin artifacts, failure of playback a crash,
  /// etc.
  ///
  LOG_ERROR = 4,

  /// @brief Log level "Severe"
  ///
  LOG_SEVERE = 5,

  /// @brief Log level "Fatal"
  ///
  /// We're screwed. Kodi's add-on is about to crash.
  ///
  LOG_FATAL = 6
} AddonLog;
//------------------------------------------------------------------------------

#ifdef __cplusplus
namespace kodi {
//==============================================================================
/// \ingroup cpp_kodi
/// @brief Add a message to [KODI's log](http://kodi.wiki/view/Log_file/Advanced#Log_levels).
///
/// @param[in] loglevel     The log level of the message
///  |  enum code:  | Description:          |
///  |-------------:|-----------------------|
///  |  LOG_DEBUG   | In depth information about the status of Kodi. This information can pretty much only be deciphered by a developer or long time Kodi power user.
///  |  LOG_INFO    | Something has happened. It's not a problem, we just thought you might want to know. Fairly excessive output that most people won't care about.
///  |  LOG_NOTICE  | Similar to INFO but the average Joe might want to know about these events. This level and above are logged by default.
///  |  LOG_WARNING | Something potentially bad has happened. If Kodi did something you didn't expect, this is probably why. Watch for errors to follow.
///  |  LOG_ERROR   | This event is bad. Something has failed. You likely noticed problems with the application be it skin artifacts, failure of playback a crash, etc.
///  |  LOG_FATAL   | We're screwed. Kodi is about to crash.
/// @param[in] format       The format of the message to pass to KODI.
/// @param[in] ...          Added string values
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/addon/AddonBase.h>
///
/// ...
/// kodi::Log(LOG_FATAL, "Oh my goddess, I'm so fatal ;)");
/// ...
/// ~~~~~~~~~~~~~
///
inline void Log(const AddonLog loglevel, const char* format, ...)
{
  char buffer[16384];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  ::kodi::addon::CAddonBase::m_instance->toKodi.Log(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, loglevel, buffer);
}
//------------------------------------------------------------------------------
} /* namespace kodi */
#endif /* __cplusplus */
