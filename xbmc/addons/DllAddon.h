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

#include "DynamicDll.h"
#include "addons/kodi-addon-dev-kit/include/kodi/xbmc_addon_cpp_dll.h"

class DllAddonInterface
{
public:
  virtual ~DllAddonInterface() {}
  virtual ADDON_STATUS Create(void *cb) =0;
};

class DllAddon : public DllDynamic, public DllAddonInterface
{
public:
  DECLARE_DLL_WRAPPER_TEMPLATE(DllAddon)
  DEFINE_METHOD1(ADDON_STATUS, Create, (void* p1))
  BEGIN_METHOD_RESOLVE()
    RESOLVE_METHOD_RENAME(ADDON_Create, Create)
  END_METHOD_RESOLVE()
};

