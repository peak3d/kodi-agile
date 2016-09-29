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

enum ADDON_STATUS
{
  ADDON_STATUS_OK,
  ADDON_STATUS_LOST_CONNECTION,
  ADDON_STATUS_NEED_RESTART,
  ADDON_STATUS_NEED_SETTINGS,
  ADDON_STATUS_UNKNOWN,
  ADDON_STATUS_NEED_SAVEDSETTINGS,
  ADDON_STATUS_PERMANENT_FAILURE,   /**< permanent failure, like failing to resolve methods */
  ADDON_STATUS_NOT_IMPLEMENTED
};

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

// To have a on add-on and kodi itself handled string always on known size!
#define ADDON_STANDARD_STRING_LENGTH 1024
#define ADDON_STANDARD_STRING_LENGTH_SMALL 256

typedef enum eAudioDataFormat
{
  AUDIO_FMT_INVALID = -1,

  AUDIO_FMT_U8,

  AUDIO_FMT_S16BE,
  AUDIO_FMT_S16LE,
  AUDIO_FMT_S16NE,

  AUDIO_FMT_S32BE,
  AUDIO_FMT_S32LE,
  AUDIO_FMT_S32NE,

  AUDIO_FMT_S24BE4,
  AUDIO_FMT_S24LE4,
  AUDIO_FMT_S24NE4,    // 24 bits in lower 3 bytes
  AUDIO_FMT_S24NE4MSB, // S32 with bits_per_sample < 32

  AUDIO_FMT_S24BE3,
  AUDIO_FMT_S24LE3,
  AUDIO_FMT_S24NE3, /* S24 in 3 bytes */

  AUDIO_FMT_DOUBLE,
  AUDIO_FMT_FLOAT,

  // Bitstream
  AUDIO_FMT_RAW,

  /* planar formats */
  AUDIO_FMT_U8P,
  AUDIO_FMT_S16NEP,
  AUDIO_FMT_S32NEP,
  AUDIO_FMT_S24NE4P,
  AUDIO_FMT_S24NE4MSBP,
  AUDIO_FMT_S24NE3P,
  AUDIO_FMT_DOUBLEP,
  AUDIO_FMT_FLOATP,

  AUDIO_FMT_MAX
} eAudioDataFormat;

typedef enum eAudioChannel
{
  AUDIO_CH_NULL = -1,
  AUDIO_CH_RAW ,

  AUDIO_CH_FL  , AUDIO_CH_FR , AUDIO_CH_FC , AUDIO_CH_LFE, AUDIO_CH_BL  , AUDIO_CH_BR  , AUDIO_CH_FLOC,
  AUDIO_CH_FROC, AUDIO_CH_BC , AUDIO_CH_SL , AUDIO_CH_SR , AUDIO_CH_TFL , AUDIO_CH_TFR , AUDIO_CH_TFC ,
  AUDIO_CH_TC  , AUDIO_CH_TBL, AUDIO_CH_TBR, AUDIO_CH_TBC, AUDIO_CH_BLOC, AUDIO_CH_BROC,

  /* p16v devices */
  AUDIO_CH_UNKNOWN1 , AUDIO_CH_UNKNOWN2 , AUDIO_CH_UNKNOWN3 , AUDIO_CH_UNKNOWN4 ,
  AUDIO_CH_UNKNOWN5 , AUDIO_CH_UNKNOWN6 , AUDIO_CH_UNKNOWN7 , AUDIO_CH_UNKNOWN8 ,
  AUDIO_CH_UNKNOWN9 , AUDIO_CH_UNKNOWN10, AUDIO_CH_UNKNOWN11, AUDIO_CH_UNKNOWN12,
  AUDIO_CH_UNKNOWN13, AUDIO_CH_UNKNOWN14, AUDIO_CH_UNKNOWN15, AUDIO_CH_UNKNOWN16,
  AUDIO_CH_UNKNOWN17, AUDIO_CH_UNKNOWN18, AUDIO_CH_UNKNOWN19, AUDIO_CH_UNKNOWN20,
  AUDIO_CH_UNKNOWN21, AUDIO_CH_UNKNOWN22, AUDIO_CH_UNKNOWN23, AUDIO_CH_UNKNOWN24,
  AUDIO_CH_UNKNOWN25, AUDIO_CH_UNKNOWN26, AUDIO_CH_UNKNOWN27, AUDIO_CH_UNKNOWN28,
  AUDIO_CH_UNKNOWN29, AUDIO_CH_UNKNOWN30, AUDIO_CH_UNKNOWN31, AUDIO_CH_UNKNOWN32,
  AUDIO_CH_UNKNOWN33, AUDIO_CH_UNKNOWN34, AUDIO_CH_UNKNOWN35, AUDIO_CH_UNKNOWN36,
  AUDIO_CH_UNKNOWN37, AUDIO_CH_UNKNOWN38, AUDIO_CH_UNKNOWN39, AUDIO_CH_UNKNOWN40,
  AUDIO_CH_UNKNOWN41, AUDIO_CH_UNKNOWN42, AUDIO_CH_UNKNOWN43, AUDIO_CH_UNKNOWN44,
  AUDIO_CH_UNKNOWN45, AUDIO_CH_UNKNOWN46, AUDIO_CH_UNKNOWN47, AUDIO_CH_UNKNOWN48,
  AUDIO_CH_UNKNOWN49, AUDIO_CH_UNKNOWN50, AUDIO_CH_UNKNOWN51, AUDIO_CH_UNKNOWN52,
  AUDIO_CH_UNKNOWN53, AUDIO_CH_UNKNOWN54, AUDIO_CH_UNKNOWN55, AUDIO_CH_UNKNOWN56,
  AUDIO_CH_UNKNOWN57, AUDIO_CH_UNKNOWN58, AUDIO_CH_UNKNOWN59, AUDIO_CH_UNKNOWN60,
  AUDIO_CH_UNKNOWN61, AUDIO_CH_UNKNOWN62, AUDIO_CH_UNKNOWN63, AUDIO_CH_UNKNOWN64,

  AUDIO_CH_MAX
} eAudioChannel;

typedef struct sAddonToKodiFuncTable_Addon
{
  void* kodiInstance;
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
