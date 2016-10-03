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
  virtual bool IsInUse() const;

  bool CreateScreenSaver();
  void Start();
  void Render();
  void Destroy();

private:
  void ExceptionStdHandle(std::exception& ex, const char* function);
  void ExceptionErrHandle(int ex, const char* function);

  sAddonInstance_ScreenSaver m_struct;
  void* m_addonInstance;
};

} /* namespace ADDON */
