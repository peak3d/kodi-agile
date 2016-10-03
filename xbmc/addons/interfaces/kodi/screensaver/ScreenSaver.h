#pragma once
/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
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

#include "addons/AddonDll.h"
#include "addons/kodi-addon-dev-kit/include/kodi/screensaver/ScreenSaver.h"

namespace ADDON
{

class CScreenSaver : public ADDON::CAddonDll
{
public:
  explicit CScreenSaver(AddonProps props);
  explicit CScreenSaver(const char *addonID);
  virtual ~CScreenSaver() {}

  /*!
   * @brief Child from ADDON::CAddonDll to check on global calls the add-on here
   * is in use.
   *
   * If it is selected in settings and is running returns it true. But in case
   * of Exception becomes is set to not running to allow disable of them.
   *
   * @return true if is in use
   */
  virtual bool IsInUse() const;

  /*!
   * @{
   * @brief Main screensaver interface functions
   */
  bool CreateScreenSaver();
  void Start();
  void Render();
  void Destroy();
  /* @}*/

private:
  /*!
   * @{
   * @brief Functions below are used to handle exception between Kodi and his
   * add-on.
   *
   * ExceptionStdHandle(...):
   *   Used for to handle standard c++ exception partly generated from dev kits
   *   headers.
   *
   * ExceptionErrHandle(...):
   *   This is a special type basically used also in dev kit headers to give a
   *   exception with his ADDON_STATUS as integer value.
   *   Can be generated from headers by a massive fault detected on call of
   *   them.
   *
   * ExceptionUnkHandle(...);
   *   Used for everything else.
   *
   * With a call of them becomes the standardized function
   * Exception::LogUnkException(...) from ExceptionHandling.h used to write
   * log entry. This is always the first call, to see if it still crashes the
   * source of them.
   *
   * After them becomes the screensaver add-on Destroyed and complete disabled.
   *
   * As last step comes a dialog to inform the used about the Exception.
   *
   * How it is set on add-on calls
   * ~~~~~~~~~~~~~{.cpp}
   * try
   * {
   *   ...
   * }
   * catch (std::exception& ex) { ExceptionStdHandle(ex, __FUNCTION__); }
   * catch (int ex)             { ExceptionErrHandle(ex, __FUNCTION__); }
   * catch (...)                { ExceptionUnkHandle(__FUNCTION__); }
   * ~~~~~~~~~~~~~
   *
   * @note this way becomes also used on add-on typed e.g. pvr ...
   */
  void ExceptionStdHandle(std::exception& ex, const char* function);
  void ExceptionErrHandle(int ex, const char* function);
  void ExceptionUnkHandle(const char* function);
  /* @}*/

  /*! The on add-on header defined interface function table */
  sAddonInstance_ScreenSaver m_struct;

  /*! The instance pointer from add-on itself, who becomes passed back on all
   *  calls to them and set by the CAddonDll::CreateInstance(...) call */
  void* m_addonInstance;
};

} /* namespace ADDON */
