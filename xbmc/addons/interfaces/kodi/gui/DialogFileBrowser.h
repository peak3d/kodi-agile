#pragma once
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

#include <string>
#include <vector>

class CMediaSource;
struct sFuncTable_Addon;

typedef std::vector<CMediaSource> VECSOURCES;

namespace ADDON
{
extern "C"
{

  struct Interface_GUIDialogFileBrowser
  {
    static void Init(sFuncTable_Addon* funcTable);

    static bool ShowAndGetDirectory(void* kodiInstance, const char* shares, 
                                    const char* heading, char& path, 
                                    unsigned int& maxStringSize, bool writeOnly);

    static bool ShowAndGetFile(void* kodiInstance, const char* shares,
                               const char* mask, const char* heading,
                               char& file, unsigned int& maxStringSize,
                               bool useThumbs, bool useFileDirectories);

    static bool ShowAndGetFileFromDir(void* kodiInstance, const char* directory,
                                      const char* mask, const char* heading,
                                      char& path, unsigned int& maxStringSize,
                                      bool useThumbs, bool useFileDirectories,
                                      bool singleList);

    static bool ShowAndGetFileList(void* kodiInstance, const char* shares, 
                                   const char* mask, const char* heading, 
                                   char**& path, unsigned int& entries, 
                                   bool useThumbs, bool useFileDirectories);

    static bool ShowAndGetSource(void* kodiInstance, char& path, 
                                 unsigned int& maxStringSize, 
                                 bool allowNetworkShares, 
                                 const char* additionalShare, 
                                 const char* strType);

    static bool ShowAndGetImage(void* kodiInstance, const char* shares,
                                const char* heading, char& path,
                                unsigned int& maxStringSize);

    static bool ShowAndGetImageList(void* kodiInstance, const char* shares,
                                    const char* heading, char**& path,
                                    unsigned int& entries);

    static void ClearList(void* kodiInstance, char**& path, unsigned int entries);

  private:
    static void GetVECShares(VECSOURCES &vecShares, std::string strShares, std::string strPath);
  };

} /* extern "C" */
} /* namespace ADDON */
