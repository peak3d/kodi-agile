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

#ifdef TARGET_WINDOWS
#include <windows.h>
#else
#ifndef __cdecl
#define __cdecl
#endif
#ifndef __declspec
#define __declspec(X)
#endif
#endif

#include <stdint.h>

extern "C"
{

  typedef struct sAddonToKodiFuncTable_AudioEncoder
  {
    void* kodiInstance;
    int (*write) (void* kodiInstance, uint8_t* data, int len);
    int64_t (*seek)(void* kodiInstance, int64_t pos, int whence);
  } sAddonToKodiFuncTable_AudioEncoder;

  typedef struct sKodiToAddonFuncTable_AudioEncoder
  {
    bool (__cdecl* Start) (void* addonInstance, int iInChannels, int iInRate, int iInBits,
                           const char* title, const char* artist,
                           const char* albumartist, const char* album,
                           const char* year, const char* track,
                           const char* genre, const char* comment,
                           int iTrackLength);
    int  (__cdecl* Encode) (void* addonInstance, int nNumBytesRead, uint8_t* pbtStream);
    bool (__cdecl* Finish) (void* addonInstance);
  } sKodiToAddonFuncTable_AudioEncoder;
  
  typedef struct sFuncTable_AudioEncoder
  {
    sAddonToKodiFuncTable_AudioEncoder toKodi;
    sKodiToAddonFuncTable_AudioEncoder toAddon;
  } sFuncTable_AudioEncoder;
}

