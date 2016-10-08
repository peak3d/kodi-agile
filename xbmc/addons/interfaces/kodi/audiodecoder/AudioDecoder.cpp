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
#include "cores/AudioEngine/Utils/AEUtil.h"
#include "music/tags/MusicInfoTag.h"
#include "music/tags/TagLoaderTagLib.h"

namespace ADDON
{

CAudioDecoder::CAudioDecoder(ADDON::AddonDllPtr addon)
  : m_addon(addon),
    m_tags(false),
    m_tracks(false),
    m_addonInstance(nullptr)

{
  m_extension = m_addon->ExtraInfoValueString("extension");
  m_mimetype = m_addon->ExtraInfoValueString("mimetype");
  m_tags = m_addon->ExtraInfoValueBool("tags");
  m_tracks = m_addon->ExtraInfoValueBool("tracks");
  m_CodecName = m_addon->ExtraInfoValueString("name");
  m_strExt = m_addon->ExtraInfoValueString("name") + "stream";

  memset(&m_struct, 0, sizeof(m_struct));
}

CAudioDecoder::~CAudioDecoder()
{
  DeInit();
}

bool CAudioDecoder::Init(const CFileItem& file, unsigned int filecache)
{
  if (!m_addon)
    return false;

  if (!m_addon->Initialized())
  {
    if (m_addon->Create() != ADDON_STATUS_OK)
      return false;
  }

  m_struct.toKodi.kodiInstance = this;
  if (m_addon->CreateInstance(ADDON_INSTANCE_AUDIODECODER, m_addon->ID().c_str(), &m_struct, &m_addonInstance) != ADDON_STATUS_OK)
    return false;

  // for replaygain
  CTagLoaderTagLib tag;
  tag.Load(file.GetPath(), XFILE::CMusicFileDirectory::m_tag, nullptr);

  int channels;
  int sampleRate;
  AudioDataFormat format;
  const AudioChannel* channel = nullptr;

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
  catch (std::exception& ex) { ExceptionStdHandle(ex, __FUNCTION__); }
  catch (int ex)             { ExceptionErrHandle(ex, __FUNCTION__); }
  catch (...)                { ExceptionUnkHandle(__FUNCTION__); }

  m_format.m_dataFormat = ADDON::GetKodiAudioFormat(format);
  m_format.m_sampleRate = sampleRate;
  if (channel)
  {
    AEChannel kodiChannel[AE_CH_MAX];
    memset(kodiChannel, AE_CH_NULL, sizeof(kodiChannel));
    for (unsigned int i = 0; i < AE_CH_MAX && i < AUDIO_CH_MAX && channel[i] != AUDIO_CH_NULL; ++i)
      kodiChannel[i] = ADDON::GetKodiAudioChannel(channel[i]);
    m_format.m_channelLayout = CAEChannelInfo((const AEChannel*)&kodiChannel);
  }
  else
    m_format.m_channelLayout = CAEUtil::GuessChLayout(channels);

  return true;
}

int CAudioDecoder::ReadPCM(uint8_t* buffer, int size, int* actualsize)
{
  try
  {
    if (m_struct.toAddon.ReadPCM)
      return m_struct.toAddon.ReadPCM(m_addonInstance, buffer, size, actualsize);
  }
  catch (std::exception& ex) { ExceptionStdHandle(ex, __FUNCTION__); }
  catch (int ex)             { ExceptionErrHandle(ex, __FUNCTION__); }
  catch (...)                { ExceptionUnkHandle(__FUNCTION__); }

  return 0;
}

bool CAudioDecoder::Seek(int64_t time)
{
  try
  {
    if (m_struct.toAddon.Seek)
      return m_struct.toAddon.Seek(m_addonInstance, time);
  }
  catch (std::exception& ex) { ExceptionStdHandle(ex, __FUNCTION__); }
  catch (int ex)             { ExceptionErrHandle(ex, __FUNCTION__); }
  catch (...)                { ExceptionUnkHandle(__FUNCTION__); }

  return false;
}

void CAudioDecoder::DeInit()
{
  m_addon->DestroyInstance(ADDON_INSTANCE_AUDIODECODER, m_addonInstance);
  memset(&m_struct, 0, sizeof(m_struct));
  m_addonInstance = nullptr;
}

bool CAudioDecoder::Load(const std::string& fileName,
                         MUSIC_INFO::CMusicInfoTag& tag,
                         MUSIC_INFO::EmbeddedArt* art)
{
  bool ret = false;

  char title[ADDON_STANDARD_STRING_LENGTH_SMALL];
  char artist[ADDON_STANDARD_STRING_LENGTH_SMALL];
  int length;
  try
  {
    if (m_struct.toAddon.ReadTag)
      ret = m_struct.toAddon.ReadTag(m_addonInstance, fileName.c_str(), title, artist, &length);
  }
  catch (std::exception& ex) { ExceptionStdHandle(ex, __FUNCTION__); }
  catch (int ex)             { ExceptionErrHandle(ex, __FUNCTION__); }
  catch (...)                { ExceptionUnkHandle(__FUNCTION__); }

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

  try
  {
    if (m_struct.toAddon.TrackCount)
      result = m_struct.toAddon.TrackCount(m_addonInstance, strPath.c_str());
  }
  catch (std::exception& ex) { ExceptionStdHandle(ex, __FUNCTION__); }
  catch (int ex)             { ExceptionErrHandle(ex, __FUNCTION__); }
  catch (...)                { ExceptionUnkHandle(__FUNCTION__); }

  if (result > 1 && !Load(strPath, XFILE::CMusicFileDirectory::m_tag, nullptr))
    return 0;

  XFILE::CMusicFileDirectory::m_tag.SetLoaded(true);
  return result;
}

void CAudioDecoder::ExceptionStdHandle(std::exception& ex, const char* function)
{
  Exception::LogStdException(m_addon, ex, function);
  m_addon->Destroy();
  CAddonMgr::GetInstance().DisableAddon(m_addon->ID());
  Exception::ShowExceptionErrorDialog(m_addon);
}

void CAudioDecoder::ExceptionErrHandle(int ex, const char* function)
{
  Exception::LogErrException(m_addon, ex, function);
  m_addon->Destroy();
  CAddonMgr::GetInstance().DisableAddon(m_addon->ID());
  Exception::ShowExceptionErrorDialog(m_addon);
}

void CAudioDecoder::ExceptionUnkHandle(const char* function)
{
  Exception::LogUnkException(m_addon, function);
  m_addon->Destroy();
  CAddonMgr::GetInstance().DisableAddon(m_addon->ID());
  Exception::ShowExceptionErrorDialog(m_addon);
}

} /* namespace ADDON */
