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

#include "SoundPlay.h"

#include "addons/kodi-addon-dev-kit/include/kodi/SoundPlay.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "addons/interfaces/Translators.h"
#include "cores/AudioEngine/AEFactory.h"
#include "cores/AudioEngine/Engines/ActiveAE/ActiveAE.h"
#include "cores/AudioEngine/Interfaces/AESound.h"
#include "cores/AudioEngine/Utils/AEChannelData.h"

using namespace ActiveAE;
using namespace kodi; // addon-dev-kit namespace

namespace ADDON
{

void Interface_Audio::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_audio = (sAddonToKodiFuncTable_kodi_audio*)malloc(sizeof(sAddonToKodiFuncTable_kodi_audio));
  funcTable->toKodi.kodi_audio->soundplay_get_handle = soundplay_get_handle;
  funcTable->toKodi.kodi_audio->soundplay_release_handle = soundplay_release_handle;
  funcTable->toKodi.kodi_audio->soundplay_play = soundplay_play;
  funcTable->toKodi.kodi_audio->soundplay_stop = soundplay_stop;
  funcTable->toKodi.kodi_audio->soundplay_set_channel = soundplay_set_channel;
  funcTable->toKodi.kodi_audio->soundplay_get_channel = soundplay_get_channel;
  funcTable->toKodi.kodi_audio->soundplay_set_volume = soundplay_set_volume;
  funcTable->toKodi.kodi_audio->soundplay_get_volume = soundplay_get_volume;
}

void Interface_Audio::DeInit(sFuncTable_Addon* funcTable)
{
  if (funcTable->toKodi.kodi_audio)
  {
    free(funcTable->toKodi.kodi_audio);
    funcTable->toKodi.kodi_audio = nullptr;
  }
}

void* Interface_Audio::soundplay_get_handle(void* kodiInstance, const char* filename)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Audio::%s - invalid data (addon='%p', filename='%p')", __FUNCTION__, addon, filename);
    return nullptr;
  }

  try
  {
    IAESound *sound = CAEFactory::MakeSound(filename);
    if (!sound)
    {
      CLog::Log(LOGERROR, "Interface_Audio::%s - failed to make sound play data", __FUNCTION__);
      return nullptr;
    }

    return sound;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void Interface_Audio::soundplay_release_handle(void* kodiInstance, void* sndHandle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || sndHandle == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Audio::%s - invalid data (addon='%p', sndHandle='%p')", __FUNCTION__, addon, sndHandle);
    return;
  }

  try
  {
    CAEFactory::FreeSound(static_cast<IAESound*>(sndHandle));
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_Audio::soundplay_play(void* kodiInstance, void* sndHandle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || sndHandle == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Audio::%s - invalid data (addon='%p', sndHandle='%p')", __FUNCTION__, addon, sndHandle);
    return;
  }

  try
  {
    static_cast<IAESound*>(sndHandle)->Play();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_Audio::soundplay_stop(void* kodiInstance, void* sndHandle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || sndHandle == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Audio::%s - invalid data (addon='%p', sndHandle='%p')", __FUNCTION__, addon, sndHandle);
    return;
  }

  try
  {
    static_cast<IAESound*>(sndHandle)->Stop();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_Audio::soundplay_set_channel(void* kodiInstance, void* sndHandle, int channel)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || sndHandle == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Audio::%s - invalid data (addon='%p', sndHandle='%p')", __FUNCTION__, addon, sndHandle);
    return;
  }

  try
  {
    static_cast<IAESound*>(sndHandle)->SetChannel(GetKodiAudioChannel(channel));
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

int Interface_Audio::soundplay_get_channel(void* kodiInstance, void* sndHandle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon || !sndHandle)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - invalid data (addon='%p', sndHandle='%p')", __FUNCTION__, addon, sndHandle);
    return AUDIO_CH_NULL;
  }

  try
  {
    return GetAddonAudioChannel(static_cast<IAESound*>(sndHandle)->GetChannel());
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return AUDIO_CH_NULL;
}

void Interface_Audio::soundplay_set_volume(void* kodiInstance, void* sndHandle, float volume)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || sndHandle == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Audio::%s - invalid data (addon='%p', sndHandle='%p')", __FUNCTION__, addon, sndHandle);
    return;
  }

  try
  {
    static_cast<IAESound*>(sndHandle)->SetVolume(volume);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

float Interface_Audio::soundplay_get_volume(void* kodiInstance, void* sndHandle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || sndHandle == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Audio::%s - invalid data (addon='%p', sndHandle='%p')", __FUNCTION__, addon, sndHandle);
    return 0.0f;
  }

  try
  {
    return static_cast<IAESound*>(sndHandle)->GetVolume();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0.0f;
}

} /* namespace ADDON */
