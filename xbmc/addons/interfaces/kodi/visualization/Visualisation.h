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

#include "addons/AddonDll.h"
#include "addons/kodi-addon-dev-kit/include/kodi/visualization/Visualization.h"
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
  class CVisualisation : public IAudioCallback
                       , public IRenderingCallback
  {
  public:
    explicit CVisualisation(ADDON::AddonDllPtr addon);
    virtual ~CVisualisation() { }

    std::string Name() const;

    // Child functions related to IAudioCallback
    virtual void OnInitialize(int iChannels, int iSamplesPerSec, int iBitsPerSample) override;
    virtual void OnAudioData(const float* pAudioData, int iAudioDataLength) override;

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
    bool IsLocked();
    unsigned GetPreset();
    std::string GetPresetName();
    bool GetPresetList(std::vector<std::string>& vecpresets);
    void Destroy();

    // Static function to transfer data from add-on to kodi
    static void TransferPreset(void* kodiInstance, const char* preset);

  private:
    void CreateBuffers();
    void ClearBuffers();

    bool GetPresets();

    // cached preset list
    std::vector<std::string> m_presets;

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

    /*!
    * @{
    * @brief Functions below are used to handle exception between Kodi and his
    * add-on.
    *
    * ExceptionStdHandle(...):
    *   Used for to handle standard c++ exception partly generated from dev kits
    *   headers.
    *
    * ExceptionErrHandle(...):
    *   This is a special type basically used also in dev kit headers to give a
    *   exception with his ADDON_STATUS as integer value.
    *   Can be generated from headers by a massive fault detected on call of
    *   them.
    *
    * ExceptionUnkHandle(...);
    *   Used for everything else.
    *
    * With a call of them becomes the standardized function
    * Exception::LogUnkException(...) from ExceptionHandling.h used to write
    * log entry. This is always the first call, to see if it still crashes the
    * source of them.
    *
    * After them becomes the visualization add-on Destroyed and complete disabled.
    *
    * As last step comes a dialog to inform the used about the Exception.
    *
    * How it is set on add-on calls
    * ~~~~~~~~~~~~~{.cpp}
    * try
    * {
    *   ...
    * }
    * catch (std::exception& ex) { ExceptionStdHandle(ex, __FUNCTION__); }
    * catch (int ex)             { ExceptionErrHandle(ex, __FUNCTION__); }
    * catch (...)                { ExceptionUnkHandle(__FUNCTION__); }
    * ~~~~~~~~~~~~~
    *
    * @note this way becomes also used on add-on typed e.g. pvr ...
    */
    void ExceptionStdHandle(std::exception& ex, const char* function);
    void ExceptionErrHandle(int ex, const char* function);
    void ExceptionUnkHandle(const char* function);
    /* @}*/

    /*! The on add-on header defined interface function table */
    sAddonInstance_Visualization m_struct;

    /*! Used add-on class of the screensaver */
    ADDON::AddonDllPtr m_addon;

    /*! The instance pointer from add-on itself, who becomes passed back on all
    *  calls to them and set by the CAddonDll::CreateInstance(...) call */
    void* m_addonInstance;
  };
}
