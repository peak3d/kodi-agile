#pragma once
/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "AddonDll.h"
#include "addons/kodi-addon-dev-kit/include/kodi/addon/visualization/Addon.h"
#include "cores/AudioEngine/Interfaces/IAudioCallback.h"
#include "guilib/IRenderingCallback.h"
#include "utils/rfft.h"

#include <algorithm>
#include <map>
#include <list>
#include <memory>
#include <vector>

#define AUDIO_BUFFER_SIZE 512 // MUST BE A POWER OF 2!!!
#define MAX_AUDIO_BUFFERS 16

class CCriticalSection;

class CAudioBuffer
{
public:
  CAudioBuffer(int iSize);
  virtual ~CAudioBuffer();
  const float* Get() const;
  void Set(const float* psBuffer, int iSize);
private:
  CAudioBuffer();
  float* m_pBuffer;
  int m_iLen;
};

namespace ADDON
{
  class CVisualisation : public CAddonDll
                       , public IAudioCallback
                       , public IRenderingCallback
  {
  public:
    explicit CVisualisation(AddonProps props);

    // Child functions related to IAudioCallback
    virtual void OnInitialize(int iChannels, int iSamplesPerSec, int iBitsPerSample) override;
    virtual void OnAudioData(const float* pAudioData, int iAudioDataLength) override;

    // Child functions related to CAddonDll
    virtual bool IsInUse() const override;

    // Child functions related to IRenderingCallback
    virtual bool Create(int x, int y, int w, int h, void *device) override;
    virtual void Render() override;
    virtual void Stop() override; // also child of CAddonDll
    
    void Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, const std::string &strSongName) ;
    void AudioData(const float *pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength);
    void GetInfo(VIS_INFO *info);
    bool OnAction(VIS_ACTION action, void *param = NULL);
    bool UpdateTrack();
    bool HasPresets() { return m_hasPresets; };
    bool HasSubModules() { return !m_submodules.empty(); }
    bool IsLocked();
    unsigned GetPreset();
    std::string GetPresetName();
    bool GetPresetList(std::vector<std::string>& vecpresets);
    bool GetSubModuleList(std::vector<std::string>& vecmodules);
    static std::string GetFriendlyName(const std::string& vis, const std::string& module);
    void Destroy();

    // Static function to transfer data from add-on to kodi
    static void TransferPreset(void* kodiInstance, const char* preset);
    static void TransferSubmodule(void* kodiInstance, const char* submodule);
  
  private:
    void CreateBuffers();
    void ClearBuffers();

    bool GetPresets();
    bool GetSubModules();

    void ExceptionHandle(std::exception& ex, const char* function);

    // cached preset list
    std::vector<std::string> m_presets;
    // cached submodule list
    std::vector<std::string> m_submodules;

    // audio properties
    int m_iChannels;
    int m_iSamplesPerSec;
    int m_iBitsPerSample;
    std::list<CAudioBuffer*> m_vecBuffers;
    int m_iNumBuffers;        // Number of Audio buffers
    bool m_bWantsFreq;
    float m_fFreq[AUDIO_BUFFER_SIZE];         // Frequency data
    bool m_hasPresets;
    std::unique_ptr<RFFT> m_transform;

    // track information
    std::string m_AlbumThumb;

    sAddonInstance_Visualization m_struct;
    void* m_addonInstance;
  };
}
