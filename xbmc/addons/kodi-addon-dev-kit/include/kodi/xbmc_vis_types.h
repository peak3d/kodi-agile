#pragma once
/*
 *      Copyright (C) 2005-2016 Team Kodi
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

/*
  Common data structures shared between Kodi and Kodi's visualisations
 */

#include "xbmc_addon_types.h"

#include <cstddef>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VIS_INFO
{
  int bWantsFreq;
  int iSyncDelay;
} VIS_INFO;

typedef struct VIS_PROPS
{
  void *device;
  int x;
  int y;
  int width;
  int height;
  float pixelRatio;
  const char *name;
  const char *presets;
  const char *profile;
  const char *submodule;
} VIS_PROPS;

typedef enum VIS_ACTION
{ 
  VIS_ACTION_NONE = 0,
  VIS_ACTION_NEXT_PRESET,
  VIS_ACTION_PREV_PRESET,
  VIS_ACTION_LOAD_PRESET,
  VIS_ACTION_RANDOM_PRESET,
  VIS_ACTION_LOCK_PRESET,
  VIS_ACTION_RATE_PRESET_PLUS,
  VIS_ACTION_RATE_PRESET_MINUS,
  VIS_ACTION_UPDATE_ALBUMART,
  VIS_ACTION_UPDATE_TRACK
} VIS_ACTION;

typedef struct sAddonToKodiFuncTable_Visualization
{
  void* kodiInstance;
  void (__cdecl* TransferPreset) (void* kodiInstance, const char* preset);
  void (__cdecl* TransferSubmodule) (void* kodiInstance, const char* submodule);
} sAddonToKodiFuncTable_Visualization;

typedef struct sKodiToAddonFuncTable_Visualization
{
  void (__cdecl* Start)(void* addonInstance, int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName);
  void (__cdecl* AudioData)(void* addonInstance, const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength);
  void (__cdecl* Render) (void* addonInstance);
  void (__cdecl* GetInfo)(void* addonInstance, VIS_INFO *info);
  bool (__cdecl* OnAction)(void* addonInstance, VIS_ACTION action, const void *param);
  int (__cdecl* HasPresets)(void* addonInstance);
  unsigned int (__cdecl *GetPresets)(void* addonInstance);
  unsigned int (__cdecl *GetPreset)(void* addonInstance);
  unsigned int (__cdecl *GetSubModules)(void* addonInstance);
  bool (__cdecl* IsLocked)(void* addonInstance);
} sKodiToAddonFuncTable_Visualization;

typedef struct sAddonInstance_Visualization
{
  VIS_PROPS props;
  sAddonToKodiFuncTable_Visualization toKodi;
  sKodiToAddonFuncTable_Visualization toAddon;
} sAddonInstance_Visualization;

#ifdef __cplusplus
namespace kodi {
namespace addon {
namespace visualization {

  class VisTrack
  {
  public:
    VisTrack()
    {
      title = artist = album = albumArtist = nullptr;
      genre = comment = lyrics = reserved1 = reserved2 = nullptr;
      trackNumber = discNumber = duration = year = 0;
      rating = 0;
      reserved3 = reserved4 = 0;
    }

    const char *title;
    const char *artist;
    const char *album;
    const char *albumArtist;
    const char *genre;
    const char *comment;
    const char *lyrics;
    const char *reserved1;
    const char *reserved2;

    int trackNumber;
    int discNumber;
    int duration;
    int year;
    char rating;
    int reserved3;
    int reserved4;
  };

  class CAddon
  {
  public:
    CAddon(void* instance)
      : m_instance(static_cast<sAddonInstance_Visualization*>(instance))
    {
       m_instance->toAddon.Start = ADDON_Start;
       m_instance->toAddon.AudioData = ADDON_AudioData;
       m_instance->toAddon.Render = ADDON_Render;
       m_instance->toAddon.GetInfo = ADDON_GetInfo;
       m_instance->toAddon.OnAction = ADDON_OnAction;
       m_instance->toAddon.HasPresets = ADDON_HasPresets;
       m_instance->toAddon.GetPresets = ADDON_GetPresets;
       m_instance->toAddon.GetPreset = ADDON_GetPreset;
       m_instance->toAddon.GetSubModules = ADDON_GetSubModules;
       m_instance->toAddon.IsLocked = ADDON_IsLocked;
    }

    inline void* Device() { return m_instance->props.device; }
    inline int X() { return m_instance->props.x; }
    inline int Y() { return m_instance->props.y; }
    inline int Width() { return m_instance->props.width; }
    inline int Height() { return m_instance->props.height; }
    inline float PixelRatio() { return m_instance->props.pixelRatio; }
    inline std::string Name() { return m_instance->props.name; }
    inline std::string Presets() { return m_instance->props.presets; }
    inline std::string Profile() { return m_instance->props.profile; }
    inline std::string SubModule() { return m_instance->props.submodule; }

    virtual void Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, std::string szSongName) { }
    virtual void Stop() { }
    virtual void AudioData(const float* pAudioData, int iAudioDataLength, float* pFreqData, int iFreqDataLength) { }
    virtual void Render() { }
    virtual void GetInfo(VIS_INFO& info) { info.bWantsFreq = false; info.iSyncDelay = 0; }
    virtual bool OnAction(VIS_ACTION action, const void* param) { return false; }
    virtual int HasPresets() { return 0; }
    virtual bool GetPresets(std::vector<std::string>& presets) { return 0; }
    virtual unsigned int GetPreset() { return 0; }
    virtual bool GetSubModules(std::vector<std::string>& submodule) { return 0; }
    virtual bool IsLocked() { return false; }

  private:
    inline static void ADDON_Start(void* addonInstance, int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName)
    {
      static_cast<CAddon*>(addonInstance)->Start(iChannels, iSamplesPerSec, iBitsPerSample, szSongName);
    }

    inline static void ADDON_AudioData(void* addonInstance, const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength)
    {
      static_cast<CAddon*>(addonInstance)->AudioData(pAudioData, iAudioDataLength, pFreqData, iFreqDataLength);
    }

    inline static void ADDON_Render(void* addonInstance)
    {
      static_cast<CAddon*>(addonInstance)->Render();
    }
    
    inline static void ADDON_GetInfo(void* addonInstance, VIS_INFO *info)
    {
      static_cast<CAddon*>(addonInstance)->GetInfo(*info);
    }
    
    inline static bool ADDON_OnAction(void* addonInstance, VIS_ACTION action, const void *param)
    {
      return static_cast<CAddon*>(addonInstance)->OnAction(action, param);
    }

    inline static int ADDON_HasPresets(void* addonInstance)
    {
      return static_cast<CAddon*>(addonInstance)->HasPresets();
    }

    inline static unsigned int ADDON_GetPresets(void* addonInstance)
    {
      CAddon* addon = static_cast<CAddon*>(addonInstance);
      std::vector<std::string> presets;
      if (addon->GetPresets(presets))
      {
        for (auto it : presets)
          addon->m_instance->toKodi.TransferPreset(addon->m_instance->toKodi.kodiInstance, it.c_str());          
      }

      return presets.size();
    }

    inline static unsigned int ADDON_GetPreset(void* addonInstance)
    {
      return static_cast<CAddon*>(addonInstance)->GetPreset();
    }

    inline static unsigned int ADDON_GetSubModules(void* addonInstance)
    {
      CAddon* addon = static_cast<CAddon*>(addonInstance);
      std::vector<std::string> subModules;
      if (addon->GetSubModules(subModules))
      {
        for (auto it : subModules)
          addon->m_instance->toKodi.TransferSubmodule(addon->m_instance->toKodi.kodiInstance, it.c_str());          
      }

      return subModules.size();
    }

    inline static bool ADDON_IsLocked(void* addonInstance)
    {
      return static_cast<CAddon*>(addonInstance)->IsLocked();
    }

    sAddonInstance_Visualization* m_instance;
  };

} /* namespace visualization */
} /* namespace addon */
} /* namespace kodi */
} /* extern "C" */
#endif
