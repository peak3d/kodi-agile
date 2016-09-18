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
#include "AudioEncoder.h"

namespace ADDON
{

std::unique_ptr<CAudioEncoder> CAudioEncoder::FromExtension(AddonProps props, const cp_extension_t* ext)
{
  std::string extension = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@extension");
  return std::unique_ptr<CAudioEncoder>(new CAudioEncoder(std::move(props), std::move(extension)));
}

CAudioEncoder::CAudioEncoder(AddonProps props, std::string _extension)
    : AudioEncoderDll(std::move(props)), extension(std::move(_extension)), m_context(nullptr), m_addonInstance(nullptr)
{
}

bool CAudioEncoder::Init(audioenc_callbacks &callbacks)
{
  if (!Initialized())
    return false;

  if (CAddonDll<DllAudioEncoder, AudioEncoder, AUDIOENC_PROPS>::CreateInstance(ADDON_INSTANCE_AUDIOENCODER, ID().c_str(), nullptr, m_pStruct, this, &m_addonInstance) != ADDON_STATUS_OK)
    return false;

  // create encoder instance
  m_context = m_pStruct->Create(m_addonInstance, &callbacks);
  if (!m_context)
    return false;

  return m_pStruct->Start(m_addonInstance, m_context,
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

int CAudioEncoder::Encode(int nNumBytesRead, uint8_t* pbtStream)
{
  if (!Initialized() || !m_context)
    return 0;

  return m_pStruct->Encode(m_addonInstance, m_context, nNumBytesRead, pbtStream);
}

bool CAudioEncoder::Close()
{
  if (!Initialized() || !m_context)
    return false;

  if (!m_pStruct->Finish(m_addonInstance, m_context))
    return false;

  m_pStruct->Free(m_addonInstance, m_context);
  m_context = NULL;

  return true;
}

void CAudioEncoder::Destroy()
{
  CAddonDll<DllAudioEncoder, AudioEncoder, AUDIOENC_PROPS>::DestroyInstance(ADDON_INSTANCE_AUDIOENCODER, ID().c_str(), m_addonInstance);
  AudioEncoderDll::Destroy();
  m_addonInstance = nullptr;
}

} /*namespace ADDON*/

