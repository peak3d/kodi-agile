/*
 *      Copyright (C) 2013 Arne Morten Kvarving
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
#include "AudioDecoder.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "addons/interfaces/Translators.h"
#include "music/tags/MusicInfoTag.h"
#include "music/tags/TagLoaderTagLib.h"
#include "cores/AudioEngine/Utils/AEUtil.h"

namespace ADDON
{

std::unique_ptr<CAudioDecoder> CAudioDecoder::FromExtension(AddonProps props, const cp_extension_t* ext)
{
  std::string extension = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@extension");
  std::string mimetype = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@mimetype");
  bool tags = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@tags") == "true";
  bool tracks = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@tracks") == "true";
  std::string codecName = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@name");
  std::string strExt = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@name") + "stream";

  return std::unique_ptr<CAudioDecoder>(new CAudioDecoder(std::move(props), std::move(extension),
      std::move(mimetype), tags, tracks, std::move(codecName), std::move(strExt)));
}

CAudioDecoder::CAudioDecoder(AddonProps props)
  : CAddonDll(std::move(props)),
    m_tags(false),
    m_tracks(false),
    m_addonInstance(nullptr)

{
  memset(&m_struct, 0, sizeof(m_struct));
}

CAudioDecoder::CAudioDecoder(AddonProps props, std::string extension, std::string mimetype,
    bool tags, bool tracks, std::string codecName, std::string strExt)
    : CAddonDll(std::move(props)), m_extension(extension), m_mimetype(mimetype),
      m_tags(tags), m_tracks(tracks),
      m_addonInstance(nullptr)

{
  m_CodecName = std::move(codecName);
  m_strExt = std::move(strExt);

  memset(&m_struct, 0, sizeof(m_struct));
}

CAudioDecoder::~CAudioDecoder()
{
  DeInit();
}

bool CAudioDecoder::Init(const CFileItem& file, unsigned int filecache)
{
  if (!Initialized())
    return false;

  m_struct.toKodi.kodiInstance = this;
  if (CAddonDll::CreateInstance(ADDON_INSTANCE_AUDIODECODER, ID().c_str(), &m_struct, &m_addonInstance) != ADDON_STATUS_OK)
    return false;

  // for replaygain
  CTagLoaderTagLib tag;
  tag.Load(file.GetPath(), XFILE::CMusicFileDirectory::m_tag, NULL);

  int channels;
  int sampleRate;
  eAudioDataFormat format;
  const eAudioChannel* channel = nullptr;

  try
  {
    if (m_struct.toAddon.Init)
    {
      if (!m_struct.toAddon.Init(m_addonInstance, file.GetPath().c_str(), filecache,
                                  &channels, &sampleRate,
                                  &m_bitsPerSample, &m_TotalTime,
                                  &m_bitRate, &format, &channel))
      {
        return false;
      }
    }
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }

  m_format.m_dataFormat = kodi::addon::GetKodiAudioFormat(format);
  m_format.m_sampleRate = sampleRate;
  if (channel)
  {
    memset(m_channel, AE_CH_NULL, sizeof(m_channel));
    for (unsigned int i = 0; i < AE_CH_MAX && i < AUDIO_CH_MAX && channel[i] != AUDIO_CH_NULL; ++i)
      m_channel[i] = kodi::addon::GetKodiAudioChannel(channel[i]);
    m_format.m_channelLayout = CAEChannelInfo((const AEChannel*)&m_channel);
  }
  else
    m_format.m_channelLayout = CAEUtil::GuessChLayout(channels);

  return true;
}

int CAudioDecoder::ReadPCM(uint8_t* buffer, int size, int* actualsize)
{
  if (!Initialized())
    return 0;

  try
  {
    if (m_struct.toAddon.ReadPCM)
      return m_struct.toAddon.ReadPCM(m_addonInstance, buffer, size, actualsize);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
  
  return 0;
}

bool CAudioDecoder::Seek(int64_t time)
{
  if (!Initialized())
    return false;

  try
  {
    if (m_struct.toAddon.Seek)
      return m_struct.toAddon.Seek(m_addonInstance, time);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }

  return false;
}

void CAudioDecoder::DeInit()
{
  if (!Initialized())
    return;

  CAddonDll::DestroyInstance(ADDON_INSTANCE_AUDIODECODER, m_addonInstance);
  memset(&m_struct, 0, sizeof(m_struct));
  m_addonInstance = nullptr;
}

bool CAudioDecoder::Load(const std::string& fileName,
                         MUSIC_INFO::CMusicInfoTag& tag,
                         MUSIC_INFO::EmbeddedArt* art)
{
  bool ret = false;

  if (!Initialized())
    return ret;

  char title[ADDON_STANDARD_STRING_LENGTH_SMALL];
  char artist[ADDON_STANDARD_STRING_LENGTH_SMALL];
  int length;
  try
  {
    if (m_struct.toAddon.ReadTag)
      ret = m_struct.toAddon.ReadTag(m_addonInstance, fileName.c_str(), title, artist, &length);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }

  if (ret)
  {
    tag.SetTitle(title);
    tag.SetArtist(artist);
    tag.SetDuration(length);
  }

  return ret;
}

int CAudioDecoder::GetTrackCount(const std::string& strPath)
{
  int result = 0;

  if (!Initialized())
    return result;

  try
  {
    if (m_struct.toAddon.TrackCount)
      result = m_struct.toAddon.TrackCount(m_addonInstance, strPath.c_str());
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }

  if (result > 1 && !Load(strPath, XFILE::CMusicFileDirectory::m_tag, NULL))
    return 0;

  XFILE::CMusicFileDirectory::m_tag.SetLoaded(true);
  return result;
}

void CAudioDecoder::Destroy()
{
  CAddonDll::Destroy();
}

void CAudioDecoder::ExceptionHandle(std::exception& ex, const char* function)
{
  ADDON::LogException(this, ex, function); // Handle exception
  memset(&m_struct.toAddon, 0, sizeof(m_struct.toAddon)); // reset function table to prevent further exception call  
}

} /*namespace ADDON*/

