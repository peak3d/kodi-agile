#pragma once
/*
 *      Copyright (C) 2016 Team KODI
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

#include "addon/AddonBase.h"

extern "C"
{

struct VFSDirEntry;

// Structure about function addresses in kodi to have available on addon
typedef struct sAddonToKodiFuncTable_kodi_vfs
{
  bool (*can_open_directory)(void* kodiInstance, const char* url);
  bool (*create_directory)(void* kodiInstance, const char* path);
  bool (*remove_directory)(void* kodiInstance, const char* path);
  bool (*directory_exists)(void* kodiInstance, const char* path);
  bool (*get_directory)(void* kodiInstance, const char* path, const char* mask, VFSDirEntry** items, unsigned int* num_items);
  void (*free_directory)(void* kodiInstance, VFSDirEntry* items, unsigned int num_items);

  void* (*open_file)(void* kodiInstance, const char* filename, unsigned int flags);
  void* (*open_file_for_write)(void* kodiInstance, const char* filename, bool overwrite);
  ssize_t (*read_file)(void* kodiInstance, void* file, void* ptr, size_t size);
  bool (*read_file_string)(void* kodiInstance, void* file, char *szLine, int iLineLength);
  ssize_t (*write_file)(void* kodiInstance, void* file, const void* ptr, size_t size);
  void (*flush_file)(void* kodiInstance, void* file);
  int64_t (*seek_file)(void* kodiInstance, void* file, int64_t position, int whence);
  int (*truncate_file)(void* kodiInstance, void* file, int64_t size);
  int64_t (*get_file_position)(void* kodiInstance, void* file);
  int64_t (*get_file_length)(void* kodiInstance, void* file);
  double (*get_file_download_speed)(void* kodiInstance, void* file);
  void (*close_file)(void* kodiInstance, void* file);
  int (*get_file_chunk_size)(void* kodiInstance, void* file);
  bool (*file_exists)(void* kodiInstance, const char *filename, bool usecache);
  int (*stat_file)(void* kodiInstance, const char *filename, struct __stat64* buffer);
  bool (*delete_file)(void* kodiInstance, const char *filename);
  char* (*get_file_md5)(void* kodiInstance, const char* filename);
  char* (*get_cache_thumb_name)(void* kodiInstance, const char* filename);
  char* (*make_legal_filename)(void* kodiInstance, const char* filename);
  char* (*make_legal_path)(void* kodiInstance, const char* path);
  void* (*curl_create)(void* kodiInstance, const char* url);
  bool (*curl_add_option)(void* kodiInstance, void* file, int type, const char* name, const char* value);
  bool (*curl_open)(void* kodiInstance, void* file, unsigned int flags);
} sAddonToKodiFuncTable_kodi_vfs;

//==============================================================================
/// \defgroup cpp_kodi_vfs_CFile_Defs Definitions, structures and enumerators
/// \ingroup cpp_kodi_vfs_CFile
/// @brief **Virtual file Server definition values**
//------------------------------------------------------------------------------
  
//==============================================================================
///
/// @brief Transfer structure for data from Kodi to addon about directory entries.
///
struct VFSDirEntry
{
  /// item label
  char* label;
  /// item path
  char* path;
  /// Item is a folder
  bool folder;
  /// Size of file represented by item
  uint64_t size;
};
//------------------------------------------------------------------------------

//==============================================================================
///
/// @ingroup cpp_kodi_vfs_CFile_Defs
/// Flags to define way how file becomes opened with kodi::vfs::CFile::OpenFile()
///
/// The values can be used together, e.g. <b>`file.OpenFile("myfile", READ_TRUNCATED | READ_CHUNKED);`</b>
///
typedef enum OpenFileFlags
{
  /// indicate that caller can handle truncated reads, where function returns
  /// before entire buffer has been filled
  READ_TRUNCATED = 0x01,

  /// indicate that that caller support read in the minimum defined chunk size,
  /// this disables internal cache then
  READ_CHUNKED = 0x02,

  /// use cache to access this file
  READ_CACHED = 0x04,

  /// open without caching. regardless to file type
  READ_NO_CACHE = 0x08,

  /// calcuate bitrate for file while reading
  READ_BITRATE = 0x10,

  /// indicate to the caller we will seek between multiple streams in the file
  /// frequently
  READ_MULTI_STREAM = 0x20,

  /// indicate to the caller file is audio and/or video (and e.g. may grow)
  READ_AUDIO_VIDEO = 0x40,

  /// indicate that caller will do write operations before reading
  READ_AFTER_WRITE = 0x80
} OpenFileFlags;
//------------------------------------------------------------------------------

//==============================================================================
/// \ingroup cpp_kodi_vfs_CFile_Defs
/// @brief Used CURL message types
///
typedef enum CURLOptiontype
{
  /// Set a general option
  ADDON_CURL_OPTION_OPTION,
  /// Set a protocol option
  ADDON_CURL_OPTION_PROTOCOL,
  /// Set User and password
  ADDON_CURL_OPTION_CREDENTIALS,
  /// Add a Header
  ADDON_CURL_OPTION_HEADER
} CURLOptiontype;
//------------------------------------------------------------------------------

namespace kodi
{
namespace vfs
{

  std::string GetDirectoryName(const std::string& path);

  //============================================================================
  ///
  /// \defgroup cpp_kodi_vfs_CDirEntry class CDirEntry
  /// \ingroup cpp_kodi_vfs
  ///
  /// @brief **Virtual file server directory entry**
  ///
  /// This class is used as an entry for files and folders in
  /// kodi::vfs::GetDirectory().
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  ///
  /// ...
  ///
  /// std::vector<kodi::vfs::CDirEntry> items;
  /// kodi::vfs::GetDirectory("special://temp", "", items);
  ///
  /// fprintf(stderr, "Directory have %lu entries\n", items.size());
  /// for (unsigned long i = 0; i < items.size(); i++)
  /// {
  ///   fprintf(stderr, " - %04lu -- Folder: %s -- Name: %s -- Path: %s\n",
  ///             i+1,
  ///             items[i].IsFolder() ? "yes" : "no ",
  ///             items[i].Label().c_str(),
  ///             items[i].Path().c_str());
  /// }
  /// ~~~~~~~~~~~~~
  ///
  /// It has the header \ref VFS.h "#include <kodi/VFS.h>" be included
  /// to enjoy it.
  ///
  //@{
  class CVFSDirEntry
  {
  public:
    //============================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CDirEntry
    /// @brief Constructor for VFS directory entry
    ///
    /// @param[in] label   [opt] Name to use for entry
    /// @param[in] path    [opt] Used path of the entry
    /// @param[in] folder  [opt] If set entry used as folder
    /// @param[in] size    [opt] If used as file, his size defined there
    ///
    CVFSDirEntry(
        const std::string&  label  = "",
        const std::string&  path   = "",
        bool                folder = false,
        int64_t             size   = -1)
      : m_label(label),
        m_path(path),
        m_folder(folder),
        m_size(size) { }
    //----------------------------------------------------------------------------

    //============================================================================
    // @note Not for addon development itself needed, thats why below is
    // disabled for doxygen!
    //
    // @ingroup cpp_kodi_vfs_CDirEntry 
    // @brief Constructor to create own copy
    //
    // @param[in] dirEntry pointer to own class type
    //
    CVFSDirEntry(const VFSDirEntry& dirEntry)
      : m_label(dirEntry.label ? dirEntry.label : ""),
        m_path(dirEntry.path ? dirEntry.path  : ""),
        m_folder(dirEntry.folder),
        m_size(dirEntry.size) { }
    //----------------------------------------------------------------------------

    //============================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CDirEntry
    /// @brief Get the directory entry name
    ///
    /// @return Name of the entry
    ///
    const std::string& Label(void) const { return m_label; }
    //----------------------------------------------------------------------------

    //============================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CDirEntry
    /// @brief Get the path of the entry
    ///
    /// @return File system path of the entry
    ///
    const std::string& Path(void) const { return m_path; }
    //----------------------------------------------------------------------------

    //============================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CDirEntry
    /// @brief Used to check entry is folder
    ///
    /// @return true if entry is a folder
    ///
    bool IsFolder(void) const { return m_folder; }
    //----------------------------------------------------------------------------

    //============================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CDirEntry
    /// @brief If file, the size of the file
    ///
    /// @return Defined file size
    ///
    int64_t Size(void) const { return m_size; }
    //----------------------------------------------------------------------------

    //============================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CDirEntry
    /// @brief Set the label name
    ///
    /// @param[in] label name of entry
    ///
    void SetLabel(const std::string& label) { m_label = label; }
    //----------------------------------------------------------------------------

    //============================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CDirEntry
    /// @brief Set the path of the entry
    ///
    /// @param[in] path path of entry
    ///
    void SetPath(const std::string& path) { m_path = path; }
    //----------------------------------------------------------------------------

    //============================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CDirEntry
    /// @brief Set the entry defined as folder
    ///
    /// @param[in] folder If true becomes entry defined as folder
    ///
    void SetFolder(bool folder) { m_folder = folder; }
    //----------------------------------------------------------------------------

    //============================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CDirEntry
    /// @brief Set a file size for a new entry
    ///
    /// @param[in] size Size to set for dir entry
    ///
    void SetSize(int64_t size) { m_size = size; }
    //----------------------------------------------------------------------------

  private:
    std::string m_label;
    std::string m_path;
    bool m_folder;
    int64_t m_size;
  };
  //@}

  //============================================================================
  ///
  /// \defgroup cpp_kodi_vfs_CFile class CFile
  /// \ingroup cpp_kodi_vfs
  ///
  /// @brief **Virtual file server control**
  ///
  /// CFile is the class used for handling Files in Kodi. This class can be used
  /// for creating, reading, writing and modifying files. It directly provides unbuffered, binary disk input/output services
  ///
  /// It has the header \ref VFS.h "#include <kodi/VFS.h>" be included
  /// to enjoy it.
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  ///
  /// ...
  ///
  /// /* Create the needed file handle class */
  /// kodi::vfs::CFile myFile();
  ///
  /// /* In this example we use the user path for the add-on */
  /// std::string file = kodi::GetUserPath() + "/myFile.txt";
  ///
  /// /* Now create and open the file or overwrite if present */
  /// myFile.OpenFileForWrite(file, true);
  ///
  /// const char* str = "I love Kodi!";
  ///
  /// /* write string */
  /// myFile.Write(str, sizeof(str));
  ///
  /// /* On this way the Close() is not needed to call, becomes done from destructor */
  ///
  /// ~~~~~~~~~~~~~
  ///
  //@{
  class CFile
  {
  public:
    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Construct a new, unopened file
    ///
    CFile() : m_file(nullptr) { }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Close() is called from the destructor, so explicitly closing the
    /// file isn't required
    ///
    virtual ~CFile() { Close(); }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Open the file with filename via Kodi's \ref cpp_kodi_vfs_CFile
    /// "CFile". Needs to be closed by calling Close() when done.
    ///
    /// @param[in] filename     The filename to open.
    /// @param[in] flags        [opt] The flags to pass, see \ref OpenFileFlags
    /// @return                 True on success or false on failure
    ///
    bool OpenFile(const std::string& filename, unsigned int flags = 0)
    {
      Close();
      m_file = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->open_file(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, filename.c_str(), flags);
      return m_file != nullptr;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Open the file with filename via Kodi's \ref cpp_kodi_vfs_CFile
    /// "CFile" in write mode. Needs to be closed by calling Close() when 
    /// done.
    ///
    /// @note Related folders becomes created if not present.
    ///
    /// @param[in] filename     The filename to open.
    /// @param[in] overwrite    True to overwrite, false otherwise.
    /// @return                 True on success or false on failure
    ///
    bool OpenFileForWrite(const std::string& filename, bool overwrite = false)
    {
      Close();

      // Try to open the file. If it fails, check if we need to create the directory first
      // This way we avoid checking if the directory exists every time
      m_file = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->open_file_for_write(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, filename.c_str(), overwrite);
      if (!m_file)
      {
        std::string cacheDirectory = kodi::vfs::GetDirectoryName(filename);
        if (::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->directory_exists(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, cacheDirectory.c_str()) ||
            ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->create_directory(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, cacheDirectory.c_str()))
          m_file = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->open_file_for_write(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, filename.c_str(), overwrite);
      }
      return m_file != nullptr;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Create a Curl representation
    ///
    /// @param[in] url          the URL of the Type.
    /// @return                 True on success or false on failure
    ///
    bool CURLCreate(std::string& url)
    {
      m_file = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->curl_create(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, url.c_str());
      return m_file != nullptr;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Add options to the curl file created with CURLCreate
    ///
    /// @param[in] type         option type to set, see \ref CURLOptiontype
    /// @param[in] name         name of the option
    /// @param[in] value        value of the option
    /// @return                 True on success or false on failure
    ///
    bool CURLAddOption(CURLOptiontype type, std::string& name, std::string& value)
    {
      if (!m_file)
      {
        kodi::Log(LOG_ERROR, "kodi::vfs::CURLCreate(...) needed to call before!");
        return false;
      }
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->curl_add_option(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file, type, name.c_str(), value.c_str());
    }
    //--------------------------------------------------------------------------
    
    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Open the curl file created with CURLCreate
    ///
    /// @param[in] flags        [opt] The flags to pass, see \ref OpenFileFlags
    /// @return                 True on success or false on failure
    ///
    bool CURLOpen(unsigned int flags = 0)
    {
      if (!m_file)
      {
        kodi::Log(LOG_ERROR, "kodi::vfs::CURLCreate(...) needed to call before!");
        return false;
      }
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->curl_open(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file, flags);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Read from an open file.
    ///
    /// @param[in] ptr          The buffer to store the data in.
    /// @param[in] size         The size of the buffer.
    /// @return                 number of successfully read bytes if any bytes
    ///                         were read and stored in buffer, zero if no bytes
    ///                         are available to read (end of file was reached)
    ///                         or undetectable error occur, -1 in case of any
    ///                         explicit error
    ///
    ssize_t Read(void* ptr, size_t size)
    {
      if (!m_file)
        return 0;
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->read_file(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file, ptr, size);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Read a string from an open file.
    ///
    /// @param[in] line         The buffer to store the data in.
    /// @return                 True when a line was read, false otherwise.
    ///
    bool ReadLine(std::string &line)
    {
      line.clear();
      if (!m_file)
        return false;
      // TODO: Read 1024 chars into buffer. If file position advanced that many
      // chars, we didn't hit a newline. Otherwise, if file position is 1 or 2
      // past the number of bytes read, we read (and skipped) a newline sequence.
      char buffer[1025];
      if (::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->read_file_string(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file, buffer, sizeof(buffer)))
      {
        line = buffer;
        return !line.empty();
      }
      return false;
    }
    //--------------------------------------------------------------------------

    //==========================================================================  
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Write to a file opened in write mode.
    ///
    /// @param[in] ptr          Pointer to the data to write, converted to a
    ///                         const void*.
    /// @param[in] size         Size of the data to write.
    /// @return                 number of successfully written bytes if any
    ///                         bytes were written, zero if no bytes were
    ///                         written and no detectable error occur,-1 in case
    ///                         of any explicit error
    ///
    ssize_t Write(const void* ptr, size_t size)
    {
      if (!m_file)
        return 0;
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->write_file(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file, ptr, size);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Flush buffered data.
    ///
    /// If the given stream was open for writing (or if it was open for updating
    /// and the last i/o operation was an output operation) any unwritten data
    /// in its output buffer is written to the file.
    ///
    /// The stream remains open after this call.
    ///
    /// When a file is closed, either because of a call to close or because the
    /// class is destructed, all the buffers associated with it are
    /// automatically flushed.
    ///
    void Flush()
    {
      if (!m_file)
        return;
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->flush_file(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// \brief Set the file's current position.
    ///
    /// The whence argument is optional and defaults to SEEK_SET (0)
    ///
    /// \param[in] position             the position that you want to seek to
    /// \param[in] whence               [optional] offset relative to
    ///                                 You can set the value of whence to one.
    ///                                 of three things:
    /// |   Value  | int | Description                                         |
    /// |:--------:|:---:|:----------------------------------------------------|
    /// | SEEK_SET |  0  | position is relative to the beginning of the file. This is probably what you had in mind anyway, and is the most commonly used value for whence.
    /// | SEEK_CUR |  1  | position is relative to the current file pointer position. So, in effect, you can say, "Move to my current position plus 30 bytes," or, "move to my current position minus 20 bytes."
    /// | SEEK_END |  2  | position is relative to the end of the file. Just like SEEK_SET except from the other end of the file. Be sure to use negative values for offset if you want to back up from the end of the file, instead of going past the end into oblivion.
    ///
    /// \return                         Returns the resulting offset location as
    ///                                 measured in bytes from the beginning of
    ///                                 the file. On error, the value -1 is
    ///                                 returned.
    ///
    int64_t Seek(int64_t position, int whence = SEEK_SET)
    {
      if (!m_file)
        return 0;
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->seek_file(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file, position, whence);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Truncate a file to the requested size.
    ///
    /// @param[in] size                 The new max size.
    /// @return                         New size?
    ///
    int Truncate(int64_t size)
    {
      if (!m_file)
        return -1;
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->truncate_file(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file, size);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief The current offset in an open file.
    ///
    /// @return                 The requested offset.
    ///
    int64_t GetPosition()
    {
      if (!m_file)
        return -1;
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->get_file_position(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Get the file size of an open file.
    ///
    /// @return                 The requested size.
    ///
    int64_t GetLength()
    {
      if (!m_file)
        return 0;
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->get_file_length(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file);
    }

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Close an open file.
    ///
    void Close()
    {
      if (!m_file)
        return;
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->close_file(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file);
      m_file = nullptr;
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @ingroup cpp_kodi_vfs_CFile
    /// @brief Get the chunk size for an open file.
    ///
    /// @return                 The requested size.
    ///
    int GetChunkSize()
    {
      if (!m_file)
        return 0;
      return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->get_file_chunk_size(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, m_file);
    }
    //--------------------------------------------------------------------------

  private:
    void* m_file;
  };
  //@}

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Make a directory
  ///
  /// The kodi::vfs::CreateDirectory() function shall create a
  /// new directory with name path.
  ///
  /// The newly created directory shall be an empty directory.
  ///
  /// @param[in] path           Path to the directory.
  /// @return  Upon successful completion, CreateDirectory() shall return true.
  ///          Otherwise false shall be returned, no directory shall be created.
  ///
  ///
  /// -------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// ...
  /// std::string directory = "C:\\my_dir";
  /// bool ret = kodi::vfs::CreateDirectory(directory);
  /// fprintf(stderr, "Directory '%s' successfull created: %s\n", directory.c_str(), ret ? "yes" : "no");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline bool CreateDirectory(const std::string& path)
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->create_directory(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, path.c_str());
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Verifying the Existence of a Directory
  ///
  /// The kodi::vfs::DirectoryExists() method determines whether
  /// a specified folder exists.
  ///
  /// @param[in] path Path to the directory.
  /// @return True when it exists, false otherwise.
  ///
  ///
  /// -------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// ...
  /// std::string directory = "C:\\my_dir";
  /// bool ret = kodi::vfs::DirectoryExists(directory);
  /// fprintf(stderr, "Directory '%s' present: %s\n", directory.c_str(), ret ? "yes" : "no");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline bool DirectoryExists(const std::string& path)
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->directory_exists(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, path.c_str());
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Removes a directory.
  ///
  /// The kodi::vfs::RemoveDirectory() function shall remove a
  /// directory whose name is given by path.
  ///
  /// @param[in] path Path to the directory.
  /// @return  Upon successful completion, the function RemoveDirectory() shall
  ///          return true. Otherwise, false shall be returned, and errno set
  ///          to indicate the error. If false is returned, the named directory
  ///          shall not be changed.
  ///
  ///
  /// -------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// ...
  /// bool ret = kodi::vfs::RemoveDirectory("C:\\my_dir");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline bool RemoveDirectory(const std::string& path)
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->remove_directory(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, path.c_str());
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Lists a directory.
  ///
  /// Return the list of files and directories which have been found in the
  /// specified directory and which respect the given constraint.
  ///
  /// It can handle the normal OS dependent paths and also the special virtual
  /// filesystem from Kodi what starts with \b special://.
  ///
  /// @param[in]  path  The path in which the files and directories are located.
  /// @param[in]  mask  Mask to filter out requested files, e.g. "*.avi|*.mpg" to
  ///                   files with this ending.
  /// @param[out] items The returned list directory entries.
  /// @return           True if listing was successful, false otherwise.
  ///
  ///
  /// -------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  ///
  /// std::vector<KodiAPI::AddOn::CVFSDirEntry> items;
  /// kodi::vfs::GetDirectory("special://temp", "", items);
  ///
  /// fprintf(stderr, "Directory have %lu entries\n", items.size());
  /// for (unsigned long i = 0; i < items.size(); i++)
  /// {
  ///   fprintf(stderr, " - %04lu -- Folder: %s -- Name: %s -- Path: %s\n",
  ///             i+1,
  ///             items[i].IsFolder() ? "yes" : "no ",
  ///             items[i].Label().c_str(),
  ///             items[i].Path().c_str());
  /// }
  /// ~~~~~~~~~~~~~
  ///
  inline bool GetDirectory(const std::string& path, const std::string& mask, std::vector<CVFSDirEntry>& items)
  {
    VFSDirEntry* dir_list = nullptr;
    unsigned int num_items = 0;
    if (::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->get_directory(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, path.c_str(), mask.c_str(), &dir_list, &num_items))
    {
      if (dir_list)
      {
        for (unsigned int i = 0; i < num_items; ++i)
          items.push_back(CVFSDirEntry(dir_list[i]));

        ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->free_directory(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, dir_list, num_items);
      }

      return true;
    }
    return false;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Check if a file exists.
  ///
  /// @param[in] filename       The filename to check.
  /// @param[in] usecache       Check in file cache.
  /// @return                   true if the file exists false otherwise.
  ///
  ///
  /// -------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// ...
  /// bool exists = kodi::vfs::FileExists("special://temp/kodi.log");
  /// fprintf(stderr, "Log file should be always present, is it present? %s\n", exists ? "yes" : "no");
  /// ~~~~~~~~~~~~~
  ///
  inline bool FileExists(const std::string& filename, bool usecache = false)
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->file_exists(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, filename.c_str(), usecache);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Get file status.
  ///
  /// These function return information about a file. Execute (search)
  /// permission is required on all of the directories in path that
  /// lead to the file.
  ///
  /// The call return a stat structure, which contains the following fields:
  ///
  /// ~~~~~~~~~~~~~{.cpp}
  /// struct __stat64
  /// {
  ///   dev_t     st_dev;     // ID of device containing file
  ///   ino_t     st_ino;     // inode number
  ///   mode_t    st_mode;    // protection
  ///   nlink_t   st_nlink;   // number of hard links
  ///   uid_t     st_uid;     // user ID of owner
  ///   gid_t     st_gid;     // group ID of owner
  ///   dev_t     st_rdev;    // device ID (if special file)
  ///   off_t     st_size;    // total size, in bytes
  ///   blksize_t st_blksize; // blocksize for file system I/O
  ///   blkcnt_t  st_blocks;  // number of 512B blocks allocated
  ///   time_t    st_atime;   // time of last access
  ///   time_t    st_mtime;   // time of last modification
  ///   time_t    st_ctime;   // time of last status change
  /// };
  /// ~~~~~~~~~~~~~
  ///
  /// The st_dev field describes the device on which this file resides.
  /// The st_rdev field describes the device that this file (inode) represents.
  ///
  /// The st_size field gives the size of the file (if it is a regular file or
  /// a symbolic link) in bytes. The size of a symbolic link (only on Linux
  /// and Mac OS X) is the length of the pathname it contains, without a
  /// terminating null byte.
  ///
  /// The st_blocks field indicates the number of blocks allocated to the file,
  /// 512-byte units. (This may be smaller than st_size/512 when the file has
  /// holes.).
  ///
  /// The st_blksize field gives the "preferred" blocksize for efficient file
  /// system I/O. (Writing to a file in smaller chunks may cause an inefficient
  /// read-modify-rewrite.)
  ///
  /// @warning Not all of the OS file systems implement all of the time fields.
  ///
  /// @param[in] filename The filename to read the status from.
  /// @param[in] buffer The file status is written into this buffer.
  /// @return On success, zero is returned. On error, -1 is returned, and errno
  /// is set appropriately.
  ///
  ///
  /// -------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// ...
  /// struct __stat64 statFile;
  /// int ret = kodi::vfs::StatFile("special://temp/kodi.log", &statFile);
  /// fprintf(stderr, "st_dev (ID of device containing file)       = %lu\n"
  ///                 "st_ino (inode number)                       = %lu\n"
  ///                 "st_mode (protection)                        = %u\n"
  ///                 "st_nlink (number of hard links)             = %lu\n"
  ///                 "st_uid (user ID of owner)                   = %u\n"
  ///                 "st_gid (group ID of owner)                  = %u\n"
  ///                 "st_rdev (device ID (if special file))       = %lu\n"
  ///                 "st_size (total size, in bytes)              = %lu\n"
  ///                 "st_blksize (blocksize for file system I/O)  = %lu\n"
  ///                 "st_blocks (number of 512B blocks allocated) = %lu\n"
  ///                 "st_atime (time of last access)              = %lu\n"
  ///                 "st_mtime (time of last modification)        = %lu\n"
  ///                 "st_ctime (time of last status change)       = %lu\n"
  ///                 "Return value                                = %i\n",
  ///                      statFile.st_dev,
  ///                      statFile.st_ino,
  ///                      statFile.st_mode,
  ///                      statFile.st_nlink,
  ///                      statFile.st_uid,
  ///                      statFile.st_gid,
  ///                      statFile.st_rdev,
  ///                      statFile.st_size,
  ///                      statFile.st_blksize,
  ///                      statFile.st_blocks,
  ///                      statFile.st_atime,
  ///                      statFile.st_mtime,
  ///                      statFile.st_ctime,
  ///                      ret);
  /// ~~~~~~~~~~~~~
  ///
  inline int StatFile(const std::string& filename, struct __stat64* buffer)
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->stat_file(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, filename.c_str(), buffer);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Deletes a file.
  ///
  /// @param[in] filename The filename to delete.
  /// @return The file was successfully deleted.
  ///
  ///
  /// -------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// #include <kodi/gui/DialogFileBrowser.h>
  /// #include <kodi/gui/DialogOK.h>
  /// ...
  /// std::string filename;
  /// if (kodi::gui::CAddonGUIDialogFileBrowser::ShowAndGetFile("local", "",
  ///                                                "Test File selection and delete of them!",
  ///                                                filename))
  /// {
  ///   bool successed = kodi::vfs::DeleteFile(filename);
  ///   if (!successed)
  ///     kodi::gui::CAddonGUIDialogOK::ShowAndGetInput("Error", "Delete of File", filename, "failed!");
  ///   else
  ///     kodi::gui::CAddonGUIDialogOK::ShowAndGetInput("Information", "Delete of File", filename, "successfull done.");
  /// }
  /// ~~~~~~~~~~~~~
  ///
  inline bool DeleteFile(const std::string& filename)
  {
    return ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->delete_file(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, filename.c_str());
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Retrieve MD5sum of a file
  ///
  /// @param[in] path path to the file to MD5sum
  /// @return md5 sum of the file
  ///
  ///
  /// -------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// #include <kodi/gui/DialogFileBrowser.h>
  /// ...
  /// std::string md5;
  /// std::string filename;
  /// if (kodi::gui::CAddonGUIDialogFileBrowser::ShowAndGetFile("local", "*.avi|*.mpg|*.mp4",
  ///                                                "Test File selection to get MD5",
  ///                                                filename))
  /// {
  ///   md5 = kodi::vfs::GetFileMD5(filename);
  ///   fprintf(stderr, "MD5 of file '%s' is %s\n", md5.c_str(), filename.c_str());
  /// }
  /// ~~~~~~~~~~~~~
  ///
  inline std::string GetFileMD5(const std::string& path)
  {
    std::string strReturn;
    char* strMd5 = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->get_file_md5(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, path.c_str());
    if (strMd5 != nullptr)
    {
      if (std::strlen(strMd5))
        strReturn = strMd5;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, strMd5);
    }
    return strReturn;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Returns a thumb cache filename
  ///
  /// @param[in] filename path to file
  /// @return cache filename
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// #include <kodi/gui/DialogFileBrowser.h>
  /// ...
  /// std::string thumb;
  /// std::string filename;
  /// if (kodi::gui::CAddonGUIDialogFileBrowser::ShowAndGetFile("local", "*.avi|*.mpg|*.mp4",
  ///                                                "Test File selection to get Thumnail",
  ///                                                filename))
  /// {
  ///   thumb = kodi::vfs::GetCacheThumbName(filename);
  ///   fprintf(stderr, "Thumb name of file '%s' is %s\n", thumb.c_str(), filename.c_str());
  /// }
  /// ~~~~~~~~~~~~~
  ///
  inline std::string GetCacheThumbName(const std::string& filename)
  {
    std::string strReturn;
    char* strThumbName = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->get_cache_thumb_name(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, filename.c_str());
    if (strThumbName != nullptr)
    {
      if (std::strlen(strThumbName))
        strReturn = strThumbName;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, strThumbName);
    }
    return strReturn;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Make filename valid
  ///
  /// Function to replace not valid characters with '_'. It can be also
  /// compared with original before in a own loop until it is equal
  /// (no invalid characters).
  ///
  /// @param[in] filename Filename to check and fix
  /// @return            The legal filename
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// ...
  /// std::string fileName = "///\\jk???lj????.mpg";
  /// std::string legalName = kodi::vfs::MakeLegalFileName(fileName);
  /// fprintf(stderr, "Legal name of '%s' is '%s'\n", fileName.c_str(), legalName.c_str());
  ///
  /// /* Returns as legal: 'jk___lj____.mpg' */
  /// ~~~~~~~~~~~~~
  ///
  inline std::string MakeLegalFileName(const std::string& filename)
  {
    std::string strReturn;
    char* strLegalFileName = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->make_legal_filename(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, filename.c_str());
    if (strLegalFileName != nullptr)
    {
      if (std::strlen(strLegalFileName))
        strReturn = strLegalFileName;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, strLegalFileName);
    }
    return strReturn;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Make directory name valid
  ///
  /// Function to replace not valid characters with '_'. It can be also
  /// compared with original before in a own loop until it is equal
  /// (no invalid characters).
  ///
  /// @param[in] path Directory name to check and fix
  /// @return        The legal directory name
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// ...
  /// std::string path = "///\\jk???lj????\\hgjkg";
  /// std::string legalPath = kodi::vfs::MakeLegalPath(path);
  /// fprintf(stderr, "Legal name of '%s' is '%s'\n", path.c_str(), legalPath.c_str());
  ///
  /// /* Returns as legal: '/jk___lj____/hgjkg' */
  /// ~~~~~~~~~~~~~
  ///
  inline std::string MakeLegalPath(const std::string& path)
  {
    std::string strReturn;
    char* strLegalPath = ::kodi::addon::CAddonBase::m_instance->toKodi.kodi_vfs->make_legal_path(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, path.c_str());
    if (strLegalPath != nullptr)
    {
      if (std::strlen(strLegalPath))
        strReturn = strLegalPath;
      ::kodi::addon::CAddonBase::m_instance->toKodi.free_string(::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance, strLegalPath);
    }
    return strReturn;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Return a size aligned to the chunk size at least as large as the chunk size.
  ///
  /// @param[in] chunk The chunk size
  /// @param[in] minimum The minimum size (or maybe the minimum number of chunks?)
  /// @return The aligned size
  ///
  inline unsigned int GetChunkSize(unsigned int chunk, unsigned int minimum)
  {
    if (chunk)
      return chunk * ((minimum + chunk - 1) / chunk);
    else
      return minimum;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Return the file name from given complate path string
  ///
  /// @param[in] path The complete path include file and directory
  /// @return Filename from path
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// ...
  /// std::string fileName = kodi::vfs::GetFileName("special://temp/kodi.log");
  /// fprintf(stderr, "File name is '%s'\n", fileName.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline std::string GetFileName(const std::string& path)
  {
    /* find the last slash */
    const size_t slash = path.find_last_of("/\\");
    return path.substr(slash+1);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  ///
  /// @ingroup cpp_kodi_vfs
  /// @brief Return the directory name from given complate path string
  ///
  /// @param[in] path The complete path include file and directory
  /// @return Directory name from path
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/VFS.h>
  /// ...
  /// std::string dirName = kodi::vfs::GetDirectoryName("special://temp/kodi.log");
  /// fprintf(stderr, "Directory name is '%s'\n", dirName.c_str());
  /// ~~~~~~~~~~~~~
  ///
  inline std::string GetDirectoryName(const std::string& path)
  {
    // Will from a full filename return the directory the file resides in.
    // Keeps the final slash at end and possible |option=foo options.

    size_t iPosSlash = path.find_last_of("/\\");
    if (iPosSlash == std::string::npos)
      return ""; // No slash, so no path (ignore any options)

    size_t iPosBar = path.rfind('|');
    if (iPosBar == std::string::npos)
      return path.substr(0, iPosSlash + 1); // Only path

    return path.substr(0, iPosSlash + 1) + path.substr(iPosBar); // Path + options
  }
  //----------------------------------------------------------------------------

} /* namespace vfs */
} /* namespace kodi */
} /* extern "C" */
