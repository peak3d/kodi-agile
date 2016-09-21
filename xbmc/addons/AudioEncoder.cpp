/*
 *      Copyright (C) 2015-2016 Team Kodi
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
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "AudioEncoder.h"
#include "addons/interfaces/ExceptionHandling.h"

namespace ADDON
{

std::unique_ptr<CAudioEncoder> CAudioEncoder::FromExtension(AddonProps props, const cp_extension_t* ext)
{
  std::string extension = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@extension");
  return std::unique_ptr<CAudioEncoder>(new CAudioEncoder(std::move(props), std::move(extension)));
}

CAudioEncoder::CAudioEncoder(AddonProps props)
  : CAddonDll(std::move(props)),
    m_addonInstance{nullptr}
{
  memset(&m_struct.toAddon, 0, sizeof(m_struct.toAddon));
}

CAudioEncoder::CAudioEncoder(AddonProps props, std::string _extension)
  : CAddonDll(std::move(props)),
    extension(std::move(_extension)),
    m_addonInstance(nullptr)
{
  memset(&m_struct.toAddon, 0, sizeof(m_struct.toAddon));
}

bool CAudioEncoder::Init(sAddonToKodiFuncTable_AudioEncoder &callbacks)
{
  if (!Initialized())
    return false;
  
  m_struct.toKodi = callbacks;
  if (CAddonDll::CreateInstance(ADDON_INSTANCE_AUDIOENCODER, ID().c_str(), nullptr, &m_struct, this, &m_addonInstance) != ADDON_STATUS_OK)
    return false;

  try
  {
    if (m_struct.toAddon.Start)
      return m_struct.toAddon.Start(m_addonInstance,
                                    m_iInChannels,
                                    m_iInSampleRate,
                                    m_iInBitsPerSample,
                                    m_strTitle.c_str(),
                                    m_strArtist.c_str(),
                                    m_strAlbumArtist.c_str(),
                                    m_strAlbum.c_str(),
                                    m_strYear.c_str(),
                                    m_strTrack.c_str(),
                                    m_strGenre.c_str(),
                                    m_strComment.c_str(),
                                    m_iTrackLength);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }

  return false;
}

int CAudioEncoder::Encode(int nNumBytesRead, uint8_t* pbtStream)
{
  if (!Initialized() || !m_addonInstance)
    return 0;

  try
  {
    if (m_struct.toAddon.Encode)
      return m_struct.toAddon.Encode(m_addonInstance, nNumBytesRead, pbtStream);
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }

  return 0;
}

bool CAudioEncoder::Close()
{
  if (!Initialized() || !m_addonInstance)
    return false;

  try
  {
    if (m_struct.toAddon.Finish)
    {
      if (!m_struct.toAddon.Finish(m_addonInstance))
        return false;
    }
  }
  catch (std::exception& ex) { ExceptionHandle(ex, __FUNCTION__); }

  CAddonDll::DestroyInstance(ADDON_INSTANCE_AUDIOENCODER, ID().c_str(), m_addonInstance);
  m_addonInstance = nullptr;

  return true;
}

void CAudioEncoder::Destroy()
{
  CAddonDll::Destroy();
  memset(&m_struct.toAddon, 0, sizeof(m_struct.toAddon));
}

void CAudioEncoder::ExceptionHandle(std::exception& ex, const char* function)
{
  ADDON::LogException(this, ex, function); // Handle exception and disable add-on
  memset(&m_struct.toAddon, 0, sizeof(m_struct.toAddon)); // reset function table to prevent further exception call  
}

} /* namespace ADDON */

