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

#include "Visualisation.h"

#include "Application.h"
#include "GUIInfoManager.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "cores/AudioEngine/AEFactory.h"
#include "guiinfo/GUIInfoLabels.h"
#include "guilib/GraphicContext.h"
#include "guilib/WindowIDs.h"
#include "music/tags/MusicInfoTag.h"
#include "settings/AdvancedSettings.h"
#include "settings/Settings.h"
#include "utils/URIUtils.h"
#include "utils/StringUtils.h"
#include "windowing/WindowingFactory.h"

using namespace MUSIC_INFO;
using namespace ADDON;

CAudioBuffer::CAudioBuffer(int iSize)
{
  m_iLen = iSize;
  m_pBuffer = new float[iSize];
}

CAudioBuffer::~CAudioBuffer()
{
  delete [] m_pBuffer;
}

const float* CAudioBuffer::Get() const
{
  return m_pBuffer;
}

void CAudioBuffer::Set(const float* psBuffer, int iSize)
{
  if (iSize < 0)
    return;

  memcpy(m_pBuffer, psBuffer, iSize * sizeof(float));
  for (int i = iSize; i < m_iLen; ++i)
    m_pBuffer[i] = 0;
}

CVisualisation::CVisualisation(AddonProps props)
  : CAddonDll(std::move(props)),
    m_addonInstance(nullptr)
{
  m_props.name = nullptr;
  m_props.presets = nullptr;
  m_props.profile = nullptr;
  m_props.submodule = nullptr;
  memset(&m_struct, 0, sizeof(m_struct));
}
    
bool CVisualisation::Create(int x, int y, int w, int h, void *device)
{
  if (CAddonDll::Create() != ADDON_STATUS_OK)
    return false;

#ifdef HAS_DX
  m_props.device = g_Windowing.Get3D11Context();
#else
  m_props.device = nullptr;
#endif
  m_props.x = x;
  m_props.y = y;
  m_props.width = w;
  m_props.height = h;
  m_props.pixelRatio = g_graphicsContext.GetResInfo().fPixelRatio;

  m_props.name = strdup(Name().c_str());
  m_props.presets = strdup(CSpecialProtocol::TranslatePath(Path()).c_str());
  m_props.profile = strdup(CSpecialProtocol::TranslatePath(Profile()).c_str());
  m_props.submodule = nullptr;
  
  ADDON_STATUS status = CAddonDll::CreateInstance(ADDON_INSTANCE_VISUALIZATION, ID().c_str(), &m_struct, &m_addonInstance);
  if (status != ADDON_STATUS_OK)
    return false;
  
  // Start the visualisation
  std::string strFile = URIUtils::GetFileName(g_application.CurrentFile());
  CLog::Log(LOGDEBUG, "Visualisation::Start()\n");
  try
  {
    m_struct.Start(m_addonInstance, m_iChannels, m_iSamplesPerSec, m_iBitsPerSample, strFile.c_str());
  }
  catch (std::exception ex)
  {
    ADDON::LogException(this, ex, __FUNCTION__); // Handle exception
    memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
    return false;
  }

  m_hasPresets = GetPresets();

  if (GetSubModules())
    m_props.submodule = strdup(CSpecialProtocol::TranslatePath(m_submodules.front()).c_str());
  else
    m_props.submodule = nullptr;

  CreateBuffers();

  CAEFactory::RegisterAudioCallback(this);

  return true;
}

void CVisualisation::Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, const std::string &strSongName)
{
  // notify visz. that new song has been started
  // pass it the nr of audio channels, sample rate, bits/sample and offcourse the songname
  if (Initialized())
  {
    try
    {
      if (m_struct.Start)
        m_struct.Start(m_addonInstance, iChannels, iSamplesPerSec, iBitsPerSample, strSongName.c_str());
    }
    catch (std::exception ex)
    {
      ADDON::LogException(this, ex, __FUNCTION__); // Handle exception
      memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
    }
  }
}

void CVisualisation::AudioData(const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength)
{
  // pass audio data to visz.
  // audio data: is short audiodata [channel][iAudioDataLength] containing the raw audio data
  // iAudioDataLength = length of audiodata array
  // pFreqData = fft-ed audio data
  // iFreqDataLength = length of pFreqData
  if (Initialized())
  {
    try
    {
      if (m_struct.AudioData)
        m_struct.AudioData(m_addonInstance, pAudioData, iAudioDataLength, pFreqData, iFreqDataLength);
    }
    catch (std::exception ex)
    {
      ADDON::LogException(this, ex, __FUNCTION__); // Handle exception
      memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
    }
  }
}

void CVisualisation::Render()
{
  // ask visz. to render itself
  if (Initialized())
  {
    try
    {
      if (m_struct.Render)
        m_struct.Render(m_addonInstance);
    }
    catch (std::exception ex)
    {
      ADDON::LogException(this, ex, __FUNCTION__); // Handle exception
      memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
    }
  }
}

void CVisualisation::Stop()
{
  CAEFactory::UnregisterAudioCallback(this);
  if (Initialized())
  {
    CAddonDll::Stop();
  }
}

void CVisualisation::GetInfo(VIS_INFO *info)
{
  if (Initialized())
  {
    try
    {
      if (m_struct.GetInfo)
        m_struct.GetInfo(m_addonInstance, info);
    }
    catch (std::exception ex)
    {
      ADDON::LogException(this, ex, __FUNCTION__); // Handle exception
      memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
    }
  }
}

bool CVisualisation::OnAction(VIS_ACTION action, void *param)
{
  if (!Initialized())
    return false;

  // see if vis wants to handle the input
  // returns false if vis doesnt want the input
  // returns true if vis handled the input
  try
  {
    if (action != VIS_ACTION_NONE && m_struct.OnAction)
    {
      // if this is a VIS_ACTION_UPDATE_TRACK action, copy relevant
      // tags from CMusicInfoTag to VisTag
      if ( action == VIS_ACTION_UPDATE_TRACK && param )
      {
        const CMusicInfoTag* tag = (const CMusicInfoTag*)param;
        std::string artist(tag->GetArtistString());
        std::string albumArtist(tag->GetAlbumArtistString());
        std::string genre(StringUtils::Join(tag->GetGenre(), g_advancedSettings.m_musicItemSeparator));
        
        VisTrack track;
        track.title       = tag->GetTitle().c_str();
        track.artist      = artist.c_str();
        track.album       = tag->GetAlbum().c_str();
        track.albumArtist = albumArtist.c_str();
        track.genre       = genre.c_str();
        track.comment     = tag->GetComment().c_str();
        track.lyrics      = tag->GetLyrics().c_str();
        track.trackNumber = tag->GetTrackNumber();
        track.discNumber  = tag->GetDiscNumber();
        track.duration    = tag->GetDuration();
        track.year        = tag->GetYear();
        track.rating      = tag->GetUserrating();

        return m_struct.OnAction(m_addonInstance, action, &track);
      }
      return m_struct.OnAction(m_addonInstance, (int)action, param);
    }
  }
  catch (std::exception ex)
  {
    ADDON::LogException(this, ex, __FUNCTION__); // Handle exception
    memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
  }

  return false;
}

void CVisualisation::OnInitialize(int iChannels, int iSamplesPerSec, int iBitsPerSample)
{
  CLog::Log(LOGDEBUG, "OnInitialize() started");

  m_iChannels = iChannels;
  m_iSamplesPerSec = iSamplesPerSec;
  m_iBitsPerSample = iBitsPerSample;
  UpdateTrack();

  CLog::Log(LOGDEBUG, "OnInitialize() done");
}

void CVisualisation::OnAudioData(const float* pAudioData, int iAudioDataLength)
{
  // FIXME: iAudioDataLength should never be less than 0
  if (iAudioDataLength < 0)
    return;

  // Save our audio data in the buffers
  std::unique_ptr<CAudioBuffer> pBuffer ( new CAudioBuffer(iAudioDataLength) );
  pBuffer->Set(pAudioData, iAudioDataLength);
  m_vecBuffers.push_back( pBuffer.release() );

  if ((int)m_vecBuffers.size() < m_iNumBuffers)
    return;

  std::unique_ptr<CAudioBuffer> ptrAudioBuffer ( m_vecBuffers.front() );
  m_vecBuffers.pop_front();
  // Fourier transform the data if the vis wants it...
  if (m_bWantsFreq)
  {
    const float *psAudioData = ptrAudioBuffer->Get();

    if (!m_transform)
      m_transform.reset(new RFFT(AUDIO_BUFFER_SIZE/2, false)); // half due to stereo

    m_transform->calc(psAudioData, m_fFreq);

    // Transfer data to our visualisation
    AudioData(psAudioData, iAudioDataLength, m_fFreq, AUDIO_BUFFER_SIZE/2); // half due to complex-conjugate
  }
  else
  { // Transfer data to our visualisation
    AudioData(ptrAudioBuffer->Get(), iAudioDataLength, NULL, 0);
  }
}

void CVisualisation::CreateBuffers()
{
  ClearBuffers();

  VIS_INFO info;

  try
  {
    // Get the number of buffers from the current vis
    if (m_struct.GetInfo)
      m_struct.GetInfo(m_addonInstance, &info);
  }
  catch (std::exception ex)
  {
    ADDON::LogException(this, ex, __FUNCTION__); // Handle exception
    memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
    return;
  }

  m_iNumBuffers = info.iSyncDelay + 1;
  m_bWantsFreq = (info.bWantsFreq != 0);
  if (m_iNumBuffers > MAX_AUDIO_BUFFERS)
    m_iNumBuffers = MAX_AUDIO_BUFFERS;
  if (m_iNumBuffers < 1)
    m_iNumBuffers = 1;
}

void CVisualisation::ClearBuffers()
{
  m_bWantsFreq = false;
  m_iNumBuffers = 0;

  while (!m_vecBuffers.empty())
  {
    CAudioBuffer* pAudioBuffer = m_vecBuffers.front();
    delete pAudioBuffer;
    m_vecBuffers.pop_front();
  }
  for (int j = 0; j < AUDIO_BUFFER_SIZE; j++)
  {
    m_fFreq[j] = 0.0f;
  }
}

bool CVisualisation::UpdateTrack()
{
  bool handled = false;
  if (Initialized())
  {
    // get the current album art filename
    m_AlbumThumb = CSpecialProtocol::TranslatePath(g_infoManager.GetImage(MUSICPLAYER_COVER, WINDOW_INVALID));

    // get the current track tag
    const CMusicInfoTag* tag = g_infoManager.GetCurrentSongTag();

    if (m_AlbumThumb == "DefaultAlbumCover.png")
      m_AlbumThumb = "";
    else
      CLog::Log(LOGDEBUG,"Updating visualisation albumart: %s", m_AlbumThumb.c_str());

    // inform the visualisation of the current album art
    if (OnAction( VIS_ACTION_UPDATE_ALBUMART, (void*)( m_AlbumThumb.c_str() ) ) )
      handled = true;

    // inform the visualisation of the current track's tag information
    if ( tag && OnAction( VIS_ACTION_UPDATE_TRACK, (void*)tag ) )
      handled = true;
  }
  return handled;
}

bool CVisualisation::GetPresetList(std::vector<std::string> &vecpresets)
{
  vecpresets = m_presets;
  return !m_presets.empty();
}

bool CVisualisation::GetPresets()
{
  m_presets.clear();
  char **presets = nullptr;
  unsigned int entries = 0;
  try
  {
    if (m_struct.GetPresets)
      entries = m_struct.GetPresets(m_addonInstance, &presets);
  }
  catch (std::exception ex)
  {
    ADDON::LogException(this, ex, __FUNCTION__); // Handle exception
    memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
    return false;
  }

  if (presets && entries > 0)
  {
    for (unsigned i=0; i < entries; i++)
    {
      if (presets[i])
      {
        m_presets.push_back(presets[i]);
      }
    }
  }
  return (!m_presets.empty());
}

bool CVisualisation::GetSubModuleList(std::vector<std::string> &vecmodules)
{
  vecmodules = m_submodules;
  return !m_submodules.empty();
}

bool CVisualisation::GetSubModules()
{
  m_submodules.clear();
  char **modules = nullptr;
  unsigned int entries = 0;
  try
  {
    if (m_struct.GetSubModules)
      entries = m_struct.GetSubModules(m_addonInstance, &modules);
  }
  catch (std::exception& ex)
  {
    ADDON::LogException(this, ex, __FUNCTION__); // Handle exception
    memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
    return false;
  }

  if (modules && entries > 0)
  {
    for (unsigned i=0; i < entries; i++)
    {
      if (modules[i])
      {
        m_submodules.push_back(modules[i]);
      }
    }
  }
  return (!m_submodules.empty());
}

std::string CVisualisation::GetFriendlyName(const std::string& strVisz,
                                            const std::string& strSubModule)
{
  // should be of the format "moduleName (visName)"
  return strSubModule + " (" + strVisz + ")";
}

bool CVisualisation::IsLocked()
{
  if (!m_presets.empty())
  {
    try
    {
      if (m_struct.IsLocked)
        return m_struct.IsLocked(m_addonInstance);
    }
    catch (std::exception& ex)
    {
      ADDON::LogException(this, ex, __FUNCTION__); // Handle exception
      memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
    }
  }
  return false;
}

void CVisualisation::Destroy()
{
  // Free what was allocated in method CVisualisation::Create

  CAddonDll::DestroyInstance(ADDON_INSTANCE_VISUALIZATION, ID().c_str(), m_addonInstance);
  memset(&m_struct, 0, sizeof(m_struct));
  m_addonInstance = nullptr;

  if (m_props.name)
  {
    free((void *) m_props.name);
    m_props.name = nullptr;
  }
  if (m_props.presets)
  {
    free((void *) m_props.presets);
    m_props.presets = nullptr;
  }
  if (m_props.profile)
  {
    free((void *) m_props.profile);
    m_props.profile = nullptr;
  }
  if (m_props.submodule)
  {
    free((void *) m_props.submodule);
    m_props.submodule = nullptr;
  }

  CAddonDll::Destroy();
}

unsigned CVisualisation::GetPreset()
{
  unsigned index = 0;
  try
  {
    if (m_struct.GetPreset)
      index = m_struct.GetPreset(m_addonInstance);
  }
  catch (std::exception& ex)
  {
    ADDON::LogException(this, ex, __FUNCTION__); // Handle exception
    memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
    return 0;
  }
  return index;
}

std::string CVisualisation::GetPresetName()
{
  if (!m_presets.empty())
    return m_presets[GetPreset()];
  else
    return "";
}

bool CVisualisation::IsInUse() const
{
  return CSettings::GetInstance().GetString(CSettings::SETTING_MUSICPLAYER_VISUALISATION) == ID();
}
