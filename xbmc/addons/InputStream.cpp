/*
 *      Copyright (C) 2016 Team Kodi
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

#include "InputStream.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "cores/VideoPlayer/DVDDemuxers/DVDDemux.h"
#include "cores/VideoPlayer/DVDDemuxers/DVDDemuxUtils.h"
#include "threads/SingleLock.h"
#include "utils/log.h"
#include "utils/RegExp.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"

namespace ADDON
{

std::map<std::string, CInputStream::Config> CInputStream::m_configMap;
CCriticalSection CInputStream::m_parentSection;

std::unique_ptr<CInputStream> CInputStream::FromExtension(AddonProps props, const cp_extension_t* ext)
{
  std::string listitemprops = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@listitemprops");
  std::string extensions = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@extension");
  std::string protocols = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@protocols");
  std::string name(ext->plugin->identifier);
  std::unique_ptr<CInputStream> istr(new CInputStream(props, name, listitemprops,
                                                      extensions, protocols));
  istr->CheckConfig();
  return istr;
}

CInputStream::CInputStream(AddonProps props)
  : CAddonDll(std::move(props)),
    m_addonInstance(nullptr)
{
  memset(&m_struct, 0, sizeof(m_struct));
}

CInputStream::CInputStream(const AddonProps& props,
                           const std::string& name,
                           const std::string& listitemprops,
                           const std::string& extensions,
                           const std::string& protocols)
  : CAddonDll(std::move(props)),
    m_addonInstance(nullptr)
{
  m_fileItemProps = StringUtils::Tokenize(listitemprops, "|");
  for (auto &key : m_fileItemProps)
  {
    StringUtils::Trim(key);
    key = name + "." + key;
  }

  m_extensionsList = StringUtils::Tokenize(extensions, "|");
  for (auto &ext : m_extensionsList)
  {
    StringUtils::Trim(ext);
  }

  m_protocolsList = StringUtils::Tokenize(protocols, "|");
  for (auto &ext : m_protocolsList)
  {
    StringUtils::Trim(ext);
  }

  memset(&m_struct, 0, sizeof(m_struct));
}

CInputStream::~CInputStream()
{
  Destroy();
}

ADDON_STATUS CInputStream::Create()
{
  ADDON_STATUS status = CAddonDll::Create();
  if (status != ADDON_STATUS_OK)
    return status;
  
  memset(&m_struct, 0, sizeof(m_struct));
  m_struct.toKodi.kodiInstance = this;
  m_struct.toKodi.AllocateDemuxPacket = InputStreamAllocateDemuxPacket;
  m_struct.toKodi.FreeDemuxPacket = InputStreamFreeDemuxPacket;
  status = CAddonDll::CreateInstance(ADDON_INSTANCE_INPUTSTREAM, ID().c_str(), &m_struct, &m_addonInstance);
  return status;
}

void CInputStream::Destroy(void)
{
  /* destroy the add-on */
  if (m_addonInstance)
  {
    CAddonDll::DestroyInstance(ADDON_INSTANCE_INPUTSTREAM, m_addonInstance);
    memset(&m_struct, 0, sizeof(m_struct));
    m_addonInstance = nullptr;
  }
  
  CAddonDll::Destroy();
}

void CInputStream::SaveSettings()
{
  CAddon::SaveSettings();
  if (!m_bIsChild)
    UpdateConfig();
}

void CInputStream::CheckConfig()
{
  bool hasConfig = false;

  {
    CSingleLock lock(m_parentSection);
    auto it = m_configMap.find(ID());
    hasConfig = it != m_configMap.end();
  }

  if (!m_bIsChild && !hasConfig)
    UpdateConfig();
}

void CInputStream::UpdateConfig()
{
  std::string pathList;
  ADDON_STATUS status = Create();
  if (status != ADDON_STATUS_OK)
    return;

  try
  {
    if (m_struct.toAddon.GetPathList)
      pathList = m_struct.toAddon.GetPathList(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
  Destroy();

  Config config;
  config.m_pathList = StringUtils::Tokenize(pathList, "|");
  for (auto &path : config.m_pathList)
  {
    StringUtils::Trim(path);
  }

  CSingleLock lock(m_parentSection);
  auto it = m_configMap.find(ID());
  if (it == m_configMap.end())
    config.m_parentBusy = false;
  else
    config.m_parentBusy = it->second.m_parentBusy;

  config.m_ready = true;
  if (status == ADDON_STATUS_PERMANENT_FAILURE)
    config.m_ready = false;

  m_configMap[ID()] = config;
}

bool CInputStream::UseParent()
{
  CSingleLock lock(m_parentSection);

  auto it = m_configMap.find(ID());
  if (it == m_configMap.end())
    return false;
  if (it->second.m_parentBusy)
    return false;

  it->second.m_parentBusy = true;
  return true;
}

bool CInputStream::Supports(const CFileItem &fileitem)
{
  {
    CSingleLock lock(m_parentSection);

    auto it = m_configMap.find(ID());
    if (it == m_configMap.end())
      return false;
    if (!it->second.m_ready)
      return false;
  }

  // check if a specific inputstream addon is requested
  CVariant addon = fileitem.GetProperty("inputstreamaddon");
  if (!addon.isNull())
  {
    if (addon.asString() != ID())
      return false;
    else
      return true;
  }

  // check protocols
  std::string protocol = fileitem.GetURL().GetProtocol();
  if (!protocol.empty())
  {
    if (std::find(m_protocolsList.begin(),
                  m_protocolsList.end(), protocol) != m_protocolsList.end())
      return true;
  }

  // check paths
  CSingleLock lock(m_parentSection);
  auto it = m_configMap.find(ID());
  if (it == m_configMap.end())
    return false;

  bool match = false;
  for (auto &path : it->second.m_pathList)
  {
    if (path.empty())
      continue;

    CRegExp r(true, CRegExp::asciiOnly, path.c_str());
    if (r.RegFind(fileitem.GetPath().c_str()) == 0 && r.GetFindLen() > 5)
    {
      match = true;
      break;
    }
  }
  return match;
}

bool CInputStream::Open(CFileItem &fileitem)
{
  INPUTSTREAM props;
  std::map<std::string, std::string> propsMap;
  for (auto &key : m_fileItemProps)
  {
    if (fileitem.GetProperty(key).isNull())
      continue;
    propsMap[key] = fileitem.GetProperty(key).asString();
  }

  props.m_nCountInfoValues = 0;
  for (auto &pair : propsMap)
  {
    props.m_ListItemProperties[props.m_nCountInfoValues].m_strKey = pair.first.c_str();
    props.m_ListItemProperties[props.m_nCountInfoValues].m_strValue = pair.second.c_str();
    props.m_nCountInfoValues++;
  }

  props.m_strURL = fileitem.GetPath().c_str();
  
  std::string libFolder = URIUtils::GetDirectory(m_parentLib);
  std::string profileFolder = CSpecialProtocol::TranslatePath(Profile());
  props.m_libFolder = libFolder.c_str();
  props.m_profileFolder = profileFolder.c_str();

  bool ret = false;
  try
  {
    if (m_struct.toAddon.Open && m_struct.toAddon.GetCapabilities)
    {
      ret = m_struct.toAddon.Open(m_addonInstance, &props);
      if (ret)
         m_struct.toAddon.GetCapabilities(m_addonInstance, &m_caps);
    }
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); return false; }

  UpdateStreams();
  return ret;
}

void CInputStream::Close()
{
  try
  {
    if (m_struct.toAddon.Close)
      m_struct.toAddon.Close(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }

  if (!m_bIsChild)
  {
    CSingleLock lock(m_parentSection);
    auto it = m_configMap.find(ID());
    if (it != m_configMap.end())
      it->second.m_parentBusy = false;
  }
}

// IDisplayTime
int CInputStream::GetTotalTime()
{
  int ret = 0;
  try
  {
    if (m_struct.toAddon.GetTotalTime)
      ret = m_struct.toAddon.GetTotalTime(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
  return ret;
}

int CInputStream::GetTime()
{
  int ret = 0;
  try
  {
    if (m_struct.toAddon.GetTime)
      ret = m_struct.toAddon.GetTime(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
  return ret;
}

// IPosTime
bool CInputStream::PosTime(int ms)
{
  bool ret = false;
  try
  {
    if (m_struct.toAddon.PosTime)
      ret = m_struct.toAddon.PosTime(m_addonInstance, ms);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
  return ret;
}

// IDemux
void CInputStream::UpdateStreams()
{
  DisposeStreams();

  INPUTSTREAM_IDS streamIDs;
  streamIDs.m_streamCount = 0;

  try
  {
    if (m_struct.toAddon.GetStreamIds)
      m_struct.toAddon.GetStreamIds(m_addonInstance, &streamIDs);
  }
  catch (std::exception &e)
  {
    DisposeStreams();
    CLog::Log(LOGERROR, "CInputStream::UpdateStreams - error GetStreamIds. Reason: %s", e.what());
    return;
  }

  if (streamIDs.m_streamCount > INPUTSTREAM_IDS::MAX_STREAM_COUNT)
  {
    DisposeStreams();
    return;
  }

  for (unsigned int i=0; i<streamIDs.m_streamCount; i++)
  {
    INPUTSTREAM_INFO stream;
    memset(&stream, 0, sizeof(stream));
    try
    {
      if (m_struct.toAddon.GetStream)
        m_struct.toAddon.GetStream(m_addonInstance, streamIDs.m_streamIds[i], &stream);
    }
    catch (std::exception& ex)
    {
      DisposeStreams();
      ExceptionHandle(ex, __FUNCTION__);
      return;
    }

    if (stream.m_streamType == INPUTSTREAM_INFO::TYPE_NONE)
      continue;

    std::string codecName(stream.m_codecName);
    StringUtils::ToLower(codecName);
    AVCodec *codec = avcodec_find_decoder_by_name(codecName.c_str());
    if (!codec)
      continue;

    CDemuxStream *demuxStream;

    if (stream.m_streamType == INPUTSTREAM_INFO::TYPE_AUDIO)
    {
      CDemuxStreamAudio *audioStream = new CDemuxStreamAudio();

      audioStream->iChannels = stream.m_Channels;
      audioStream->iSampleRate = stream.m_SampleRate;
      audioStream->iBlockAlign = stream.m_BlockAlign;
      audioStream->iBitRate = stream.m_BitRate;
      audioStream->iBitsPerSample = stream.m_BitsPerSample;
      demuxStream = audioStream;
    }
    else if (stream.m_streamType == INPUTSTREAM_INFO::TYPE_VIDEO)
    {
      CDemuxStreamVideo *videoStream = new CDemuxStreamVideo();

      videoStream->iFpsScale = stream.m_FpsScale;
      videoStream->iFpsRate = stream.m_FpsRate;
      videoStream->iWidth = stream.m_Width;
      videoStream->iHeight = stream.m_Height;
      videoStream->fAspect = stream.m_Aspect;
      videoStream->stereo_mode = "mono";
      demuxStream = videoStream;
    }
    else if (stream.m_streamType == INPUTSTREAM_INFO::TYPE_SUBTITLE)
    {
      //! @todo needs identifier in INPUTSTREAM_INFO
      continue;
    }
    else
      continue;

    demuxStream->codec = codec->id;
    demuxStream->bandwidth = stream.m_Bandwidth;
    demuxStream->codecName = stream.m_codecInternalName;
    demuxStream->uniqueId = streamIDs.m_streamIds[i];
    demuxStream->language[0] = stream.m_language[0];
    demuxStream->language[1] = stream.m_language[1];
    demuxStream->language[2] = stream.m_language[2];
    demuxStream->language[3] = stream.m_language[3];

    if (stream.m_ExtraData && stream.m_ExtraSize)
    {
      demuxStream->ExtraData = new uint8_t[stream.m_ExtraSize];
      demuxStream->ExtraSize = stream.m_ExtraSize;
      for (unsigned int j=0; j<stream.m_ExtraSize; j++)
        demuxStream->ExtraData[j] = stream.m_ExtraData[j];
    }

    m_streams[demuxStream->uniqueId] = demuxStream;
  }
}

void CInputStream::DisposeStreams()
{
  for (auto &stream : m_streams)
    delete stream.second;
  m_streams.clear();
}

int CInputStream::GetNrOfStreams() const
{
  return m_streams.size();
}

CDemuxStream* CInputStream::GetStream(int iStreamId)
{
  std::map<int, CDemuxStream*>::iterator it = m_streams.find(iStreamId);
  if (it != m_streams.end())
    return it->second;

  return nullptr;
}

std::vector<CDemuxStream*> CInputStream::GetStreams() const
{
  std::vector<CDemuxStream*> streams;

  for (auto &stream : m_streams)
  {
    streams.push_back(stream.second);
  }

  return streams;
}

void CInputStream::EnableStream(int iStreamId, bool enable)
{
  std::map<int, CDemuxStream*>::iterator it = m_streams.find(iStreamId);
  if (it == m_streams.end())
    return;

  try
  {
    if (m_struct.toAddon.EnableStream)
      m_struct.toAddon.EnableStream(m_addonInstance, it->second->uniqueId, enable);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
}

DemuxPacket* CInputStream::ReadDemux()
{
  DemuxPacket* pPacket = nullptr;
  try
  {
    if (m_struct.toAddon.DemuxRead)
      pPacket = m_struct.toAddon.DemuxRead(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); return nullptr; }

  if (!pPacket)
  {
    return nullptr;
  }
  else if (pPacket->iStreamId == DMX_SPECIALID_STREAMINFO)
  {
    UpdateStreams();
  }
  else if (pPacket->iStreamId == DMX_SPECIALID_STREAMCHANGE)
  {
    UpdateStreams();
  }

  return pPacket;
}

bool CInputStream::SeekTime(int time, bool backward, double* startpts)
{
  bool ret = false;
  try
  {
    if (m_struct.toAddon.DemuxSeekTime)
      ret = m_struct.toAddon.DemuxSeekTime(m_addonInstance, time, backward, startpts);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
  return ret;
}

void CInputStream::AbortDemux()
{
  try
  {
    if (m_struct.toAddon.DemuxAbort)
      m_struct.toAddon.DemuxAbort(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
}

void CInputStream::FlushDemux()
{
  try
  {
    if (m_struct.toAddon.DemuxFlush)
      m_struct.toAddon.DemuxFlush(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
}

void CInputStream::SetSpeed(int iSpeed)
{
  try
  {
    if (m_struct.toAddon.DemuxSetSpeed)
      m_struct.toAddon.DemuxSetSpeed(m_addonInstance, iSpeed);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
}

int CInputStream::ReadStream(uint8_t* buf, unsigned int size)
{
  int ret = -1;
  try
  {
    if (m_struct.toAddon.ReadStream)
      ret = m_struct.toAddon.ReadStream(m_addonInstance, buf, size);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
  return ret;
}

int64_t CInputStream::SeekStream(int64_t offset, int whence)
{
  int64_t ret = -1;
  try
  {
    if (m_struct.toAddon.SeekStream)
      ret = m_struct.toAddon.SeekStream(m_addonInstance, offset, whence);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
  return ret;
}

int64_t CInputStream::PositionStream()
{
  int64_t ret = -1;
  try
  {
    if (m_struct.toAddon.PositionStream)
      ret = m_struct.toAddon.PositionStream(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
  return ret;
}

int64_t CInputStream::LengthStream()
{
  int64_t ret = -1;
  try
  {
    if (m_struct.toAddon.LengthStream)
      ret = m_struct.toAddon.LengthStream(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
  return ret;
}

void CInputStream::PauseStream(double time)
{
  try
  {
    if (m_struct.toAddon.PauseStream)
      m_struct.toAddon.PauseStream(m_addonInstance, time);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
}

bool CInputStream::IsRealTimeStream()
{
  bool ret = false;
  try
  {
    if (m_struct.toAddon.IsRealTimeStream)
      ret = m_struct.toAddon.IsRealTimeStream(m_addonInstance);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
  return ret;
}

void CInputStream::SetVideoResolution(int width, int height)
{
  try
  {
    if (m_struct.toAddon.SetVideoResolution)
      m_struct.toAddon.SetVideoResolution(m_addonInstance, width, height);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }
}

void CInputStream::ExceptionHandle(std::exception& ex, const char* function)
{
  ADDON::Exception::LogStdException(this, ex, function); // Handle exception and disable add-on
  memset(&m_struct.toAddon, 0, sizeof(m_struct.toAddon)); // reset function table to prevent further exception call  
}

// Static callback functions for add-on to Kodi below

DemuxPacket* CInputStream::InputStreamAllocateDemuxPacket(void *addonData, int iDataSize)
{
  return CDVDDemuxUtils::AllocateDemuxPacket(iDataSize);
}

void CInputStream::InputStreamFreeDemuxPacket(void *addonData, DemuxPacket* pPacket)
{
  CDVDDemuxUtils::FreeDemuxPacket(pPacket);
}

} /* namespace ADDON */
