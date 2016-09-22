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

#include "xbmc_addon_dll.h"
#include "xbmc_scr_types.h"

#include <string>

extern "C"
{

  class CAddonScreenSaver
  {
  public:
    CAddonScreenSaver(void* instance)
      : m_instance(static_cast<sFuncTable_ScreenSaver*>(instance))
    {
      m_instance->toAddon.Start = ADDON_Start;
      m_instance->toAddon.Render = ADDON_Render;
    }
    virtual ~CAddonScreenSaver() { }   

    inline void* Device() { return m_instance->props.device; }
    inline int X() { return m_instance->props.x; }
    inline int Y() { return m_instance->props.y; }
    inline int Width() { return m_instance->props.width; }
    inline int Height() { return m_instance->props.height; }
    inline float PixelRatio() { return m_instance->props.pixelRatio; }
    inline std::string Name() { return m_instance->props.name; }
    inline std::string Presets() { return m_instance->props.presets; }
    inline std::string Profile() { return m_instance->props.profile; }
    
    virtual void Start()=0;
    virtual void Render()=0;

  private:
    inline static void ADDON_Start(void* addonInstance)
    {
      static_cast<CAddonScreenSaver*>(addonInstance)->Start();
    }

    inline static void ADDON_Render(void* addonInstance)
    {
      static_cast<CAddonScreenSaver*>(addonInstance)->Render();
    }

    sFuncTable_ScreenSaver* m_instance;
  };

};

