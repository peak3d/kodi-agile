#pragma once
/*
 *      Copyright (C) 2005-2016 Team Kodi
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

#include "../AddonBase.h"

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

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SCR_PROPS
{
  void *device;
  int x;
  int y;
  int width;
  int height;
  float pixelRatio;
  const char *name;
  const char *presets;
  const char *profile;
} SCR_PROPS;

typedef struct sAddonToKodiFuncTable_ScreenSaver
{
  void* kodiInstance;
} sAddonToKodiFuncTable_ScreenSaver;

typedef struct sKodiToAddonFuncTable_ScreenSaver
{
  void (__cdecl* Start) (void* addonInstance);
  void (__cdecl* Render) (void* addonInstance);
} sKodiToAddonFuncTable_ScreenSaver;

typedef struct sAddonInstance_ScreenSaver
{
  SCR_PROPS props;
  sAddonToKodiFuncTable_ScreenSaver toKodi;
  sKodiToAddonFuncTable_ScreenSaver toAddon;
} sAddonInstance_ScreenSaver;


#ifdef __cplusplus
namespace kodi {
namespace addon {
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

namespace screensaver {

  class CAddon : public kodi::addon::IAddonInstance
  {
  public:
    CAddon(void* instance)
      : IAddonInstance(ADDON_INSTANCE_SCREENSAVER),
        m_instance(static_cast<sAddonInstance_ScreenSaver*>(instance))
    {
      m_instance->toAddon.Start = ADDON_Start;
      m_instance->toAddon.Render = ADDON_Render;
    }
    virtual ~CAddon() { }

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
      static_cast<CAddon*>(addonInstance)->Start();
    }

    inline static void ADDON_Render(void* addonInstance)
    {
      static_cast<CAddon*>(addonInstance)->Render();
    }

    sAddonInstance_ScreenSaver* m_instance;
  };

} /* namespace screensaver */
} /* namespace addon */
} /* namespace kodi */
} /* extern "C" */
#endif
