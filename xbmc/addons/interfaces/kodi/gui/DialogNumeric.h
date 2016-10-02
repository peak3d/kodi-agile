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

#include <time.h>

struct sFuncTable_Addon;

namespace ADDON
{
extern "C"
{

  struct Interface_GUIDialogNumeric
  {
    static void Init(sFuncTable_Addon* funcTable);

    static bool ShowAndVerifyNewPassword(void* kodiInstance,
                                         char &newPassword,
                                         unsigned int &maxStringSize);

    static int  ShowAndVerifyPassword(void* kodiInstance, char &password,
                                      unsigned int &maxStringSize,
                                      const char *heading,
                                      int retries);

    static bool ShowAndVerifyInput(void* kodiInstance, char &toVerify,
                                   unsigned int &maxStringSize,
                                   const char *heading,
                                   bool verifyInput);

    static bool ShowAndGetTime(void* kodiInstance, tm &time, const char* heading);

    static bool ShowAndGetDate(void* kodiInstance, tm &date, const char* heading);

    static bool ShowAndGetIPAddress(void* kodiInstance, char& ipAddress,
                                    unsigned int &maxStringSize, const char* heading);

    static bool ShowAndGetNumber(void* kodiInstance, char& input,
                                 unsigned int &maxStringSize, const char *heading,
                                 unsigned int autoCloseTimeoutMs);

    static bool ShowAndGetSeconds(void* kodiInstance, char &timeString,
                                  unsigned int &maxStringSize, const char* heading);
  };

} /* extern "C" */
} /* namespace ADDON */
