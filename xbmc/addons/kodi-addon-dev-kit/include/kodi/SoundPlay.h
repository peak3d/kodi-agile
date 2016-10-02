#pragma once
/*
 *      Copyright (C) 2015 Team KODI
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

#include "AddonBase.h"

//==============================================================================
///
/// \defgroup cpp_kodi_SoundPlay class CSoundPlay
/// \ingroup cpp_kodi
/// @brief **Background sound playback**
///
/// This class is designed for playback of "wave" audio files to point for the
/// user with sound steps performed or to draw attention to warnings.
///
/// Further allowed this playback on a single selected channel which can be
/// used to identify or also to test. For larger and more extensive audio
/// system testing is recommended but the use of "CAddonAE_Stream" since
/// this is intended specifically for this purpose.
///
/// @warning class supports only <b>wave files (*.wav)</b> for play back!
///
/// It has the header \ref SoundPlay.h "#include <kodi/SoundPlay.h>" be included
/// to enjoy it.
///
///
/// ------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/SoundPlay.h>
/// #include <kodi/gui/DialogFileBrowser.h>
///
/// ...
///
/// void PlaySelectedWaveFile()
/// {
///   std::string filename;
///   if (kodi::gui::DialogFileBrowser::ShowAndGetFile("local", "*.wav",
///                                                    "Test File selection and playback of them",
///                                                    filename))
///   {
///     //
///     // The example performs a playback with the value "waitUntilEnd" from
///     // "kodi::CSoundPlay::Play(...)" is set to true. This waits for the call
///     // until the playback is complete, so that it is then also possible to
///     // use the class static as in this example.
///     //
///     kodi::CSoundPlay playback(filename);
///     playback->Play();
///   }
/// }
/// ~~~~~~~~~~~~~
///
//------------------------------------------------------------------------------

extern "C"
{

typedef struct sAddonToKodiFuncTable_kodi_audio
{
  void* (*soundplay_get_handle)(void* kodiInstance, const char *filename);
  void (*soundplay_release_handle)(void* kodiInstance, void* sndHandle);
  void (*soundplay_play)(void* kodiInstance, void* sndHandle);
  void (*soundplay_stop)(void* kodiInstance, void* sndHandle);
  void (*soundplay_set_channel)(void* kodiInstance, void* sndHandle, int channel);
  int (*soundplay_get_channel)(void* kodiInstance, void* sndHandle);
  void (*soundplay_set_volume)(void* kodiInstance, void* sndHandle, float volume);
  float (*soundplay_get_volume)(void* kodiInstance, void* sndHandle);
} sAddonToKodiFuncTable_kodi_audio;

namespace kodi
{

  class CSoundPlay
  {
  public:
    //==========================================================================
    ///
    /// @ingroup cpp_kodi_SoundPlay
    /// @brief Construct class
    ///
    /// Construct the class for background playback of given wave file.
    ///
    /// @param[in] filename The wave file to play
    ///
    CSoundPlay(const std::string& filename)
      : m_playHandle(nullptr)
    {
      m_playHandle = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_audio->soundplay_get_handle(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, filename.c_str());
      if (!m_playHandle)
        kodi::Log(LOG_FATAL, "CSoundPlay can't get callback table from KODI !!!");
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_SoundPlay
    /// @brief Destructor
    ///
    virtual ~CSoundPlay(void)
    {
      if (m_playHandle)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_audio->soundplay_release_handle(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_playHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_SoundPlay
    /// @brief Start playback
    ///
    /// Starts the playback of wave file. Normally returns this function.
    ///
    /// This function ordinarily comes after him begin the sound reproduction
    /// immediately again back around other steps to permit.
    ///
    void Play()
    {
      if (m_playHandle)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_audio->soundplay_play(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_playHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_SoundPlay
    /// @brief Stop playback
    ///
    /// Stops the playback of wave file
    ///
    void Stop()
    {
      if (m_playHandle)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_audio->soundplay_stop(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_playHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_SoundPlay
    /// @brief Set audio channel for playback
    ///
    /// To set channel position the for a single channel playback of wave file.
    /// To reset selection use "AUDIO_CH_NULL".
    ///
    /// @param[in] channel The channel identifier to play on
    ///
    void SetChannel(AudioChannel channel)
    {
      if (m_playHandle)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_audio->soundplay_set_channel(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_playHandle, channel);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_SoundPlay
    /// @brief Get the active channel for playback
    ///
    /// To set channel position the for a single channel playback of wave file.
    ///
    /// @return The channel where currently played
    ///
    AudioChannel GetChannel()
    {
      if (!m_playHandle)
        return AUDIO_CH_NULL;
      return (AudioChannel)::kodi::addon::CAddonBase::m_instance->toKodi.kodi_audio->soundplay_get_channel(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_playHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_SoundPlay
    /// @brief Set the volume
    ///
    /// To set the volume of wave file playback.
    ///
    /// @note Volume is independent from Kodi's normal volume control and only
    /// for the wave file. Also is the volume only on this class and not to all
    /// background sounds on Kodi.
    ///
    /// @param[in] volume The playback volume of wave file
    ///
    void SetVolume(float volume)
    {
      if (m_playHandle)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_audio->soundplay_set_volume(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_playHandle, volume);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_SoundPlay
    /// @brief Get the volume
    ///
    /// To get the volume of wave file playback.
    ///
    /// @note Volume is independent from Kodi's normal volume control and only
    /// for the wave file. Also is the volume only on this class and not to all
    /// background sounds on Kodi.
    ///
    /// @return The currently active volume of this wave file
    ///
    float GetVolume()
    {
      if (!m_playHandle)
        return 0.0f;
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_audio->soundplay_get_volume(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_playHandle);
    }
    //--------------------------------------------------------------------------

  private:
    void* m_playHandle;
  };

} /* namespace kodi */
} /* extern "C" */
