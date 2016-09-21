#pragma once

/*
 *      Copyright (C) 2005-2015 Team Kodi
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
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <stdint.h>
#include "xbmc_addon_dll.h"
#include "xbmc_audioenc_types.h"

extern "C"
{
  /*! \brief Start encoder
   * \param addonInstance Encoder context from Create.
   * \param iInChannels Number of channels
   * \param iInRate Sample rate of input data
   * \param iInBits Bits per sample in input data
   * \param title The title of the song
   * \param artist The artist of the song
   * \param albumartist The albumartist of the song
   * \param year The year of the song
   * \param track The track number of the song
   * \param genre The genre of the song
   * \param comment A comment to attach to the song
   * \param iTrackLength Total track length in seconds
   * \sa IEncoder::Init
   */
  bool Start(void* addonInstance, int iInChannels, int iInRate, int iInBits,
             const char* title, const char* artist,
             const char* albumartist, const char* album,
             const char* year, const char* track,
             const char* genre, const char* comment, int iTrackLength);

  /*! \brief Encode a chunk of audio
   * \param addonInstance Encoder context from Create.
   * \param nNumBytesRead Number of bytes in input buffer
   * \param pbtStream the input buffer
   * \return Number of bytes consumed
   * \sa IEncoder::Encode
   */
  int Encode(void* addonInstance, int nNumBytesRead, uint8_t* pbtStream);


  /*! \brief Finalize encoding
   * \param addonInstance Encoder context from Create.
   * \return True on success, false on failure.
   */
  bool Finish(void* addonInstance);

  // function to export the above structure to XBMC
  void SetAudioEncoderFuncTable(struct sKodiToAddonFuncTable_AudioEncoder* pScr)
  {
    pScr->Start  = Start;
    pScr->Encode = Encode;
    pScr->Finish = Finish;
  };
};

