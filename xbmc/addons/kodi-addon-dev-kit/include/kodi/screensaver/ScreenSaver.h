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

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * The four structures below are used from the headers to create the "C"
 * interface to Kodi. It contains the needed data and for Kodi needed add-on
 * function addresses for the call of them.
 *
 * The sAddonInstance_ScreenSaver structure becomes given to the class creation
 * of CInstanceScreensaver(KODI_HANDLE instance) who structure is send from
 * Kodi before.
 *
 * No need of doxygen documentation of the structures here, are only internally
 * used.
 */
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
namespace kodi
{
namespace addon
{
  //============================================================================
  ///
  /// \addtogroup cpp_kodi_addon_screensaver
  /// @brief \python_func_with_rev{ kodi::addon::CInstanceScreensaver, added in v18 }
  /// **Screensaver add-on instance**
  ///
  /// A screensaver (or screen saver) is a Kodi addon that blanks the screen or
  /// fills it with moving images or patterns when the computer is not in use.
  /// Initially designed to prevent phosphor burn-in on CRT and plasma computer
  /// monitors (hence the name), screensavers are now used primarily for
  /// entertainment, security or to display system status information.
  ///
  /// It has the header \ref ScreenSaver.h "#include <kodi/screensaver/ScreenSaver.h>"
  /// be included to enjoy it.
  ///
  /// This interface allow the creating of a screensavers for Kodi, based upon
  /// **DirectX** or/and **OpenGL** rendering with `C++` code.
  ///
  /// The interface is small and easy usable, it have two function. One for
  /// the creation with <b><c>Start()</c></b> and for the render time with
  /// <b><c>Render()</c></b>, this both are required and must be implemented.
  ///
  /// Further are several \ref cpp_kodi_addon_screensaver_CB "other functions"
  /// available where the child class can  ask about used device, display and
  /// few other parts.
  ///
  ///
  /// --------------------------------------------------------------------------
  ///
  /// **Here as example what is minimum required to start on a screensaver:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/screensaver/ScreenSaver.h>
  ///
  /// class CMyScreenSaver : public ::kodi::addon::CInstanceScreensaver
  /// {
  /// public:
  ///   CMyScreenSaver(void* instance);
  ///
  ///   void Start() override;
  ///   void Render() override;
  /// };
  ///
  /// CMyScreenSaver::CMyScreenSaver(void* instance)
  ///   : CInstanceScreensaver(instance)
  /// {
  ///   ...
  /// }
  ///
  /// void CMyScreenSaver::Start()
  /// {
  ///   ...
  /// }
  ///
  /// void CMyScreenSaver::Render()
  /// {
  ///   ...
  /// }
  ///
  /// /*----------------------------------------------------------------------*/
  ///
  /// class CMyAddon : public ::kodi::addon::CAddonBase
  /// {
  /// public:
  ///   CMyAddon() { }
  ///   ADDON_STATUS CreateInstance(int instanceType,
  ///                               std::string instanceID,
  ///                               KODI_HANDLE instance,
  ///                               KODI_HANDLE& addonInstance) override;
  /// };
  ///
  /// /* If you use only one instance in your add-on, can be instanceType and
  ///  * instanceID ignored */
  /// ADDON_STATUS CMyAddon::CreateInstance(int instanceType,
  ///                                       std::string instanceID,
  ///                                       KODI_HANDLE instance,
  ///                                       KODI_HANDLE& addonInstance)
  /// {
  ///   kodi::Log(LOG_NOTICE, "Creating my Screensaver");
  ///   addonInstance = new CMyScreenSaver(instance);
  ///   return ADDON_STATUS_OK;
  /// }
  ///
  /// ADDONCREATOR(CMyAddon);
  /// ~~~~~~~~~~~~~
  ///
  /// The **desctruction** of the here created example class `CMyScreenSaver`
  /// becomes done from Kodi's header and never for add-on instances a delete on
  /// add-on itself necessary.
  ///
  //----------------------------------------------------------------------------
  class CInstanceScreensaver : public IAddonInstance
  {
  public:
    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver
    /// @brief
    ///
    /// @param[in] instance               The with <b>`kodi::addon::CAddonBase::CreateInstance(...)`</b>
    ///                                   given `instance` value.
    ///   @warning Use always only `instance` value from CreateInstance call
    ///
    CInstanceScreensaver(KODI_HANDLE instance)
      : IAddonInstance(ADDON_INSTANCE_SCREENSAVER),
        m_instance(static_cast<sAddonInstance_ScreenSaver*>(instance))
    {
      m_instance->toAddon.Start = ADDON_Start;
      m_instance->toAddon.Render = ADDON_Render;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver
    /// @brief Destructor
    ///
    virtual ~CInstanceScreensaver() { }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver
    /// @brief To inform about start of rendering
    /// @note Required to implement
    ///
    virtual void Start()=0;
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver
    /// @brief To handle rendering time on add-on
    /// @note Required to implement
    ///
    virtual void Render()=0;
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \defgroup cpp_kodi_addon_screensaver_CB Information functions
    /// \ingroup cpp_kodi_addon_screensaver
    /// @brief **To get info about device, display and few other parts**
    ///
    //@{

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver_CB
    /// @brief Device that represents the display adapter
    ///
    /// @return Used device pointer
    ///
    /// @note only be available on **DirectX** and unused (`nullptr`) on
    /// **OpenGL**
    ///
    inline void* Device() { return m_instance->props.device; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver_CB
    /// @brief Returns the X position of the rendering window
    ///
    /// @return Position X in pixels
    ///
    inline int X() { return m_instance->props.x; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver_CB
    /// @brief Returns the Y position of the rendering window
    ///
    /// @return Position Y in pixels
    ///
    inline int Y() { return m_instance->props.y; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver_CB
    /// @brief Returns the width of the rendering window
    ///
    /// @return Width in pixels
    ///
    inline int Width() { return m_instance->props.width; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver_CB
    /// @brief Returns the height of the rendering window
    ///
    /// @return Height in pixels
    ///
    inline int Height() { return m_instance->props.height; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver_CB
    /// @brief Pixel aspect ratio (often abbreviated PAR) is a mathematical
    /// ratio that describes how the width of a pixel in a digital image
    /// compares to the height of that pixel.
    ///
    /// @return used Pixel aspect ratio
    ///
    inline float PixelRatio() { return m_instance->props.pixelRatio; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver_CB
    /// @brief To get the in `addon.xml` defined name of the add-on
    ///
    /// @return Add-on name
    ///
    inline std::string Name() { return m_instance->props.name; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver_CB
    /// @brief To get the full path where add-on is be installed
    ///
    /// @return Add-on installation path
    ///
    inline std::string Presets() { return m_instance->props.presets; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_screensaver_CB
    /// @brief To get the full user related path name for the add-on
    ///
    /// @note The folder with his add-on name on end is not created by default,
    /// if it is needed must be one time a kodi::vfs::CreateDirectory()
    /// call performed.
    ///
    /// @return User path name
    ///
    inline std::string Profile() { return m_instance->props.profile; }
    //--------------------------------------------------------------------------
    //@}

  private:
    inline static void ADDON_Start(void* addonInstance)
    {
      static_cast<CInstanceScreensaver*>(addonInstance)->Start();
    }

    inline static void ADDON_Render(void* addonInstance)
    {
      static_cast<CInstanceScreensaver*>(addonInstance)->Render();
    }

    sAddonInstance_ScreenSaver* m_instance;
  };

} /* namespace addon */
} /* namespace kodi */
} /* extern "C" */
#endif
