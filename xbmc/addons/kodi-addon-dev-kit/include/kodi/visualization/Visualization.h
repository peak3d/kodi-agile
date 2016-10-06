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

/*
 * Common data structures shared between Kodi and Kodi's visualizations
 */

#include "../AddonBase.h"

#include <cstddef>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The six structures below are used from the headers to create the "C"
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

/* Actions that we have defined for visualization. */
typedef enum VIS_ACTION
{
  /* Nothing */
  VIS_ACTION_NONE = 0,

  /* Next visualization preset */
  VIS_ACTION_NEXT_PRESET,

  /* Previous visualization preset */
  VIS_ACTION_PREV_PRESET,

  /* Load visualization preset. Becomes sendet after new select is requested */
  VIS_ACTION_LOAD_PRESET,

  /* Switch to a new random preset */
  VIS_ACTION_RANDOM_PRESET,

  /* Lock the current visualization preset */
  VIS_ACTION_LOCK_PRESET,

  /* Used to increase track rating in full screen playback (press info on
   * track and use pageplus on keyboard/remote */
  VIS_ACTION_RATE_PRESET_PLUS,

  /* Used to decrease track rating in full screen playback (press info on
   * track and use pageminus on keyboard/remote */
  VIS_ACTION_RATE_PRESET_MINUS,

  /* inform the visualization of the current album art image */
  VIS_ACTION_UPDATE_ALBUMART,

  /* inform the visualization of the current track's tag information */
  VIS_ACTION_UPDATE_TRACK
} VIS_ACTION;

typedef struct VIS_INFO
{
  bool bWantsFreq;
  int iSyncDelay;
} VIS_INFO;

typedef struct VIS_PROPS
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
  const char *submodule;
} VIS_PROPS;

typedef struct sAddonToKodiFuncTable_Visualization
{
  void* kodiInstance;
  void (__cdecl* TransferPreset) (void* kodiInstance, const char* preset);
  void (__cdecl* TransferSubmodule) (void* kodiInstance, const char* submodule);
} sAddonToKodiFuncTable_Visualization;

typedef struct sKodiToAddonFuncTable_Visualization
{
  void (__cdecl* Start)(void* addonInstance, int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName);
  void (__cdecl* AudioData)(void* addonInstance, const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength);
  void (__cdecl* Render) (void* addonInstance);
  void (__cdecl* GetInfo)(void* addonInstance, VIS_INFO *info);
  bool (__cdecl* OnAction)(void* addonInstance, VIS_ACTION action, const void *param);
  bool (__cdecl* HasPresets)(void* addonInstance);
  unsigned int (__cdecl *GetPresets)(void* addonInstance);
  unsigned int (__cdecl *GetPreset)(void* addonInstance);
  unsigned int (__cdecl *GetSubModules)(void* addonInstance);
  bool (__cdecl* IsLocked)(void* addonInstance);
} sKodiToAddonFuncTable_Visualization;

typedef struct sAddonInstance_Visualization
{
  VIS_PROPS props;
  sAddonToKodiFuncTable_Visualization toKodi;
  sKodiToAddonFuncTable_Visualization toAddon;
} sAddonInstance_Visualization;

#ifdef __cplusplus
namespace kodi {
namespace addon {

  //============================================================================
  /// \defgroup cpp_kodi_addon_visualization_VisTrack class VisTrack
  /// \ingroup cpp_kodi_addon_visualization
  /// @{
  /// @brief **Visualization track information structure**
  ///
  /// Becomes given on CInstanceVisualization::UpdateTrack() call to have on
  /// information of currently played song.
  ///
  class VisTrack
  {
  public:
    VisTrack()
    {
      title = artist = album = albumArtist = nullptr;
      genre = comment = lyrics = reserved1 = reserved2 = nullptr;
      trackNumber = discNumber = duration = year = 0;
      rating = 0;
      reserved3 = reserved4 = 0;
    }

    /// @brief Title of the currently playing song
    const char *title;

    /// @brief Get artist names from the artist decription string (if it exists)
    /// as a single string
    const char *artist;

    /// @brief Album from which the current song is from
    const char *album;

    /// @brief Album artist names from the artist decription string (if it
    /// exists) as a single string.
    const char *albumArtist;

    /// @brief Shows the genre of the currently playing artist
    const char *genre;

    /// @brief Comment of current song stored in ID tag info
    const char *comment;

    /// @brief Lyrics of the currently played song if available
    const char *lyrics;

    const char *reserved1;
    const char *reserved2;

    /// @brief Track number of current song
    int trackNumber;

    /// @brief Disc Number of current song stored in ID tag info
    int discNumber;

    /// @brief Duration of song in seconds
    int duration;

    /// @brief Year of release of current song
    int year;

    /// @brief The from user defined rating of song
    int rating;

    int reserved3;
    int reserved4;
  };
  /// @}
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// \addtogroup cpp_kodi_addon_visualization
  /// @brief \cpp_class{ kodi::addon::CInstanceVisualization }
  /// **Visualization add-on instance**
  ///
  /// [Music visualization](https://en.wikipedia.org/wiki/Music_visualization)
  /// or music visualisation, a feature in Kodi, generates animated imagery
  /// based on a piece of music. The imagery is usually generated and rendered
  /// in real time and in a way synchronized with the music as it is played.
  ///
  /// Visualization techniques range from simple ones (e.g., a simulation of an
  /// oscilloscope display) to elaborate ones, which often include a plurality
  /// of composited effects. The changes in the music's loudness and frequency
  /// spectrum are among the properties used as input to the visualization.
  ///
  /// It has the header \ref Visualization.h "#include <kodi/visualization/Visualization.h>"
  /// be included to enjoy it.
  ///
  /// This interface allow the creating of a visualizations for Kodi, based upon
  /// **DirectX** or/and **OpenGL** rendering with `C++` code.
  ///
  /// The interface have big possibility. By transferring the many different
  /// data, almost everything can be possible to use items of the respective
  /// title.
  ///
  /// Further are several \ref cpp_kodi_addon_visualization_CB "other functions"
  /// available where the child class can ask about used device, display and
  /// few other parts.
  ///
  /// --------------------------------------------------------------------------
  ///
  /// **Here as example what is minimum required to start on a visualization:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/visualization/Visualization.h>
  ///
  /// class CMyVisualization : public ::kodi::addon::CInstanceVisualization
  /// {
  /// public:
  ///   CMyVisualization(void* instance);
  ///
  ///   void Start(int channels, int samplesPerSec, int bitsPerSample, std::string songName) override;
  ///   void AudioData(const float* audioData, int audioDataLength, float* freqData, int freqDataLength) override;
  ///   void Render() override;
  /// };
  ///
  /// CMyVisualization::CMyVisualization(void* instance)
  ///   : CInstanceVisualization(instance)
  /// {
  ///   ...
  /// }
  ///
  /// void CMyVisualization::Start(int channels, int samplesPerSec, int bitsPerSample, std::string songName)
  /// {
  ///   ...
  /// }
  ///
  /// void CMyVisualization::AudioData(const float* audioData, int audioDataLength, float* freqData, int freqDataLength)
  /// {
  ///   ...
  /// }
  ///
  /// void CMyVisualization::Render()
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
  ///   kodi::Log(LOG_NOTICE, "Creating my Visualization");
  ///   addonInstance = new CMyVisualization(instance);
  ///   return ADDON_STATUS_OK;
  /// }
  ///
  /// ADDONCREATOR(CMyAddon);
  /// ~~~~~~~~~~~~~
  ///
  /// The **desctruction** of the here created example class `CMyVisualization`
  /// becomes done from Kodi's header and never for add-on instances a delete on
  /// add-on itself necessary.
  ///
  //----------------------------------------------------------------------------
  class CInstanceVisualization : public IAddonInstance
  {
  public:
    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Visualization class constructor
    ///
    /// @param[in] instance               The with <b>`kodi::addon::CAddonBase::CreateInstance(...)`</b>
    ///                                   given `instance` value.
    ///   @warning Use always only `instance` value from CreateInstance call
    ///
    CInstanceVisualization(void* instance)
      : IAddonInstance(ADDON_INSTANCE_VISUALIZATION),
        m_presetLockedByUser(false),
        m_instance(static_cast<sAddonInstance_Visualization*>(instance))
    {
       m_instance->toAddon.Start = ADDON_Start;
       m_instance->toAddon.AudioData = ADDON_AudioData;
       m_instance->toAddon.Render = ADDON_Render;
       m_instance->toAddon.GetInfo = ADDON_GetInfo;
       m_instance->toAddon.OnAction = ADDON_OnAction;
       m_instance->toAddon.HasPresets = ADDON_HasPresets;
       m_instance->toAddon.GetPresets = ADDON_GetPresets;
       m_instance->toAddon.GetPreset = ADDON_GetPreset;
       m_instance->toAddon.GetSubModules = ADDON_GetSubModules;
       m_instance->toAddon.IsLocked = ADDON_IsLocked;
    }

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Destructor
    ///
    virtual ~CInstanceVisualization() { }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief To notify visualization. that new song has been started
    ///
    /// It pass the number of audio channels, sample rate, bits/sample and
    /// offcourse the songname.
    ///
    /// @param[in] channels             Amount of channels on stream
    /// @param[in] samplesPerSec        Samples per second on stream
    /// @param[in] bitsPerSample        Amount of bits on one sample
    /// @param[in] songName             The name of the currently played song
    ///
    /// @note Required to implement
    ///
    virtual void Start(int channels, int samplesPerSec, int bitsPerSample, std::string songName)=0;
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief To inform about stop of rendering control
    ///
    virtual void Stop() { }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Pass audio data to visualization
    ///
    /// @param[in] audioData            is short audiodata [channel][audioDataLength]
    ///                                 containing the raw audio data
    /// @param[in] audioDataLength      length of audiodata array
    /// @param[in] freqData             fft-ed audio data
    /// @param[in] freqDataLength       length of frequeny data array
    ///
    /// @note Required to implement
    ///
    virtual void AudioData(const float* audioData, int audioDataLength, float* freqData, int freqDataLength)=0;
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief To handle rendering time on add-on
    ///
    /// @note Required to implement
    ///
    virtual void Render()=0;
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief To get the number of buffers from the current visualization
    ///
    /// @param[out] wantsFreq           Fourier transform the data if the wants it
    /// @param[out] syncDelay           Number of buffers from the current
    ///
    virtual void GetInfo(bool& wantsFreq, int& syncDelay) { wantsFreq = false; syncDelay = 0; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief To ask add-on about possibility with selectable presets
    ///
    /// @return                         Return true if it becomes supported
    ///
    virtual bool HasPresets() { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief To get a list of selectable visualization presets on add-on
    ///
    /// @param[out] presets             Vector list with selectable names of
    ///                                 presets
    /// @return                         Return true if successfull done
    ///
    virtual bool GetPresets(std::vector<std::string>& presets) { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Get the index of currently used preset on visualization add-on
    ///
    /// @return                         Index number
    ///
    virtual unsigned int GetPreset() { return 0; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief
    ///
    /// @param[out] submodules
    /// @return
    ///
    virtual bool GetSubModules(std::vector<std::string>& submodules) { return 0; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief To ask add-on is locked on current preset
    ///
    /// @return                         True if locked there
    ///
    virtual bool IsLocked() { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Load previous visualization preset
    ///
    /// @return                 Return true if it becomes used from you
    virtual bool PrevPreset() { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Load next visualization preset
    ///
    /// @return                 Return true if it becomes used from you
    virtual bool NextPreset() { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Load visualization preset
    ///
    /// Becomes send after new select is requested.
    ///
    /// @param[in] select       Preset index to use
    /// @return                 Return true if the load becomes used from you
    virtual bool LoadPreset(int select) { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Switch to a new random preset
    ///
    /// @return                 Return true if the random becomes used from you
    virtual bool RandomPreset() { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Lock the current visualization preset on add-on if selected from
    /// user
    ///
    /// @param[in] lockUnlock   If set to true is the preset is needed to lock
    ///                         there
    /// @return                 Return true if the lock becomes used from you
    virtual bool LockPreset(bool lockUnlock) { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Used to increase/decrease track rating in full screen playback
    ///
    /// Press info on track and use pageplus or pageminus on keyboard/remote.
    ///
    /// @param[in] plusMinus    If set to true is the rate increased, otherwise
    ///                         decreased
    /// @return                 Return true if the rate becomes used from you
    virtual bool RatePreset(bool plusMinus) { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Inform the visualization of the current album art image
    ///
    /// @param[in] albumart     Path to the current album art image
    /// @return                 Return true if the image becomes used from you
    virtual bool UpdateAlbumart(std::string albumart) { return false; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization
    /// @brief Inform the visualization of the current track's tag information
    ///
    /// @param[in] track        Visualization track information structure
    /// @return                 Return true if it becomes used from you
    virtual bool UpdateTrack(const VisTrack &track);

    //==========================================================================
    ///
    /// \defgroup cpp_kodi_addon_visualization_CB Information functions
    /// \ingroup cpp_kodi_addon_visualization
    /// @brief **To get info about device, display and few other parts**
    ///
    //@{

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization_CB
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
    /// @ingroup cpp_kodi_addon_visualization_CB
    /// @brief Returns the X position of the rendering window
    ///
    /// @return Position X in pixels
    ///
    inline int X() { return m_instance->props.x; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization_CB
    /// @brief Returns the Y position of the rendering window
    ///
    /// @return Position Y in pixels
    ///
    inline int Y() { return m_instance->props.y; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization_CB
    /// @brief Returns the width of the rendering window
    ///
    /// @return Width in pixels
    ///
    inline int Width() { return m_instance->props.width; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization_CB
    /// @brief Returns the height of the rendering window
    ///
    /// @return Height in pixels
    ///
    inline int Height() { return m_instance->props.height; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization_CB
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
    /// @ingroup cpp_kodi_addon_visualization_CB
    /// @brief To get the in `addon.xml` defined name of the add-on
    ///
    /// @return Add-on name
    ///
    inline std::string Name() { return m_instance->props.name; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization_CB
    /// @brief To get the full path where add-on is be installed
    ///
    /// @return Add-on installation path
    ///
    inline std::string Presets() { return m_instance->props.presets; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization_CB
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

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_addon_visualization_CB
    /// @brief
    ///
    /// @return
    ///
    inline std::string SubModule() { return m_instance->props.submodule; }
    //--------------------------------------------------------------------------
    //@}

  private:
    inline static void ADDON_Start(void* addonInstance, int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName)
    {
      static_cast<CInstanceVisualization*>(addonInstance)->Start(iChannels, iSamplesPerSec, iBitsPerSample, szSongName);
    }

    inline static void ADDON_AudioData(void* addonInstance, const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength)
    {
      static_cast<CInstanceVisualization*>(addonInstance)->AudioData(pAudioData, iAudioDataLength, pFreqData, iFreqDataLength);
    }

    inline static void ADDON_Render(void* addonInstance)
    {
      static_cast<CInstanceVisualization*>(addonInstance)->Render();
    }

    inline static void ADDON_GetInfo(void* addonInstance, VIS_INFO *info)
    {
      static_cast<CInstanceVisualization*>(addonInstance)->GetInfo(info->bWantsFreq, info->iSyncDelay);
    }

    inline static bool ADDON_OnAction(void* addonInstance, VIS_ACTION action, const void *param)
    {
      switch (action)
      {
        case VIS_ACTION_NEXT_PRESET:
          return static_cast<CInstanceVisualization*>(addonInstance)->NextPreset();
        case VIS_ACTION_PREV_PRESET:
          return static_cast<CInstanceVisualization*>(addonInstance)->PrevPreset();
        case VIS_ACTION_LOAD_PRESET:
          return static_cast<CInstanceVisualization*>(addonInstance)->LoadPreset(*(int*)param);
        case VIS_ACTION_RANDOM_PRESET:
          return static_cast<CInstanceVisualization*>(addonInstance)->RandomPreset();
        case VIS_ACTION_LOCK_PRESET:
          static_cast<CInstanceVisualization*>(addonInstance)->m_presetLockedByUser = !static_cast<CInstanceVisualization*>(addonInstance)->m_presetLockedByUser;
          return static_cast<CInstanceVisualization*>(addonInstance)->LockPreset(static_cast<CInstanceVisualization*>(addonInstance)->m_presetLockedByUser);
        case VIS_ACTION_RATE_PRESET_PLUS:
          return static_cast<CInstanceVisualization*>(addonInstance)->RatePreset(true);
        case VIS_ACTION_RATE_PRESET_MINUS:
          return static_cast<CInstanceVisualization*>(addonInstance)->RatePreset(false);
        case VIS_ACTION_UPDATE_ALBUMART:
          return static_cast<CInstanceVisualization*>(addonInstance)->UpdateAlbumart(static_cast<const char*>(param));
        case VIS_ACTION_UPDATE_TRACK:
          return static_cast<CInstanceVisualization*>(addonInstance)->UpdateTrack(*static_cast<const VisTrack*>(param));
        case VIS_ACTION_NONE:
        default:
          break;
      }
      return false;
    }

    inline static bool ADDON_HasPresets(void* addonInstance)
    {
      return static_cast<CInstanceVisualization*>(addonInstance)->HasPresets();
    }

    inline static unsigned int ADDON_GetPresets(void* addonInstance)
    {
      CInstanceVisualization* addon = static_cast<CInstanceVisualization*>(addonInstance);
      std::vector<std::string> presets;
      if (addon->GetPresets(presets))
      {
        for (auto it : presets)
          addon->m_instance->toKodi.TransferPreset(addon->m_instance->toKodi.kodiInstance, it.c_str());
      }

      return presets.size();
    }

    inline static unsigned int ADDON_GetPreset(void* addonInstance)
    {
      return static_cast<CInstanceVisualization*>(addonInstance)->GetPreset();
    }

    inline static unsigned int ADDON_GetSubModules(void* addonInstance)
    {
      CInstanceVisualization* addon = static_cast<CInstanceVisualization*>(addonInstance);
      std::vector<std::string> subModules;
      if (addon->GetSubModules(subModules))
      {
        for (auto it : subModules)
          addon->m_instance->toKodi.TransferSubmodule(addon->m_instance->toKodi.kodiInstance, it.c_str());
      }

      return subModules.size();
    }

    inline static bool ADDON_IsLocked(void* addonInstance)
    {
      return static_cast<CInstanceVisualization*>(addonInstance)->IsLocked();
    }

    bool m_presetLockedByUser;
    sAddonInstance_Visualization* m_instance;
  };

} /* namespace addon */
} /* namespace kodi */
} /* extern "C" */
#endif
