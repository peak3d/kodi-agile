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
#include "addons/interfaces/ExceptionHandling.h"

namespace ADDON
{

std::unique_ptr<CAudioEncoder> CAudioEncoder::FromExtension(AddonProps props, const cp_extension_t* ext)
{
  std::string extension = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@extension");
  return std::unique_ptr<CAudioEncoder>(new CAudioEncoder(std::move(props), std::move(extension)));
}

CAudioEncoder::CAudioEncoder(AddonProps props, std::string _extension)
    : CAddonDll(std::move(props)), extension(std::move(_extension)), m_context(nullptr), m_addonInstance(nullptr)
{
  memset(&m_struct, 0, sizeof(m_struct));
}

bool CAudioEncoder::Init(audioenc_callbacks &callbacks)
{
  bool ret = false;

  if (!Initialized())
    return ret;

  if (CAddonDll::CreateInstance(ADDON_INSTANCE_AUDIOENCODER, ID().c_str(), nullptr, &m_struct, this, &m_addonInstance) != ADDON_STATUS_OK)
    return ret;

  // create encoder instance
  try
  {
    m_context = nullptr;
    if (m_struct.Create)
      m_context = m_struct.Create(m_addonInstance, &callbacks);
    if (!m_context)
      return ret;
  }
  catch (std::exception& ex)
  {
    ADDON::LogException(this, ex, __FUNCTION__); // Handle exception and disable add-on
    memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
  }
 
  try
  {
    if (m_struct.Start)
      ret = m_struct.Start(m_addonInstance, m_context,
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
  catch (std::exception& ex)
  {
    ADDON::LogException(this, ex, __FUNCTION__); // Handle exception and disable add-on
    memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
  }

  return ret;
}

int CAudioEncoder::Encode(int nNumBytesRead, uint8_t* pbtStream)
{
  int ret = 0;
  if (!Initialized() || !m_context)
    return ret;

  try
  {
    if (m_struct.Encode)
      ret = m_struct.Encode(m_addonInstance, m_context, nNumBytesRead, pbtStream);
  }
  catch (std::exception& ex)
  {
    ADDON::LogException(this, ex, __FUNCTION__); // Handle exception and disable add-on
    memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
  }

  return ret;
}

bool CAudioEncoder::Close()
{
  if (!Initialized() || !m_context)
    return false;

  try
  {
    if (m_struct.Finish)
    {
      if (!m_struct.Finish(m_addonInstance, m_context))
        return false;
    }

    if (m_struct.Free)
      m_struct.Free(m_addonInstance, m_context);
  }
  catch (std::exception& ex)
  {
    ADDON::LogException(this, ex, __FUNCTION__); // Handle exception and disable add-on
    memset(&m_struct, 0, sizeof(m_struct)); // reset function table to prevent further exception call
  }

  m_context = nullptr;

  return true;
}

void CAudioEncoder::Destroy()
{
  CAddonDll::DestroyInstance(ADDON_INSTANCE_AUDIOENCODER, ID().c_str(), m_addonInstance);
  CAddonDll::Destroy();
  memset(&m_struct, 0, sizeof(m_struct));
  m_addonInstance = nullptr;
}

} /*namespace ADDON*/

