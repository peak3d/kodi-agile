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

#include "addon/AddonBase.h"

/*
 * #include <kodi/General.h>
 * -------------------------
 * 
 * Addon header file to bring general interface functions to call inside Kodi.
 */

extern "C"
{

/*
 * For interface between add-on and kodi.
 * 
 * In this structure becomes the addresses of functions inside Kodi stored who
 * then available for the add-on to call.
 * 
 * All function pointers there are used by the C++ interface functions below.
 * You find the set of them on xbmc/addons/interfaces/kodi/General.cpp
 * 
 * Note: For add-on development itself not needed, thats why with '*' here in
 * text.
 */
typedef struct sAddonToKodiFuncTable_kodi
{
  char* (*get_addon_info)(void* kodiInstance, const char* id);
  bool (*get_setting)(void* kodiInstance, const char* settingName, void *settingValue, bool global);
  void (*open_settings_dialog)(void* kodiInstance);
  void (*queue_notification)(void* kodiInstance, const int type, const char* message);
  void (*queue_notification_from_type)(void* kodiInstance, const int type, const char* caption, const char* description, unsigned int displayTime, bool withSound, unsigned int messageTime);
  void (*queue_notification_with_image)(void* kodiInstance, const char* aImageFile, const char* caption, const char* description, unsigned int displayTime, bool withSound, unsigned int messageTime);
  void (*get_md5)(void* kodiInstance, const char* text, char& md5);
  char* (*unknown_to_utf8)(void* kodiInstance, const char* source, bool& ret, bool failOnBadChar);
  char* (*get_localized_string)(void* kodiInstance, long dwCode);
  void (*get_language)(void* kodiInstance, char& language, unsigned int& iMaxStringSize, int format, bool region);
  void (*get_dvd_menu_language)(void* kodiInstance, char &language, unsigned int &iMaxStringSize);
  bool (*start_server)(void* kodiInstance, int typ, bool start, bool wait);
  void (*audio_suspend)(void* kodiInstance);
  void (*audio_resume)(void* kodiInstance);
  float (*get_volume)(void* kodiInstance, bool percentage);
  void (*set_volume)(void* kodiInstance, float value, bool isPercentage);
  bool (*is_muted)(void* kodiInstance);
  void (*toggle_mute)(void* kodiInstance);
  void (*enable_nav_sounds)(void* kodiInstance, bool yesNo);
  long (*get_optical_state)(void* kodiInstance);
  bool (*eject_optical_drive)(void* kodiInstance);
  void (*kodi_version)(void* kodiInstance, char*& compile_name, int& major, int& minor, char*& revision, char*& tag, char*& tagversion);
  void (*kodi_quit)(void* kodiInstance);
  void (*htpc_shutdown)(void* kodiInstance);
  void (*htpc_restart)(void* kodiInstance);
  void (*execute_script)(void* kodiInstance, const char* script);
  void (*execute_builtin)(void* kodiInstance, const char* function, bool wait);
  char* (*execute_jsonrpc)(void* kodiInstance, const char* jsonrpccommand);
  char* (*get_region)(void* kodiInstance, const char* id);
  long (*get_free_mem)(void* kodiInstance);
  int  (*get_global_idle_time)(void* kodiInstance);
  char* (*translate_path)(void* kodiInstance, const char* path);
} sAddonToKodiFuncTable_kodi;


//==============================================================================
// \ingroup
/// @brief For kodi::QueueNotification() used message types
///
typedef enum QueueMsg
{
  /// Show info notification message
  QUEUE_INFO,
  /// Show warning notification message
  QUEUE_WARNING,
  /// Show error notification message
  QUEUE_ERROR
} QueueMsg;
//------------------------------------------------------------------------------


//==============================================================================
// \ingroup
/// @enum dvd_state State values about optical drive
///
typedef enum DVDState
{
  ///
  ADDON_DRIVE_NOT_READY = 0x01,
  ///
  ADDON_DRIVE_CLOSED_NO_MEDIA = 0x03,
  ///
  ADDON_TRAY_OPEN = 0x10,
  ///
  ADDON_TRAY_CLOSED_NO_MEDIA = 0x40,
  ///
  ADDON_TRAY_CLOSED_MEDIA_PRESENT = 0x60
} DVDState;
//------------------------------------------------------------------------------


//============================================================================
// \ingroup
/// @brief For kodi::Version used structure
///
typedef struct KodiVersion
{
  /// Application name, normally 'Kodi'
  std::string compile_name;
  /// Major code version of Kodi
  int major;
  /// Minor code version of Kodi
  int minor;
  /// The Revision contains a id and the build date, e.g. 2015-11-30-74edffb-dirty
  std::string revision;
  /// The version canditate e.g. alpha, beta or release
  std::string tag;
  /// The revision of tag before
  std::string tag_revision;
} KodiVersion;
//----------------------------------------------------------------------------

  
//==============================================================================
// \ingroup
/// @brief Format codes to get string from them.
///
typedef enum LangFormats
{
  /// two letter code as defined in ISO 639-1
  LANG_FMT_ISO_639_1,
  /// three letter code as defined in ISO 639-2/T or ISO 639-2/B
  LANG_FMT_ISO_639_2,
  /// full language name in English
  LANG_FMT_ENGLISH_NAME
} LangFormats;
//------------------------------------------------------------------------------

  
//==============================================================================
// \ingroup
/// @brief Kodi server identificators
///
typedef enum EServers
{
  /// [To control Kodi's builtin webserver](http://kodi.wiki/view/Webserver)
  ADDON_ES_WEBSERVER = 1,
  /// [AirPlay is a proprietary protocol stack/suite developed by Apple Inc.](http://kodi.wiki/view/AirPlay)
  ADDON_ES_AIRPLAYSERVER,
  /// [Control JSON-RPC HTTP/TCP socket-based interface](http://kodi.wiki/view/JSON-RPC_API)
  ADDON_ES_JSONRPCSERVER,
  /// [UPnP client (aka UPnP renderer)](http://kodi.wiki/view/UPnP/Client)
  ADDON_ES_UPNPRENDERER,
  /// [Control built-in UPnP A/V media server (UPnP-server)](http://kodi.wiki/view/UPnP/Server)
  ADDON_ES_UPNPSERVER,
  /// [Set eventServer part that accepts remote device input on all platforms](http://kodi.wiki/view/EventServer)
  ADDON_ES_EVENTSERVER,
  /// [Control Kodi's Avahi Zeroconf](http://kodi.wiki/view/Zeroconf)
  ADDON_ES_ZEROCONF
} EServers;
//------------------------------------------------------------------------------

} /* extern "C" */

namespace kodi
{

  //============================================================================
  ///
  // \ingroup
  /// @brief Get a string settings value for this add-on.
  ///
  /// \warning For this function and others below is no access on Kodi about
  /// passwords, paths and network parts allowed!
  /// Text input elements allow a user to input text in various formats. The
  /// "label" attribute must contain an id from the language file that indicates
  /// which text to display for the input field.
  /// To get path from add-on can be from <tt>GetSettingString</tt> the
  /// <tt>settingValue</tt> <a><b>__addonpath__</b></a> used.
  ///
  /// @param[in] settingName    The name of the setting to get.
  /// @param[out] settingValue  The value.
  /// @param[in] global         [opt] If set to true becomes Kodi itself asked 
  ///                           about a setting <em>(default is <b><c>false</c></b>)</em>
  /// @return                   true if successfull done
  ///
  /// <b><em>String type examples for add-on</em></b> <tt>./resources/settings.xml</tt>
  ///   - <tt>text</tt>
  ///     - Allow a user to enter one line of text.
  ///        \code{.xml}
  ///        <setting id="addon.language" type="text" label="30523" default="en-US"/>
  ///        \endcode
  ///       - <b><em><tt>id="string"</tt></em></b> (required) - the name of the setting.
  ///       - <b><em><tt>label="id"</tt></em></b> (required) - an id from the language file that indicates which text to display.
  ///       - <b><em><tt>option="hidden"|"urlencoded"</tt></em></b> (optional)
  ///         - if set to <b><em>"hidden"</em></b>, each characters entered by the user will be obfuscated with an asterisks ("*"), as is usual for entering passwords.
  ///         - if set to <b><em>"urlencoded"</em></b>, each non-alphanumeric characters entered by the user will be "escaped" using %XX encoding.
  ///       - <b><em><tt>default="value"</tt></em></b> (optional) - the default value.
  ///
  ///   - <tt>folder</tt>
  ///     - Allow the user to browse for and select a folder.
  ///        \code{.xml}
  ///        <setting id="iconpath" type="folder" source="files" label="30048" default="" />
  ///        <setting id="download-folder" type="folder" label="32010"/>
  ///        <setting id="image_path" type="folder" source="pictures" label="32012" visible="eq(-2,1)" default=""/>
  ///        <setting label="32033" type="folder" id="folder" source="auto" option="writeable"/>
  ///        \endcode
  ///       - <b><em><tt>id="string"</tt></em></b> (required) - the name of the setting
  ///       - <b><em><tt>label="id"</tt></em></b> (required) - an id from the language file that indicates which text to display.
  ///       - <b><em><tt>source="auto"|"images"|...</tt></em></b> (optional, default="auto") - select a starting folder for the browse dialog.
  ///        | Source types      | Description                            |
  ///        |:-----------------:|----------------------------------------|
  ///        | music             | Include music related sources          |
  ///        | video             | Include video related sources          |
  ///        | pictures          | Include pictures related sources       |
  ///        | programs          | Include praograms related sources      |
  ///        | local             | Include local system related sources   |
  ///
  ///       - <b><em><tt>value="value"</tt></em></b> (optional, default="") - the default value.
  ///       - <b><em><tt>option="writeable"</tt></em></b> (optional, default="") - the user can be allowed to create and select new folders by setting this argument.
  ///
  ///   - <tt>file</tt>
  ///     - To select file by the user.
  ///        \code{.xml}
  ///        <setting id="rtmp_binary" type="file" label="103" default="" />
  ///        \endcode
  ///
  ///   - <tt>ipaddress</tt>
  ///     - To use setting for ip address with the related dialog.
  ///        \code{.xml}
  ///        <setting id="hostip" enable="eq(-1,true)" type="ipaddress" label="32302" default="127.0.0.1" subsetting="true" />
  ///        \endcode
  ///       - <b><em><tt>id="string"</tt></em></b> (required) - the name of the setting.
  ///       - <b><em><tt>label="id"</tt></em></b> (required) - an id from the language file that indicates which text to display.
  ///       - <b><em><tt>default="value"</tt></em></b> (optional) - the default value.
  ///
  ///   - <tt>fileenum</tt>
  ///     - Display files in a folder as an enum selector.
  ///        \code{.xml}
  ///        <-- Example: List sub-folders of the 'resources' folder for this add-on in the settings. --/>
  ///        <setting id="myenum" type="fileenum" values="resources" mask="/"/>
  ///        <setting id="trailer.ratingMax" label="32062" type="fileenum" values="$PROFILE/settings/ratings" default="MPAA" mask="/"/>
  ///        \endcode
  ///       - <b><em><tt>mask="value"</tt></em></b> - filter selectable files. Examples: "/" - display only folders. "*.txt" - display only .txt
  ///         files.
  ///       - <b><em><tt>option="hideext"</tt></em></b> - hide file extensions.
  ///
  ///   - <tt>labelenum</tt>
  ///     - Shows a spin control about given string values and use a string value from selection.
  ///        \code{.xml}
  ///        <setting id="bitrate" type="labelenum" label="30003" default="Max" values="Min|Max|Select" />
  ///        <setting id="trailer.quality" label="32067" type="labelenum" values="480p|720p|1080p" default="720p" />
  ///        <setting id="scope" label="32092" type="labelenum" values="16:9|2.35:1|2.40:1" default="0" />
  ///        <setting id="quality" type="labelenum" label="$LOCALIZE[622]" values="720p|480p" default="720p" />
  ///        \endcode
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// <b>Code example:</b>
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  ///
  /// std::string bitrate = "Max";
  /// if (!kodi::GetSettingString("bitrate", bitrate))
  ///   kodi::Log(LOG_ERROR, "Couldn't get 'bitrate' setting, falling back to default");
  /// ~~~~~~~~~~~~~
  ///
  inline bool GetSettingString(const std::string& settingName, std::string& settingValue, bool global = false)
  {
    char * buffer = (char*) malloc(1024);
    buffer[0] = 0; /* Set the end of string */
    bool ret = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_setting
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      settingName.c_str(),
      buffer,
      global
    );
    if (ret)
      settingValue = buffer;
    free(buffer);
    return ret;
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Numeric input elements allow a user to enter a number. The "label" 
  /// attribute must contain an id from the language file that indicates which
  /// text to display for the input field.
  ///
  /// @param[in] settingName    The name of the setting to get.
  /// @param[out] settingValue  The value.
  /// @param[in] global         [opt] If set to true becomes Kodi itself asked
  ///                           about a setting <em>(default is <b><c>false</c></b>)</em>
  /// @return                   true if successfull done
  ///
  /// <b><em>Integer type examples for add-on</em></b> <tt>./resources/settings.xml</tt>
  ///   - <tt>number</tt>
  ///     - To get a numeric entry
  ///        \code{.xml}
  ///        <setting id="kodion.view.default" type="number" label="30027" default="50"/>
  ///        \endcode
  ///       - <b><em><tt>id="string"</tt></em></b> (required) - the name of the setting.
  ///       - <b><em><tt>label="id"</tt></em></b> (required) - an id from the language file that indicates which text to display.
  ///       - <b><em><tt>default="value"</tt></em></b> (optional) - the default value.
  ///
  ///   - <tt>enum</tt>
  ///     - Shows a spin control about given string values and use a integer value from selection
  ///        \code{.xml}
  ///        <setting id="kodion.video.quality" type="enum" label="30010" enable="eq(1,false)" lvalues="30016|30017|30011|30012|30013" default="3" />
  ///        <setting id="notification_length" type="enum" label="101" values="1|2|3|4|5|6|7|8|9|10" default="5" />
  ///        <setting id="time-of-day" type="enum" values="All|Day only|Night only|Smart" label="32006" visible="true" default="0"/>
  ///        \endcode
  ///       - <b><em><tt>id="string"</tt></em></b> (required) - the name of the setting
  ///       - <b><em><tt>label="id"</tt></em></b> (required) - an id from the language file that indicates which text to display.
  ///       - <b><em><tt>values="value1[|value2[...]]"</tt></em></b> - or -
  ///       - <b><em><tt>lvalues="id1[|id2[...]]"</tt></em></b> (required):
  ///          - A list of values or language file ids from which the user can choose.
  ///          - values="$HOURS" is special case to select hour ( works only for type="enum" )
  ///       - <b><em><tt>sort="yes"</tt></em></b> - sorts labels ( works only for type="labelenum" )
  ///
  ///   - <tt>slider</tt>
  ///     - Allows the user to enter a number using a horizontal sliding bar.
  ///        \code{.xml}
  ///        <setting id="kodion.cache.size" type="slider" label="30024" default="5" range="1,1,20" option="int"/>
  ///        <setting label="32053" type="slider" id="limit" default="20" range="5,5,100" option="int" />
  ///        <setting label="32053" type="slider" id="limit" default="20" range="0,100"/>
  ///        \endcode
  ///       - <b><em><tt>id="string"</tt></em></b> (required) - the name of the setting.
  ///       - <b><em><tt>label="id"</tt></em></b> (required) - an id from the language file that indicates which text to display.
  ///       - <b><em><tt>range="min[,step],max"</tt></em></b> (required) - specify the range of valid values.
  ///       - <b><em><tt>option="int"|"float"|"percent"</tt></em></b> (required) - specifies whether to allow the user to choose between
  ///         integers, floating point numbers or a percentage.
  ///       - <b><em><tt>default="value"</tt></em></b> (optional) - the default value.
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// <b>Code example:</b>
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  ///
  /// int cachesize = 5;
  /// if (!kodi::GetSettingInt("kodion.cache.size", cachesize))
  ///   kodi::Log(LOG_ERROR, "Couldn't get 'kodion.cache.size' setting, falling back to default");
  /// ~~~~~~~~~~~~~
  ///
  inline bool GetSettingInt(const std::string& settingName, int& settingValue, bool global = false)
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_setting
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      settingName.c_str(),
      &settingValue,
      global
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Get a boolean settings value for this add-on.
  ///
  /// @param[in] settingName    The name of the setting to get.
  /// @param[out] settingValue  The value.
  /// @param[in] global         [opt] If set to true becomes Kodi itself asked 
  ///                           about a setting <em>(default is <b><c>false</c></b>)</em>
  /// @return                   true if successfull done
  ///
  /// <b><em>Boolean type examples for add-on</em></b> <tt>./resources/settings.xml</tt>
  ///   - <tt>bool</tt>
  ///     - Shows a boolean check point to settings dialog
  ///        \code{.xml}
  ///        <setting id="kodion.setup_wizard" type="bool" label="30025" default="true"/>
  ///        <setting id="show-notifications" type="bool" label="32018" default="true" />
  ///        \endcode
  ///       - <b><em><tt>id="string"</tt></em></b> (required) - the name of the setting
  ///       - <b><em><tt>label="id"</tt></em></b> (required) - an id from the language file that indicates which text to display.
  ///       - <b><em><tt>default="true"|"false"</tt></em></b> (optional) - the default value.
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// <b>Code example:</b>
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  ///
  /// bool show_notifications = true;
  /// if (!kodi::GetSettingBoolean("show-notifications", show_notifications))
  ///   kodi::Log(LOG_ERROR, "Couldn't get 'show-notifications' setting, falling back to default");
  /// ~~~~~~~~~~~~~
  ///
  inline bool GetSettingBoolean(const std::string& settingName, bool& settingValue, bool global = false)
  { 
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_setting
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      settingName.c_str(),
      &settingValue,
      global
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Get a float settings value for this add-on.
  ///
  /// @param[in] settingName  The name of the setting to get.
  /// @param[in] settingValue The value.
  /// @param[in] global       [opt] If set to true becomes Kodi itself asked 
  ///                         about a setting <em>(default is <b><c>false</c></b>)</em>
  /// @return  true if successful done
  ///
  /// <b><em>Floating type examples for add-on</em></b> <tt>./resources/settings.xml</tt>
  ///   - <tt>slider</tt>
  ///     - Shows a moveable slider control to get values from range
  ///        \code{.xml}
  ///        <setting id="kodi.amplication.volume" type="slider" label="30024" default="1.0" range="0.0,0.1,2.0" option="float"/>
  ///        \endcode
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// <b>Code example:</b>
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  ///
  /// float volume = 1.0;
  /// if (!kodi::GetSettingFloat("kodi.amplication.volume", volume))
  ///   kodi::Log(LOG_ERROR, "Couldn't get 'kodi.amplication.volume' setting, falling back to default");
  /// ~~~~~~~~~~~~~
  ///
  inline bool GetSettingFloat(const std::string& settingName, float& settingValue, bool global = false)
  { 
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_setting
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      settingName.c_str(),
      &settingValue,
      global
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Opens this Add-Ons settings dialog.
  ///
  ///
  /// --------------------------------------------------------------------------
  ///
  ///  **Example:**
  ///  ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  ///  ..
  ///  kodi::OpenSettings();
  ///  ..
  ///  ~~~~~~~~~~~~~
  ///
  inline void OpenSettings()
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->open_settings_dialog
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Writes the C string pointed by format in the GUI. If format includes
  /// format specifiers (subsequences beginning with %), the additional arguments
  /// following format are formatted and inserted in the resulting string replacing
  /// their respective specifiers.
  ///
  /// After the format parameter, the function expects at least as many additional
  /// arguments as specified by format.
  ///
  /// @param[in] type          The message type.
  ///  |  enum code:    | Description:                      |
  ///  |---------------:|-----------------------------------|
  ///  |  QUEUE_INFO    | Show info notification message    |
  ///  |  QUEUE_WARNING | Show warning notification message |
  ///  |  QUEUE_ERROR   | Show error notification message   |
  /// @param[in] format        The format of the message to pass to display in Kodi.
  ///                      C string that contains the text to be written to the stream.
  ///                      It can optionally contain embedded format specifiers that are
  ///                      replaced by the values specified in subsequent additional
  ///                      arguments and formatted as requested.
  ///  |  specifier | Output                                             | Example
  ///  |------------|----------------------------------------------------|------------
  ///  |  d or i    | Signed decimal integer                             | 392
  ///  |  u         | Unsigned decimal integer                           | 7235
  ///  |  o         | Unsigned octal                                     | 610
  ///  |  x         | Unsigned hexadecimal integer                       | 7fa
  ///  |  X         | Unsigned hexadecimal integer (uppercase)           | 7FA
  ///  |  f         | Decimal floating point, lowercase                  | 392.65
  ///  |  F         | Decimal floating point, uppercase                  | 392.65
  ///  |  e         | Scientific notation (mantissa/exponent), lowercase | 3.9265e+2
  ///  |  E         | Scientific notation (mantissa/exponent), uppercase | 3.9265E+2
  ///  |  g         | Use the shortest representation: %e or %f          | 392.65
  ///  |  G         | Use the shortest representation: %E or %F          | 392.65
  ///  |  a         | Hexadecimal floating point, lowercase              | -0xc.90fep-2
  ///  |  A         | Hexadecimal floating point, uppercase              | -0XC.90FEP-2
  ///  |  c         | Character                                          | a
  ///  |  s         | String of characters                               | sample
  ///  |  p         | Pointer address                                    | b8000000
  ///  |  %         | A % followed by another % character will write a single % to the stream. | %
  ///
  /// The length sub-specifier modifies the length of the data type. This is a chart
  /// showing the types used to interpret the corresponding arguments with and without
  /// length specifier (if a different type is used, the proper type promotion or
  /// conversion is performed, if allowed):
  ///  | length| d i           | u o x X               | f F e E g G a A | c     | s       | p       | n               |
  ///  |-------|---------------|-----------------------|-----------------|-------|---------|---------|-----------------|
  ///  | (none)| int           | unsigned int          | double          | int   | char*   | void*   | int*            |
  ///  | hh    | signed char   | unsigned char         |                 |       |         |         | signed char*    |
  ///  | h     | short int     | unsigned short int    |                 |       |         |         | short int*      |
  ///  | l     | long int      | unsigned long int     |                 | wint_t| wchar_t*|         | long int*       |
  ///  | ll    | long long int | unsigned long long int|                 |       |         |         | long long int*  |
  ///  | j     | intmax_t      | uintmax_t             |                 |       |         |         | intmax_t*       |
  ///  | z     | size_t        | size_t                |                 |       |         |         | size_t*         |
  ///  | t     | ptrdiff_t     | ptrdiff_t             |                 |       |         |         | ptrdiff_t*      |
  ///  | L     |               |                       | long double     |       |         |         |                 |
  ///  <b>Note:</b> that the c specifier takes an int (or wint_t) as argument, but performs the proper conversion to a char value
  ///  (or a wchar_t) before formatting it for output.
  /// @param[in] ... (additional arguments) Depending on the format string, the function
  ///            may expect a sequence of additional arguments, each containing a value
  ///            to be used to replace a format specifier in the format string (or a pointer
  ///            to a storage location, for n).
  ///            There should be at least as many of these arguments as the number of values specified
  ///            in the format specifiers. Additional arguments are ignored by the function.
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::QueueFormattedNotification(QUEUE_WARNING, "I'm want to inform you, here with a test call to show '%s'", "this");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void QueueFormattedNotification(const QueueMsg type, const char* format, ... )
  {
    va_list args;
    char buffer[16384];
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->queue_notification
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      type,
      buffer
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Queue a notification in the GUI.
  ///
  /// @param[in] type          The message type.
  ///  |  enum code:    | Description:                      |
  ///  |---------------:|-----------------------------------|
  ///  |  QUEUE_INFO    | Show info notification message    |
  ///  |  QUEUE_WARNING | Show warning notification message |
  ///  |  QUEUE_ERROR   | Show error notification message   |
  /// @param[in] caption       Header Name
  /// @param[in] description   Message to display on Kodi
  /// @param[in] displayTime   [opt] The time how long message is displayed <b>(default 5 sec)</b>
  /// @param[in] withSound     [opt] if true also warning sound becomes played <b>(default with sound)</b>
  /// @param[in] messageTime   [opt] how many milli seconds start show of notification <b>(default 1 sec)</b>
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::QueueNotification(QUEUE_INFO, "I'm want to inform you", "Here with a test call", 3000, false, 1000);
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void QueueNotification(const QueueMsg type, const std::string& caption, const std::string& description, unsigned int displayTime = 5000, bool withSound = true, unsigned int messageTime = 1000)
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->queue_notification_from_type
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      type, caption.c_str(), description.c_str(),
      displayTime, withSound, messageTime
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Queue a notification in the GUI.
  ///
  /// @param[in] caption       Header Name
  /// @param[in] description   Message to display on Kodi
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::QueueNotification("I'm want to inform you", "Here with a test call");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void QueueNotification(const std::string& caption, const std::string& description)
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->queue_notification_with_image
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      "", caption.c_str(), description.c_str(),
      5000, false, 1000
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Queue a notification in the GUI.
  ///
  /// @param[in] aImageFile    The image file to show on message
  /// @param[in] caption       Header Name
  /// @param[in] description   Message to display on Kodi
  /// @param[in] displayTime   [opt] The time how long message is displayed in ms <b>(default 5 sec)</b>
  /// @param[in] withSound     [opt] true also warning sound becomes played <b>(default with sound)</b>
  /// @param[in] messageTime   [opt] in how many milli seconds start show of notification in ms <b>(default 1 sec)</b>
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::QueueNotification("./myImage.png", "I'm want to inform you", "Here with a test call", 3000, true, 1000);
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void QueueNotification(const std::string& aImageFile, const std::string& caption, const std::string& description, unsigned int displayTime = 5000, bool withSound = true, unsigned int messageTime = 1000)
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->queue_notification_with_image
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      aImageFile.c_str(),
      caption.c_str(), description.c_str(),
      displayTime, withSound, messageTime
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Get the MD5 digest of the given text
  ///
  /// @param[in]  text  text to compute the MD5 for
  /// @param[out] md5   Returned MD5 digest
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// std::string md5;
  /// kodi::GetMD5("Make me as md5", md5);
  /// fprintf(stderr, "My md5 digest is: '%s'", md5.c_str());
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void GetMD5(const std::string& text, std::string& md5)
  {
    char* md5ret = (char*)malloc(40*sizeof(char)); // md5 size normally 32 bytes
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_md5
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      text.c_str(), *md5ret
    );
    md5 = md5ret;
    free(md5ret);
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Translate a string with an unknown encoding to UTF8.
  ///
  /// @param[in]  stringSrc       The string to translate.
  /// @param[out] utf8StringDst   The translated string.
  /// @param[in]  failOnBadChar   [opt] return failed if bad character is inside <em>(default is <b><c>false</c></b>)</em>
  /// @return                     true if OK
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// std::string ret;
  /// if (!kodi::UnknownToUTF8("test string", ret, true))
  ///   fprintf(stderr, "Translation to UTF8 failed!\n");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline bool UnknownToUTF8(const std::string& stringSrc, std::string& utf8StringDst, bool failOnBadChar = false)
  {
    bool ret = false;
    char* retString = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->unknown_to_utf8
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      stringSrc.c_str(),
      ret,
      failOnBadChar
    );
    if (retString != nullptr)         
    {
      if (ret)
        utf8StringDst = retString;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string
      (
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
       retString
      );
    }
    return ret;
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Returns an addon's localized 'unicode string'.
  ///
  /// @param[in] labelId    for string you want to localize.
  /// @param[in] defaultStr [opt] The default message, also helps to identify 
  ///                       the code that is used <em>(default is
  ///                       <b><c>empty</c></b>)</em>
  /// @return               The localized message, or default if the add-on
  ///                       helper fails to return a message
  ///
  /// @note Label id's \b 30000 to \b 30999 and \b 32000 to \b 32999 are related
  /// to own add-on included strings inside 
  /// <b>./resources/language/resource.language.??_??/strings.po</b>
  /// all others are from Kodi itself.
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// std::string str = kodi::GetLocalizedString(30005, "Use me as default");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline std::string GetLocalizedString(uint32_t labelId, const std::string& defaultStr = "")
  {
    std::string retString = defaultStr;
    char* strMsg = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_localized_string
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      labelId
    );
    if (strMsg != nullptr)
    {
      if (std::strlen(strMsg))
        retString = strMsg;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string
      (
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
        strMsg
      );
    }
    return retString;
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Returns the active language as a string.
  ///
  /// @param[in] format Used format of the returned language string
  ///  | enum code:            | Description:                                               |
  ///  |----------------------:|------------------------------------------------------------|
  ///  | LANG_FMT_ENGLISH_NAME | full language name in English (Default)                    |
  ///  | LANG_FMT_ISO_639_1    | two letter code as defined in ISO 639-1                    |
  ///  | LANG_FMT_ISO_639_2    | three letter code as defined in ISO 639-2/T or ISO 639-2/B |
  /// @param[in] region [opt] append the region delimited by "-" of the language (setting) to the returned language string <em>(default is <b><c>false</c></b>)</em>
  /// @return active language
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// std::string language = kodi::GetLanguage(LANG_FMT_ISO_639_1, false);
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline std::string GetLanguage(LangFormats format = LANG_FMT_ENGLISH_NAME, bool region = false)
  {
    std::string language;
    language.resize(256);
    unsigned int size = (unsigned int)language.capacity();
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_language
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      language[0],
      size,
      format,
      region
    );
    language.resize(size);
    language.shrink_to_fit();
    return language;
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Get the DVD menu language.
  ///
  /// @return The DVD menu langauge, or empty if unknown
  ///
  inline std::string GetDVDMenuLanguage()
  {
    std::string language;
    language.resize(16);
    unsigned int size = (unsigned int)language.capacity();
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_dvd_menu_language
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      language[0],
      size
    );
    language.resize(size);
    language.shrink_to_fit();
    return language;
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Start or stop a server.
  ///
  /// @param[in] typ Used format of the returned language string
  /// | enum code:             | Description:                                               |
  /// |-----------------------:|------------------------------------------------------------|
  /// | ADDON_ES_WEBSERVER     | [To control Kodi's builtin webserver](http://kodi.wiki/view/Webserver)
  /// | ADDON_ES_AIRPLAYSERVER | [AirPlay is a proprietary protocol stack/suite developed by Apple Inc.](http://kodi.wiki/view/AirPlay)
  /// | ADDON_ES_JSONRPCSERVER | [Control JSON-RPC HTTP/TCP socket-based interface](http://kodi.wiki/view/JSON-RPC_API)
  /// | ADDON_ES_UPNPRENDERER  | [UPnP client (aka UPnP renderer)](http://kodi.wiki/view/UPnP/Client)
  /// | ADDON_ES_UPNPSERVER    | [Control built-in UPnP A/V media server (UPnP-server)](http://kodi.wiki/view/UPnP/Server)
  /// | ADDON_ES_EVENTSERVER   | [Set eventServer part that accepts remote device input on all platforms](http://kodi.wiki/view/EventServer)
  /// | ADDON_ES_ZEROCONF      | [Control Kodi's Avahi Zeroconf](http://kodi.wiki/view/Zeroconf)
  /// @param[in] start start (True) or stop (False) a server
  /// @param[in] wait [opt] wait on stop before returning (not supported by all servers) <em>(default is <b><c>false</c></b>)</em>
  /// @return true if successfull done
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// if (!kodi::StartServer(ADDON_ES_WEBSERVER, true))
  ///   kodi::Log(LOG_ERROR, "Start of web server failed!");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline bool StartServer(EServers typ, bool start, bool wait = false)
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->start_server
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      (int)typ,
      start,
      wait
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Suspend Audio engine
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::AudioSuspend();
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void AudioSuspend()
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->audio_suspend
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Resume Audio engine
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::AudioResume();
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void AudioResume()
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->audio_resume
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }
  //----------------------------------------------------------------------------
  
  
  //============================================================================
  ///
  // \ingroup
  /// @brief Get the current global Kodi audio volume
  ///
  /// @param[in] percentage [opt] if set to false becomes amplication level returned
  /// @return The volume value
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// float percent = kodi::GetVolume(true);
  /// fprintf(stderr, "Current volume is set to %02f %%\n");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline float GetVolume(bool percentage = true)
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_volume
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      percentage
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Set the current global Kodi audio volume
  ///
  /// @param[in] value the volume to use
  /// @param[in] isPercentage [opt] if set to false becomes amplication level use
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::SetVolume(0.45, false);
  /// ...
  /// ~~~~~~~~~~~~~
  /// @b or
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::SetVolume(75.0, true);
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void SetVolume(float value, bool isPercentage = true)
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->set_volume
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      value,
      isPercentage
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Ask if Kodi audio is muted
  ///
  /// @return true if audio is muted
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// bool muted = kodi::IsMuted();
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline bool IsMuted()
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->is_muted
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Toggle the audio volume between on and off
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::ToggleMute();
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void ToggleMute()
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->toggle_mute
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }
  //----------------------------------------------------------------------------

  
  //============================================================================
  ///
  // \ingroup
  /// @brief Set Kodi's mute
  ///
  /// @param[in] mute with set to true becomes it muted otherwise audio is present
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::SetMute(true); // Enable muted audio
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void SetMute(bool mute)
  {
    if (::kodi::addon::CAddonBase::m_instance->toKodi.kodi->is_muted
        (
          ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
        ) != mute)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->toggle_mute
      (
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
      );
    }
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Enables/Disables nav sounds
  ///
  /// @param[in] yesNo          enable (True) or disable (False) nav sounds
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::EnableNavSounds(true);
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void EnableNavSounds(bool yesNo)
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->enable_nav_sounds
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      yesNo
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Returns the dvd state as an integer
  ///
  /// @return The current state of drive
  ///
  inline DVDState GetOpticalDriveState()
  {
    return (DVDState)::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_optical_state
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Send eject signal to optical drive
  ///
  /// @return true if successfull done
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// if (!kodi::EjectOpticalDrive())
  ///   kodi::Log(LOG_ERROR, "Eject of drive failed!");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline bool EjectOpticalDrive()
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->eject_optical_drive
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Get current Kodi informations and versions, returned data from the following
  /// <b><tt>kodi_version_t version; kodi::KodiVersion(version);</tt></b>
  /// is e.g.:
  /// ~~~~~~~~~~~~~{.cpp}
  /// version.compile_name = Kodi
  /// version.major        = 16
  /// version.minor        = 0
  /// version.revision     = 2015-11-30-74edffb-dirty
  /// version.tag          = beta
  /// version.tag_revision = 1
  /// ~~~~~~~~~~~~~
  ///
  /// @param[out] version structure to store data from kodi
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi_version_t version;
  /// kodi::KodiVersion(version);
  /// fprintf(stderr,
  ///     "kodi_version_t version;\n"
  ///     "kodi::KodiVersion(version);\n"
  ///     " - version.compile_name = %s\n"
  ///     " - version.major        = %i\n"
  ///     " - version.minor        = %i\n"
  ///     " - version.revision     = %s\n"
  ///     " - version.tag          = %s\n"
  ///     " - version.tag_revision = %s\n",
  ///             version.compile_name.c_str(), version.major, version.minor,
  ///             version.revision.c_str(), version.tag.c_str(), version.tag_revision.c_str());
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void KodiVersion(KodiVersion& version)
  {
    char* compile_name = nullptr;
    char* revision     = nullptr;
    char* tag          = nullptr;
    char* tag_revision = nullptr;

    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->kodi_version
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      compile_name,
      version.major,
      version.minor,
      revision,
      tag,
      tag_revision
    );
    if (compile_name != nullptr)
    {
      version.compile_name  = compile_name;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string
      (
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
        compile_name
      );
    }
    if (revision != nullptr)
    {
      version.revision = revision;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string
      (
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
        revision
      );
    }
    if (tag != nullptr)
    {
      version.tag = tag;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string
      (
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
        tag
      );
    }
    if (tag_revision != nullptr)
    {
      version.tag_revision = tag_revision;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string
      (
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
        tag_revision
      );
    }
  }
  //----------------------------------------------------------------------------

  
  //============================================================================
  ///
  // \ingroup
  /// @brief Tell Kodi to stop work, go to exit and stop his work.
  /// \warning Kodi is really quited!
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::KodiQuit();
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void KodiQuit()
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->kodi_quit
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }
  //----------------------------------------------------------------------------
  
  
  //============================================================================
  ///
  // \ingroup
  /// @brief Shutdown the htpc
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::HTPCShutdown();
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void HTPCShutdown()
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->htpc_shutdown
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Restart the htpc
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::HTPCRestart();
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void HTPCRestart()
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->htpc_restart
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }
  //----------------------------------------------------------------------------
  
  
  //============================================================================
  ///
  // \ingroup
  /// @brief Execute a python script
  ///
  /// @param[in] script filename to execute
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::ExecuteScript("special://home/scripts/update.py");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void ExecuteScript(const std::string& script)
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->execute_script
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      script.c_str()
    );
  }
  //----------------------------------------------------------------------------

  
  //============================================================================
  ///
  // \ingroup
  /// @brief Execute a built in Kodi function
  ///
  /// @param[in] function builtin function to execute
  /// @param[in] wait [opt] if true wait until finished
  ///
  ///
  /// See list of \ref page_List_of_built_in_functions "build in functions"
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::ExecuteBuiltin("RunXBE(c:\\avalaunch.xbe)");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void ExecuteBuiltin(const std::string& function, bool wait = false)
  {
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->execute_builtin
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      function.c_str(),
      wait
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Execute an JSONRPC command
  ///
  /// @param[in] jsonrpccommand jsonrpc command to execute
  /// @return  From jsonrpc returned string for command
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// std::string response = kodi::ExecuteJSONRPC('{ \"jsonrpc\": \"2.0\", \"method\": \"JSONRPC.Introspect\", \"id\": 1 }');
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline std::string ExecuteJSONRPC(const std::string& jsonrpccommand)
  {
    std::string strReturn;
    char* strMsg = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->execute_jsonrpc
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      jsonrpccommand.c_str()
    );
    if (strMsg != nullptr)
    {
      if (std::strlen(strMsg))
        strReturn = strMsg;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string
      (
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
        strMsg
      );
    }
    return strReturn;
  }

  //----------------------------------------------------------------------------

  
  //============================================================================
  ///
  // \ingroup
  /// @brief Returns your regions setting as a string for the specified id
  ///
  /// @param[in] id id of setting to return
  /// |              | Choices are  |              |
  /// |:------------:|:------------:|:------------:|
  /// |  dateshort   | time         | tempunit     |
  /// |  datelong    | meridiem     | speedunit    |
  ///
  /// @return settings string
  ///
  /// @warning Throws exception on wrong string value!
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// std::string timeFormat = kodi::GetRegion("time");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline std::string GetRegion(const std::string& id)
  {
    std::string strReturn;
    char* strMsg = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_region
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      id.c_str()
    );
    if (strMsg != nullptr)
    {
      if (std::strlen(strMsg))
        strReturn = strMsg;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string
      (
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
        strMsg
      );
    }
    return strReturn;
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Returns the amount of free memory in MB as an integer
  ///
  /// @return free memory
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// long freeMem = kodi::GetFreeMem();
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline long GetFreeMem()
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_free_mem
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }

  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Returns the elapsed idle time in seconds as an integer
  ///
  /// @return idle time
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// int time = kodi::GetGlobalIdleTime();
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline int GetGlobalIdleTime()
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_global_idle_time
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance
    );
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Returns the value of an addon property as a string
  ///
  /// @param[in] id id of the property that the module needs to access
  /// |              | Choices are  |              |
  /// |:------------:|:------------:|:------------:|
  /// |  author      | icon         | stars        |
  /// |  changelog   | id           | summary      |
  /// |  description | name         | type         |
  /// |  disclaimer  | path         | version      |
  /// |  fanart      | profile      |              |
  ///
  /// @return Returned information string
  ///
  /// @warning Throws exception on wrong string value!
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// std::string addonName = kodi::GetAddonInfo("name");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline std::string GetAddonInfo(const std::string& id)
  {
    std::string strReturn;
    char* strMsg = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->get_addon_info
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      id.c_str()
    );
    if (strMsg != nullptr)
    {
      if (std::strlen(strMsg))
        strReturn = strMsg;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string
      (
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
        strMsg
      );
    }
    return strReturn;
  }
  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Returns the translated path
  ///
  /// @param[in] path  string or unicode - Path to format
  /// @return      A human-readable string suitable for logging
  ///
  /// @note        Only useful if you are coding for both Linux and Windows.
  ///              e.g. Converts 'special://masterprofile/script_data' -> '/home/user/.kodi/UserData/script_data'
  ///              on Linux.
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// std::string path = kodi::TranslatePath("special://masterprofile/script_data");
  /// fprintf(stderr, "Translated path is: %s\n", path.c_str());
  /// ...
  /// ~~~~~~~~~~~~~
  /// or
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// fprintf(stderr, "Directory 'special://temp' is '%s'\n", kodi::TranslatePath("special://temp").c_str());
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline std::string TranslatePath(const std::string& path)
  {
    std::string strReturn;
    char* strMsg = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->translate_path
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      path.c_str()
    );
    if (strMsg != nullptr)
    {
      if (std::strlen(strMsg))
        strReturn = strMsg;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string
      (
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
        strMsg
      );
    }
    return strReturn;
  }

  //----------------------------------------------------------------------------


  //============================================================================
  ///
  // \ingroup
  /// @brief Translate an add-on status return code into a human-readable string
  ///
  /// @param[in] status The return code
  /// @return A human-readable string suitable for logging
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// std::string error = kodi::TranslateAddonStatus(ADDON_STATUS_PERMANENT_FAILURE);
  /// fprintf(stderr, "Error is: %s\n", error.c_str());
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline std::string TranslateAddonStatus(ADDON_STATUS status)
  {
    switch (status)
    {
      case ADDON_STATUS_OK:                 return "OK";
      case ADDON_STATUS_LOST_CONNECTION:    return "Lost connection";
      case ADDON_STATUS_NEED_RESTART:       return "Needs restart";
      case ADDON_STATUS_NEED_SETTINGS:      return "Needs settngs";
      case ADDON_STATUS_UNKNOWN:            return "Unknown";
      case ADDON_STATUS_NEED_SAVEDSETTINGS: return "Needs saved settings";
      case ADDON_STATUS_PERMANENT_FAILURE:  return "Permanent failure";
      default:
        break;
    }
    return "";
  }
  //----------------------------------------------------------------------------

} /* namespace kodi */
