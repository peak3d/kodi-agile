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

struct sFuncTable_Addon;

namespace ADDON
{
extern "C"
{

  struct Interface_GUIDialogKeyboard
  {
    static void Init(sFuncTable_Addon* funcTable);

    static bool ShowAndGetInput(void* kodiInstance, char &aTextString,
                                unsigned int &maxStringSize,
                                bool allowEmptyResult,
                                unsigned int  autoCloseMs);

    static bool ShowAndGetInputWithHead(void* kodiInstance, char &aTextString,
                                        unsigned int &maxStringSize,
                                        const char *heading,
                                        bool allowEmptyResult,
                                        bool hiddenInput,
                                        unsigned int autoCloseMs);

    static bool ShowAndGetNewPassword(void* kodiInstance, char &strNewPassword,
                                      unsigned int &maxStringSize,
                                      unsigned int autoCloseMs);

    static bool ShowAndGetNewPasswordWithHead(void* kodiInstance, 
                                              char &newPassword,
                                              unsigned int &maxStringSize,
                                              const char *strHeading,
                                              bool allowEmptyResult,
                                              unsigned int autoCloseMs);

    static bool ShowAndVerifyNewPassword(void* kodiInstance,
                                         char &strNewPassword,
                                         unsigned int &maxStringSize,
                                         unsigned int autoCloseMs);

    static bool ShowAndVerifyNewPasswordWithHead(void* kodiInstance, 
                                                 char &strNewPassword,
                                                 unsigned int &maxStringSize,
                                                 const char *strHeading,
                                                 bool allowEmpty,
                                                 unsigned int autoCloseMs);

    static int  ShowAndVerifyPassword(void* kodiInstance, 
                                      char &strPassword,
                                      unsigned int &maxStringSize,
                                      const char *strHeading,
                                      int retries, unsigned int autoCloseMs);

    static bool ShowAndGetFilter(void* kodiInstance, char &aTextString,
                                 unsigned int &maxStringSize,
                                 bool searching, unsigned int  autoCloseMs);

    static bool SendTextToActiveKeyboard(void* kodiInstance, 
                                         const char *aTextString,
                                         bool closeKeyboard);

    static bool isKeyboardActivated(void* kodiInstance);
  };

} /* extern "C" */
} /* namespace ADDON */
