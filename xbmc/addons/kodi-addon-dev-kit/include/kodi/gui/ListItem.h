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

#include "../AddonBase.h"
#include "definitions.h"

extern "C"
{
namespace kodi
{
namespace gui
{

  ///
  /// \ingroup cpp_kodi_gui_CListItem
  /// \defgroup cpp_kodi_gui_CListItem_MusicInfoTag Functions - Music info tag
  ///

  ///
  /// \ingroup cpp_kodi_gui_CListItem
  /// \defgroup cpp_kodi_gui_CListItem_VideoInfoTag Functions - Video info tag
  ///

  ///
  /// \ingroup cpp_kodi_gui_CListItem
  /// \defgroup cpp_kodi_gui_CListItem_PictureInfoTag Functions - Picture info tag
  ///

  //============================================================================
  ///
  /// \defgroup cpp_kodi_gui_CListItem List Item
  /// \ingroup cpp_kodi_gui
  /// @brief \cpp_class{ kodi::gui::CListItem }
  /// **Selectable window list item**
  ///
  /// The list item control is used for creating item lists in Kodi
  ///
  /// The with \ref ListItem.h "#include <kodi/gui/ListItem.h>" given
  /// class is used to create a item entry for a list on window and to support it's
  /// control.
  ///

  //============================================================================
  ///
  /// \defgroup cpp_kodi_gui_CListItem_Defs Definitions, structures and enumerators
  /// \ingroup cpp_kodi_gui_CListItem
  /// @brief **Library definition values**
  ///

  class CListItem
  {
  public:
    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Class constructor with parameters
    ///
    /// @param[in] label                Item label
    /// @param[in] label2               Second Item label (if needed)
    /// @param[in] iconImage            Item icon image (if needed)
    /// @param[in] thumbnailImage       Thumbnail Image of item (if needed)
    /// @param[in] path                 Path to where item is defined
    ///
    CListItem(
      const std::string& label,
      const std::string& label2,
      const std::string& iconImage,
      const std::string& thumbnailImage,
      const std::string& path)
    {
      m_ListItemHandle = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.Create(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, label.c_str(),
                                                label2.c_str(), iconImage.c_str(),
                                                thumbnailImage.c_str(), path.c_str());
    }

    CListItem(GUIHANDLE listItemHandle)
    {
      m_ListItemHandle = listItemHandle;
    }

    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Class destructor
    ///
    virtual ~CListItem()
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.Destroy(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Returns the listitem label.
    ///
    /// @return                       Label of item
    ///
    std::string GetLabel()
    {
      std::string label;
      label.resize(1024);
      unsigned int size = (unsigned int)label.capacity();
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.GetLabel(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, label[0], size);
      label.resize(size);
      label.shrink_to_fit();
      return label.c_str();
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Sets the listitem's label.
    ///
    /// @param[in] label              string or unicode - text string.
    ///
    void SetLabel(const std::string& label)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetLabel(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, label.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Returns the second listitem label.
    ///
    /// @return                       Second label of item
    ///
    std::string GetLabel2()
    {
      std::string label;
      label.resize(1024);
      unsigned int size = (unsigned int)label.capacity();
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.GetLabel2(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, label[0], size);
      label.resize(size);
      label.shrink_to_fit();
      return label.c_str();
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Sets the listitem's label2.
    ///
    /// @param[in] label              string or unicode - text string.
    ///
    void SetLabel2(const std::string& label)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetLabel2(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, label.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To get current icon image of entry
    ///
    /// @return The current icon image path (if present)
    ///
    std::string GetIconImage()
    {
      std::string image;
      image.resize(1024);
      unsigned int size = (unsigned int)image.capacity();
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.GetIconImage(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, image[0], size);
      image.resize(size);
      image.shrink_to_fit();
      return image.c_str();
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To set icon image of entry
    ///
    /// @param image                    The image to use for.
    ///
    /// @note Alternative can be \ref SetArt used
    ///
    ///
    void SetIconImage(const std::string& image)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetIconImage(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, image.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To get current overlay image of entry
    ///
    /// @return The current overlay image path (if present)
    ///
    std::string GetOverlayImage()
    {
      std::string image;
      image.resize(1024);
      unsigned int size = (unsigned int)image.capacity();
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.GetOverlayImage(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, image[0], size);
      image.resize(size);
      image.shrink_to_fit();
      return image.c_str();
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To set overlay image of entry
    ///
    /// @param image                    The image to use for.
    /// @param bOnOff                   To switch image view on (true) or off (false)
    ///
    void SetOverlayImage(unsigned int image, bool bOnOff = false)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetOverlayImage(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, image, bOnOff);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To set thumbnail image of entry
    ///
    /// @warning Deprecated. Use setArt
    ///
    ///
    void SetThumbnailImage(const std::string& image)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetThumbnailImage(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, image.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Sets the listitem's art
    ///
    /// @param[in] type                 Type of Art to set
    /// @param[in] url                  The url to use for Art
    ///  - Some default art values (any string possible):
    ///  | value (type)  | Type                                              |
    ///  |:-------------:|:--------------------------------------------------|
    ///  | thumb         | string - image filename
    ///  | poster        | string - image filename
    ///  | banner        | string - image filename
    ///  | fanart        | string - image filename
    ///  | clearart      | string - image filename
    ///  | clearlogo     | string - image filename
    ///  | landscape     | string - image filename
    ///  | icon          | string - image filename
    ///
    void SetArt(const std::string& type, const std::string& url)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetArt(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type.c_str(), url.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Sets the listitem's art
    ///
    /// @param[in] type                 Type of Art to set
    ///  - Some default art values (any string possible):
    ///  | value (type)  | Type                                              |
    ///  |:-------------:|:--------------------------------------------------|
    ///  | thumb         | string - image filename
    ///  | poster        | string - image filename
    ///  | banner        | string - image filename
    ///  | fanart        | string - image filename
    ///  | clearart      | string - image filename
    ///  | clearlogo     | string - image filename
    ///  | landscape     | string - image filename
    ///  | icon          | string - image filename
    /// @return                         The url to use for Art
    ///
    std::string GetArt(const std::string& type)
    {
      std::string strReturn;
      char* strMsg = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.GetArt(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type.c_str());
      if (strMsg != nullptr)
      {
        if (std::strlen(strMsg))
          strReturn = strMsg;
        ::kodi::addon::CAddonBase::m_instance->toKodi.free_string(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, strMsg);
      }
      return strReturn;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Set a fallback image for art
    ///
    /// @param[in] from the type to fallback from
    /// @param[in] to the type to fallback to
    /// \sa SetArt
    void SetArtFallback(const std::string& from, const std::string& to)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetArtFallback(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, from.c_str(), to.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief
    ///
    /// @return                         true if present,, false if not
    ///
    bool HasArt(const std::string& type)
    {
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.HasArt(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Check item about presence of Icon
    ///
    /// @return                         true if present,, false if not
    ///
    bool HasIcon()
    {
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.HasIcon(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To check item for presence of a Overlay image
    ///
    /// @return                         true if present, false if not
    ///
    bool HasOverlay()
    {
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.HasOverlay(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To check item is a File
    ///
    /// @return                         true if is a file, false if not
    ///
    bool IsFileItem()
    {
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.IsFileItem(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To check item is a Folder
    ///
    /// @return                         true if yes, false if not
    ///
    bool IsFolder()
    {
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.IsFolder(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To control selection of item in list (also multiple selection,
    /// in list on serveral items possible).
    ///
    /// @param[in] bOnOff               if true becomes set as selected
    ///
    void Select(bool bOnOff)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.Select(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, bOnOff);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To check item is selected on list.
    ///
    /// @return                         true if selected, otherwise false
    ///
    bool IsSelected()
    {
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.IsSelected(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Sets a listitem property, similar to an infolabel.
    ///
    /// @param[in] key            string - property name.
    /// @param[in] value          string or unicode - value of property.
    ///
    /// @note Key is NOT case sensitive.
    ///       You can use the above as keywords for arguments and skip certain\n
    ///       optional arguments.\n
    ///       Once you use a keyword, all following arguments require the
    ///       keyword.
    ///
    /// Some of these are treated internally by Kodi, such as the
    /// <b>'StartOffset'</b> property, which is the offset in seconds at which to
    /// start playback of an item.  Others may be used in the skin to add
    /// extra information, such as <b>'WatchedCount'</b> for tvshow items
    ///
    void SetProperty(const std::string& key, const std::string& value)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetProperty(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, key.c_str(), value.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Returns a listitem property as a string, similar to an infolabel.
    ///
    /// @param[in] key                string - property name.
    /// @return                       string - List item property
    ///
    /// @note Key is NOT case sensitive.\n
    ///       You can use the above as keywords for arguments and skip certain
    ///       optional arguments.\n
    ///       Once you use a keyword, all following arguments require the
    ///       keyword.
    ///
    std::string GetProperty(const std::string& key)
    {
      std::string property;
      property.resize(1024);
      unsigned int size = (unsigned int)property.capacity();
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.GetProperty(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, key.c_str(), property[0], size);
      property.resize(size);
      property.shrink_to_fit();
      return property;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Clears the specific window property.
    ///
    /// @param[in] key                string - property name.
    ///
    /// @note Key is NOT case sensitive. Equivalent to setProperty(key,'')
    ///       You can use the above as keywords for arguments and skip certain
    ///       optional arguments.
    ///       Once you use a keyword, all following arguments require the
    ///       keyword.
    ///
    ///
    ///-----------------------------------------------------------------------
    ///
    /// **Example:**
    /// ~~~~~~~~~~~~~{.cpp}
    /// ..
    /// ..
    /// ~~~~~~~~~~~~~
    ///
    void ClearProperty(const std::string& key)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.ClearProperty(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, key.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Clears all window properties.
    ///
    ///
    ///-----------------------------------------------------------------------
    ///
    /// **Example:**
    /// ~~~~~~~~~~~~~{.cpp}
    /// ..
    /// ..
    /// ~~~~~~~~~~~~~
    ///
    void ClearProperties()
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.ClearProperties(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To check list item about presence of properties
    ///
    /// @return                         true if properties present
    ///
    bool HasProperties()
    {
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.HasProperties(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief To check list item about presence of searched key by name
    ///
    /// @param[in] key                  Property to check
    /// @return                         true if properties present
    ///
    bool HasProperty(const std::string& key)
    {
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.HasProperty(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, key.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Sets the listitem's path.
    ///
    /// @param[in] path               string or unicode - path, activated when
    ///                               item is clicked.
    ///
    /// @note You can use the above as keywords for arguments.
    ///
    void SetPath(const std::string& path)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetPath(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, path.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Returns the path / filename of this listitem.
    ///
    /// @return                       Path string
    ///
    std::string GetPath()
    {
      std::string strReturn;
      char* strMsg = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.GetPath(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle);
      if (strMsg != nullptr)
      {
        if (std::strlen(strMsg))
          strReturn = strMsg;
        ::kodi::addon::CAddonBase::m_instance->toKodi.free_string(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, strMsg);
      }
      return strReturn;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Returns the description of this listitem.
    ///
    /// @return                       Description string
    ///
    std::string GetDescription()
    {
      std::string strReturn;
      char* strMsg = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.GetDescription(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle);
      if (strMsg != nullptr)
      {
        if (std::strlen(strMsg))
          strReturn = strMsg;
        ::kodi::addon::CAddonBase::m_instance->toKodi.free_string(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, strMsg);
      }
      return strReturn;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Returns the duration of this listitem
    ///
    /// @return                       Duration of item as integer
    ///
    int GetDuration() const
    {
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.GetDuration(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Sets subtitles for this listitem.
    ///
    /// @param[in] subtitleFiles      string - List of subtitles to add
    ///
    ///
    ///-----------------------------------------------------------------------
    ///
    /// **Example:**
    /// ~~~~~~~~~~~~~{.cpp}
    /// ...
    /// ...
    /// ~~~~~~~~~~~~~
    ///
    void SetSubtitles(const std::vector<std::string>& subtitleFiles)
    {
      unsigned int size = subtitleFiles.size();
      if (size == 0)
        return;

      const char** subtitles = (const char**)malloc(size);
      for (unsigned int i = 0; i < size; ++i)
        subtitles[i] = subtitleFiles[i].c_str();

      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetSubtitles(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, subtitles, size);
      free(subtitles);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Sets the listitem's mimetype if known.
    ///
    /// @param[in] mimetype           string or unicode - mimetype.
    ///
    /// If known prehand, this can (but does not have to) avoid HEAD requests
    /// being sent to HTTP servers to figure out file type.
    ///
    void SetMimeType(const std::string& mimetype)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetMimeType(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, mimetype.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Enable or disable content lookup for item.
    ///
    /// If disabled, HEAD requests to e.g. determine mime type will not be
    /// sent.
    ///
    /// @param[in] enable             bool - True = enable, False = disable
    ///
    void SetContentLookup(bool enable)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetContentLookup(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, enable);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Adds item(s) to the context menu for media lists.
    ///
    /// @param[in] items              list - <b><c>[(label, action,)*]</c></b> A list of tuples consisting of label and action pairs.
    ///                               - `label`          : string or unicode - item's label.
    ///                               - `action`         : string or unicode - any built-in function to perform.
    /// @param[in] replaceItems       [opt] bool - True=only your items will show/False=your items will be added to context menu(Default).
    ///
    /// [List of built-in functions](http://kodi.wiki/view/List_of_Built_In_Functions)
    ///
    /// @note You can use the above as keywords for arguments and skip certain
    ///       optional arguments.\n
    ///       Once you use a keyword, all following arguments require the
    ///       keyword.
    ///
    ///
    ///-----------------------------------------------------------------------
    ///
    /// **Example:**
    /// ~~~~~~~~~~~~~{.cpp}
    /// ...
    /// ...
    /// ~~~~~~~~~~~~~
    ///
    void AddContextMenuItems(const std::vector<std::pair<std::string, std::string> >& items, bool replaceItems = false)
    {
      const char*** entries = nullptr;
      unsigned int size = items.size();
      if (size != 0)
      {
        entries = (const char***)malloc(size*sizeof(const char***));
        for (unsigned int i = 0; i < size; ++i)
        {
          entries[i][0] = items.at(i).first.c_str();
          entries[i][1] = items.at(i).second.c_str();
        }
      }

      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.AddContextMenuItems(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, entries, size, replaceItems);
      if (entries)
        free(entries);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem
    /// @brief Add a stream with details.
    ///
    /// @param[in] type               string - type of stream(video/audio/subtitle).
    /// @param[in] dictionary         dictionary - pairs of { label: value }.
    /// - Video Values:
    ///  | Label       | Type          | Example value     |
    ///  |:-----------:|:-------------:|:------------------|
    ///  | codec       | string        | h264
    ///  | aspect      | float         | 1.78
    ///  | width       | integer       | 1280
    ///  | height      | integer       | 720
    ///  | duration    | integer       | seconds
    ///
    /// - Audio Values:
    ///  | Label       | Type          | Example value     |
    ///  |:-----------:|:-------------:|:------------------|
    ///  | codec       | string        | dts
    ///  | language    | string        | en
    ///  | channels    | integer       | 2
    ///
    /// - Subtitle Values:
    ///  | Label       | Type          | Example value    |
    ///  |:-----------:|:-------------:|:-----------------|
    ///  | language    | string        | en
    ///
    ///
    ///
    ///-----------------------------------------------------------------------
    ///
    /// **Example:**
    /// ~~~~~~~~~~~~~{.cpp}
    /// ...
    /// ...
    /// ~~~~~~~~~~~~~
    ///
    void AddStreamInfo(const std::string& type, const std::vector<std::pair<std::string, std::string> >& dictionary)
    {
      const char*** entries = nullptr;
      unsigned int size = dictionary.size();
      if (size == 0)
        return;

      entries = (const char***)malloc(size*sizeof(const char***));
      for (unsigned int i = 0; i < size; ++i)
      {
        entries[i][0] = dictionary.at(i).first.c_str();
        entries[i][1] = dictionary.at(i).second.c_str();
      }

      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.AddStreamInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type.c_str(), entries, size);
      free(entries);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_MusicInfoTag
    /// @brief Sets the music listitem's infoLabels of type bool.
    ///
    /// @param[in] type               type of \ref ADDON_MusicInfoTag "Music Info Tag"
    /// @param[in] value              boolean value to set
    ///
    void SetMusicInfo_BOOL(ADDON_MusicInfoTag type, bool value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetMusicInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(bool));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_MusicInfoTag
    /// @brief Sets the music listitem's infoLabels of type integer.
    ///
    /// @param[in] type               type of \ref ADDON_MusicInfoTag "Music Info Tag"
    /// @param[in] value              integer value to set
    ///
    void SetMusicInfo_INT(ADDON_MusicInfoTag type, int value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetMusicInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(int));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_MusicInfoTag
    /// @brief Sets the music listitem's infoLabels of type unsigned integer.
    ///
    /// @param[in] type               type of \ref ADDON_MusicInfoTag "Music Info Tag"
    /// @param[in] value              unsigned integer value to set
    ///
    void SetMusicInfo_UINT(ADDON_MusicInfoTag type, unsigned int value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetMusicInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(unsigned int));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_MusicInfoTag
    /// @brief Sets the music listitem's infoLabels of type float.
    ///
    /// @param[in] type               type of \ref ADDON_MusicInfoTag "Music Info Tag"
    /// @param[in] value              float value to set
    ///
    void SetMusicInfo_FLOAT(ADDON_MusicInfoTag type, float value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetMusicInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(float));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_MusicInfoTag
    /// @brief Sets the music listitem's infoLabels with a string.
    ///
    /// @param[in] type               type of \ref ADDON_MusicInfoTag "Music Info Tag"
    /// @param[in] value              string value to set
    ///
    void SetMusicInfo_STRING(ADDON_MusicInfoTag type, std::string value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetMusicInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, (void*)value.c_str(), sizeof(const char*));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_MusicInfoTag
    /// @brief Sets the music listitem's infoLabels with string table.
    ///
    /// @param[in] type               type of \ref ADDON_MusicInfoTag "Music Info Tag"
    /// @param[in] values             string table to set
    ///
    void SetMusicInfo_STRING_LIST(ADDON_MusicInfoTag type, std::vector<std::string> values)
    {
      unsigned int size = values.size();
      if (type == 0 || size == 0)
        return;

      const char** entries = (const char**)malloc(size*sizeof(const char**));
      for (unsigned int i = 0; i < size; ++i)
        entries[i] = values[i].c_str();

      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetMusicInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &entries, size);
      free(entries);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_VideoInfoTag
    /// @brief Sets the video listitem's infoLabels of type bool.
    ///
    /// @param[in] type               type of \ref ADDON_VideoInfoTag "Video Info Tag"
    /// @param[in] value              boolean value to set
    ///
    void SetVideoInfo_BOOL(ADDON_VideoInfoTag type, bool value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetVideoInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(bool));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_VideoInfoTag
    /// @brief Sets the video listitem's infoLabels of type integer.
    ///
    /// @param[in] type               type of \ref ADDON_VideoInfoTag "Video Info Tag"
    /// @param[in] value              integer value to set
    ///
    void SetVideoInfo_INT(ADDON_VideoInfoTag type, int value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetVideoInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(int));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_VideoInfoTag
    /// @brief Sets the video listitem's infoLabels of type unsigned integer.
    ///
    /// @param[in] type               type of \ref ADDON_VideoInfoTag "Video Info Tag"
    /// @param[in] value              unsigned integer value to set
    ///
    void SetVideoInfo_UINT(ADDON_VideoInfoTag type, unsigned int value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetVideoInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(unsigned int));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_VideoInfoTag
    /// @brief Sets the video listitem's infoLabels of type float.
    ///
    /// @param[in] type               type of \ref ADDON_VideoInfoTag "Video Info Tag"
    /// @param[in] value              float value to set
    ///
    void SetVideoInfo_FLOAT(ADDON_VideoInfoTag type, float value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetVideoInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(float));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_VideoInfoTag
    /// @brief Sets the video listitem's infoLabels with a string.
    ///
    /// @param[in] type               type of \ref ADDON_VideoInfoTag "Video Info Tag"
    /// @param[in] value              string value to set
    ///
    void SetVideoInfo_STRING(ADDON_VideoInfoTag type, std::string value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetVideoInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, (void*)value.c_str(), sizeof(const char*));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_VideoInfoTag
    /// @brief Sets the video listitem's infoLabels with string table.
    ///
    /// @param[in] type               type of \ref ADDON_VideoInfoTag "Video Info Tag"
    /// @param[in] values             string table to set
    ///
    void SetVideoInfo_STRING_LIST(ADDON_VideoInfoTag type, std::vector<std::string> values)
    {
      unsigned int size = values.size();
      if (type == 0 || size == 0)
        return;

      const char** entries = (const char**)malloc(size*sizeof(const char**));
      for (unsigned int i = 0; i < size; ++i)
        entries[i] = values[i].c_str();

      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetVideoInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, (void*)entries, size);
      free(entries);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_VideoInfoTag
    /// @brief Sets the video listitem's infoLabels of type bool.
    ///
    /// @param[in] resume             resume of \ref ADDON_VideoInfoTag "Video Info Tag"
    ///
    void SetVideoInfo_Resume(ADDON_VideoInfoTag_Resume &resume)
    {
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetVideoInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, ADDON_VideoInfoTag____resume____________________DATA, &resume, sizeof(ADDON_VideoInfoTag____resume____________________DATA));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_VideoInfoTag
    /// @brief Sets the video infoLabels for cast.
    ///
    /// @param[in] cast               cast of \ref ADDON_VideoInfoTag "Video Info Tag"
    ///
    void SetVideoInfo_Cast(std::vector<ADDON_VideoInfoTag_Cast> &cast)
    {
      unsigned int size = cast.size();
      if (size == 0)
        return;

      ADDON_VideoInfoTag__cast__DATA_STRUCT** entries = (ADDON_VideoInfoTag__cast__DATA_STRUCT**)malloc(size*sizeof(ADDON_VideoInfoTag__cast__DATA_STRUCT**));
      for (unsigned int i = 0; i < size; ++i)
      {
        entries[i] = (ADDON_VideoInfoTag__cast__DATA_STRUCT*)malloc(sizeof(ADDON_VideoInfoTag__cast__DATA_STRUCT*));
        entries[i]->name       = cast[i].name.c_str();
        entries[i]->role       = cast[i].role.c_str();
        entries[i]->order      = cast[i].order;
        entries[i]->thumbnail  = cast[i].thumbnail.c_str();
      }

      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetVideoInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, ADDON_VideoInfoTag____cast______________________DATA_LIST, (void*)entries, size);
      free(entries);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_PictureInfoTag
    /// @brief Sets the video infoLabels with picture info tag
    ///
    /// @param[in] type               type of \ref ADDON_PictureInfoTag "Picture Info Tag"
    /// @param[in] value              boolean value to set
    ///
    void SetPictureInfo_BOOL(ADDON_PictureInfoTag type, bool value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetPictureInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(bool));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_PictureInfoTag
    /// @brief Sets the picture listitem's infoLabels of type integer.
    ///
    /// @param[in] type               type of \ref ADDON_PictureInfoTag "Picture Info Tag"
    /// @param[in] value              integer value to set
    ///
    void SetPictureInfo_INT(ADDON_PictureInfoTag type, int value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetPictureInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(int));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_PictureInfoTag
    /// @brief Sets the picture listitem's infoLabels of type unsigned integer.
    ///
    /// @param[in] type               type of \ref ADDON_PictureInfoTag "Picture Info Tag"
    /// @param[in] value              unsigned integer value to set
    ///
    void SetPictureInfo_UINT(ADDON_PictureInfoTag type, unsigned int value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetPictureInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(unsigned int));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_PictureInfoTag
    /// @brief Sets the picture listitem's infoLabels of type float.
    ///
    /// @param[in] type               type of \ref ADDON_PictureInfoTag "Picture Info Tag"
    /// @param[in] value              float value to set
    ///
    void SetPictureInfo_FLOAT(ADDON_PictureInfoTag type, float value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetPictureInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, &value, sizeof(float));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_PictureInfoTag
    /// @brief Sets the picture listitem's infoLabels with a string.
    ///
    /// @param[in] type               type of \ref ADDON_PictureInfoTag "Picture Info Tag"
    /// @param[in] value              string value to set
    ///
    void SetPictureInfo_STRING(ADDON_PictureInfoTag type, std::string value)
    {
      if (type != 0)
        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetPictureInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, (void*)value.c_str(), sizeof(const char*));
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// \ingroup cpp_kodi_gui_CListItem_PictureInfoTag
    /// @brief Sets the picture listitem's infoLabels with string table.
    ///
    /// @param[in] type               type of \ref ADDON_PictureInfoTag "Picture Info Tag"
    /// @param[in] values             string table to set
    ///
    void SetPictureInfo_STRING_LIST(ADDON_PictureInfoTag type, std::vector<std::string> values)
    {
      unsigned int size = values.size();
      if (type == 0 || size == 0)
        return;

      const char** entries = (const char**)malloc(size*sizeof(const char**));
      for (unsigned int i = 0; i < size; ++i)
        entries[i] = values[i].c_str();

      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_gui->listItem.SetPictureInfo(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_ListItemHandle, type, (void*)entries, size);
      free(entries);
    }
    //--------------------------------------------------------------------------

    const void* GetListItemHandle() const
    {
      return m_ListItemHandle;
    }

  protected:
    void* m_ListItemHandle;

  private:
    friend class CWindow;
  };

} /* namespace gui */
} /* namespace kodi */
} /* extern "C" */
