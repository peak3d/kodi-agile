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

#include "VFS.h"
#include "addons/kodi-addon-dev-kit/include/kodi/VFS.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "filesystem/File.h"
#include "filesystem/Directory.h"
#include "utils/Crc32.h"

using namespace kodi; // addon-dev-kit namespace
using namespace XFILE;

namespace ADDON
{

void Interface_VFS::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_vfs = (sAddonToKodiFuncTable_kodi_vfs*)malloc(sizeof(sAddonToKodiFuncTable_kodi_vfs));
  funcTable->toKodi.kodi_vfs->can_open_directory = can_open_directory;
  funcTable->toKodi.kodi_vfs->create_directory = create_directory;
  funcTable->toKodi.kodi_vfs->remove_directory = remove_directory;
  funcTable->toKodi.kodi_vfs->directory_exists = directory_exists;
  funcTable->toKodi.kodi_vfs->get_directory = get_directory;
  funcTable->toKodi.kodi_vfs->free_directory = free_directory;

  funcTable->toKodi.kodi_vfs->open_file = open_file;
  funcTable->toKodi.kodi_vfs->open_file_for_write = open_file_for_write;
  funcTable->toKodi.kodi_vfs->read_file = read_file;
  funcTable->toKodi.kodi_vfs->read_file_string = read_file_string;
  funcTable->toKodi.kodi_vfs->write_file = write_file;
  funcTable->toKodi.kodi_vfs->flush_file = flush_file;
  funcTable->toKodi.kodi_vfs->seek_file = seek_file;
  funcTable->toKodi.kodi_vfs->truncate_file = truncate_file;
  funcTable->toKodi.kodi_vfs->get_file_position = get_file_position;
  funcTable->toKodi.kodi_vfs->get_file_length = get_file_length;
  funcTable->toKodi.kodi_vfs->get_file_download_speed = get_file_download_speed;
  funcTable->toKodi.kodi_vfs->close_file = close_file;
  funcTable->toKodi.kodi_vfs->get_file_chunk_size = get_file_chunk_size;
  funcTable->toKodi.kodi_vfs->file_exists = file_exists;
  funcTable->toKodi.kodi_vfs->stat_file = stat_file;
  funcTable->toKodi.kodi_vfs->delete_file = delete_file;
  funcTable->toKodi.kodi_vfs->get_file_md5 = get_file_md5;
  funcTable->toKodi.kodi_vfs->get_cache_thumb_name = get_cache_thumb_name;
  funcTable->toKodi.kodi_vfs->make_legal_filename = make_legal_filename;
  funcTable->toKodi.kodi_vfs->make_legal_path = make_legal_path;
  funcTable->toKodi.kodi_vfs->curl_create = curl_create;
  funcTable->toKodi.kodi_vfs->curl_add_option = curl_add_option;
  funcTable->toKodi.kodi_vfs->curl_open = curl_open;
};

void Interface_VFS::DeInit(sFuncTable_Addon* funcTable)
{
  if (funcTable->toKodi.kodi_vfs)
  {
    free(funcTable->toKodi.kodi_vfs);
    funcTable->toKodi.kodi_vfs = nullptr;
  }
}

bool Interface_VFS::can_open_directory(void* kodiInstance, const char* url)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || url == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', url='%p)", __FUNCTION__, addon, url);
    return false;
  }

  try
  {
    CFileItemList items;
    return CDirectory::GetDirectory(url, items);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_VFS::create_directory(void* kodiInstance, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', path='%p)", __FUNCTION__, addon, path);
    return false;
  }

  try
  {
    return CDirectory::Create(path);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_VFS::directory_exists(void* kodiInstance, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', path='%p)", __FUNCTION__, addon, path);
    return false;
  }

  try
  {
    return CDirectory::Exists(path);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_VFS::remove_directory(void* kodiInstance, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', path='%p)", __FUNCTION__, addon, path);
    return false;
  }

  try
  {
    // Empty directory
    CFileItemList fileItems;
    CDirectory::GetDirectory(path, fileItems);
    for (int i = 0; i < fileItems.Size(); ++i)
      CFile::Delete(fileItems.Get(i)->GetPath());

    return CDirectory::Remove(path);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_VFS::get_directory(void* kodiInstance, const char* path, const char* mask, VFSDirEntry** items, unsigned int* num_items)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', path='%p)", __FUNCTION__, addon, path);
    return false;
  }

  try
  {
    CFileItemList fileItems;
    if (!CDirectory::GetDirectory(path, fileItems, mask, DIR_FLAG_NO_FILE_DIRS))
      return false;

    if (fileItems.Size() > 0)
    {
      *num_items = static_cast<unsigned int>(fileItems.Size());
      *items = (VFSDirEntry*)malloc(fileItems.Size()*sizeof(VFSDirEntry));

      CFileItemListToVFSDirEntries(*items, *num_items, fileItems);
    }
    else
    {
      *num_items = 0;
      *items = nullptr;
    }

    return true;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

void Interface_VFS::free_directory(void* kodiInstance, VFSDirEntry* items, unsigned int num_items)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || items == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', items='%p)", __FUNCTION__, addon, items);
    return;
  }

  try
  {
    for (unsigned int i = 0; i < num_items; ++i)
    {
      free(items[i].label);
      free(items[i].path);
    }
    free(items);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_VFS::CFileItemListToVFSDirEntries(VFSDirEntry* entries, unsigned int num_entries, const CFileItemList& items)
{
  if (!entries)
    return;

  int toCopy = std::min(num_entries, (unsigned int)items.Size());

  for (int i = 0; i < toCopy; ++i)
  {
    entries[i].label = strdup(items[i]->GetLabel().c_str());
    entries[i].path = strdup(items[i]->GetPath().c_str());
    entries[i].size = items[i]->m_dwSize;
    entries[i].folder = items[i]->m_bIsFolder;
  }
}

void* Interface_VFS::open_file(void* kodiInstance, const char* filename, unsigned int flags)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', filename='%p)", __FUNCTION__, addon, filename);
    return nullptr;
  }

  try
  {
    CFile* file = new CFile;
    if (file->Open(filename, flags))
      return ((void*)file);

    delete file;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_VFS::open_file_for_write(void* kodiInstance, const char* filename, bool overwrite)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', filename='%p)", __FUNCTION__, addon, filename);
    return nullptr;
  }
  
  try
  {
    CFile* file = new CFile;
    if (file->OpenForWrite(filename, overwrite))
      return ((void*)file);

    delete file;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

ssize_t Interface_VFS::read_file(void* kodiInstance, void* file, void* ptr, size_t size)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr || ptr == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p, ptr='%p)", __FUNCTION__, addon, file, ptr);
    return -1;
  }

  try
  {
    return static_cast<CFile*>(file)->Read(ptr, size);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

bool Interface_VFS::read_file_string(void* kodiInstance, void* file, char *szLine, int iLineLength)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p)", __FUNCTION__, addon, file);
    return false;
  }

  try
  {
    return static_cast<CFile*>(file)->ReadString(szLine, iLineLength);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

ssize_t Interface_VFS::write_file(void* kodiInstance, void* file, const void* ptr, size_t size)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr || ptr == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p, ptr='%p)", __FUNCTION__, addon, file, ptr);
    return -1;
  }

  try
  {
    return static_cast<CFile*>(file)->Write(ptr, size);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

void Interface_VFS::flush_file(void* kodiInstance, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p)", __FUNCTION__, addon, file);
    return;
  }

  try
  {
    static_cast<CFile*>(file)->Flush();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

int64_t Interface_VFS::seek_file(void* kodiInstance, void* file, int64_t position, int whence)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p)", __FUNCTION__, addon, file);
    return 0.0f;
  }

  try
  {
    return static_cast<CFile*>(file)->Seek(position, whence);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0;
}

int Interface_VFS::truncate_file(void* kodiInstance, void* file, int64_t size)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p)", __FUNCTION__, addon, file);
    return 0.0f;
  }

  try
  {
    return static_cast<CFile*>(file)->Truncate(size);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0;
}

int64_t Interface_VFS::get_file_position(void* kodiInstance, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p)", __FUNCTION__, addon, file);
    return 0;
  }

  try
  {
    return static_cast<CFile*>(file)->GetPosition();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0;
}

int64_t Interface_VFS::get_file_length(void* kodiInstance, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p)", __FUNCTION__, addon, file);
    return 0;
  }
  
  try
  {
    return static_cast<CFile*>(file)->GetLength();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0;
}

double Interface_VFS::get_file_download_speed(void* kodiInstance, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p)", __FUNCTION__, addon, file);
    return 0.0f;
  }

  try
  {
    return static_cast<CFile*>(file)->GetDownloadSpeed();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0.0f;
}

void Interface_VFS::close_file(void* kodiInstance, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p)", __FUNCTION__, addon, file);
    return;
  }

  try
  {
    static_cast<CFile*>(file)->Close();
    delete static_cast<CFile*>(file);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

int Interface_VFS::get_file_chunk_size(void* kodiInstance, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p)", __FUNCTION__, addon, file);
    return 0;
  }

  try
  {
    return static_cast<CFile*>(file)->GetChunkSize();
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0;
}

bool Interface_VFS::file_exists(void* kodiInstance, const char *filename, bool usecache)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', filename='%p)", __FUNCTION__, addon, filename);
    return false;
  }

  try
  {
    return CFile::Exists(filename, usecache);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

int Interface_VFS::stat_file(void* kodiInstance, const char *filename, struct __stat64* buffer)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || filename == nullptr || buffer == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', filename='%p, buffer='%p')", __FUNCTION__, addon, filename, buffer);
    return 0;
  }

  try
  {
    return CFile::Stat(filename, buffer);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return 0;
}

bool Interface_VFS::delete_file(void* kodiInstance, const char *filename)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', filename='%p)", __FUNCTION__, addon, filename);
    return false;
  }

  try
  {
    return CFile::Delete(filename);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

char* Interface_VFS::get_file_md5(void* kodiInstance, const char* filename)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', filename='%p)", __FUNCTION__, addon, filename);
    return nullptr;
  }

  try
  {
    std::string string = CUtil::GetFileMD5(filename);
    char* buffer = strdup(string.c_str());
    return buffer;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

char* Interface_VFS::get_cache_thumb_name(void* kodiInstance, const char* filename)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', filename='%p)", __FUNCTION__, addon, filename);
    return nullptr;
  }

  try
  {
    Crc32 crc;
    crc.ComputeFromLowerCase(filename);
    std::string string = StringUtils::Format("%08x.tbn", (unsigned __int32)crc);
    char* buffer = strdup(string.c_str());
    return buffer;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

char* Interface_VFS::make_legal_filename(void* kodiInstance, const char* filename)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', filename='%p)", __FUNCTION__, addon, filename);
    return nullptr;
  }

  try
  {
    std::string string = CUtil::MakeLegalFileName(filename);;
    char* buffer = strdup(string.c_str());
    return buffer;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

char* Interface_VFS::make_legal_path(void* kodiInstance, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', path='%p)", __FUNCTION__, addon, path);
    return nullptr;
  }
  
  try
  {
    std::string string = CUtil::MakeLegalPath(path);;
    char* buffer = strdup(string.c_str());
    return buffer;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

void* Interface_VFS::curl_create(void* kodiInstance, const char* url)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || url == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', url='%p')", __FUNCTION__, addon, url);
    return nullptr;
  }

  try
  {
    CFile* file = new CFile;
    if (file->CURLCreate(url))
      return static_cast<void*>(file);

    delete file;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

bool Interface_VFS::curl_add_option(void* kodiInstance, void* file, int type, const char* name, const char* value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr || name == nullptr || value == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p', name='%p', value='%p')", __FUNCTION__, addon, file, name, value);
    return false;
  }

  try
  {
    XFILE::CURLOPTIONTYPE internalType;
    switch (type)
    {
    case ADDON_CURL_OPTION_OPTION:
      internalType = XFILE::CURL_OPTION_OPTION;
      break;
    case ADDON_CURL_OPTION_PROTOCOL:
      internalType = XFILE::CURL_OPTION_PROTOCOL;
      break;
    case ADDON_CURL_OPTION_CREDENTIALS:
      internalType = XFILE::CURL_OPTION_CREDENTIALS;
      break;
    case ADDON_CURL_OPTION_HEADER:
      internalType = XFILE::CURL_OPTION_HEADER;
      break;
    default:
      throw std::logic_error("Interface_VFS::curl_add_option - invalid curl option type");
      return false;
    };

    CFile* cfile = static_cast<CFile*>(file);
    if (cfile)
      return cfile->CURLAddOption(internalType, name, value);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

bool Interface_VFS::curl_open(void* kodiInstance, void* file, unsigned int flags)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::%s - invalid data (addon='%p', file='%p')", __FUNCTION__, addon, file);
    return false;
  }

  try
  {
    CFile* cfile = static_cast<CFile*>(file);
    if (cfile)
      return cfile->CURLOpen(flags);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

} /* namespace ADDON */
