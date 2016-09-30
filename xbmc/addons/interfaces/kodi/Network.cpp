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

#include "Network.h"

#include "addons/kodi-addon-dev-kit/include/kodi/Network.h"

#include "Application.h"
#include "PasswordManager.h"
#include "URL.h"
#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "network/DNSNameCache.h"
#include "network/Network.h"

using namespace kodi; // addon-dev-kit namespace

namespace ADDON
{

void Interface_Network::Init(sFuncTable_Addon *funcTable)
{
  funcTable->toKodi.kodi_network = (sAddonToKodiFuncTable_kodi_network*)malloc(sizeof(sAddonToKodiFuncTable_kodi_network));
  funcTable->toKodi.kodi_network->wake_on_lan = wake_on_lan;
  funcTable->toKodi.kodi_network->get_ip_address = get_ip_address;
  funcTable->toKodi.kodi_network->dns_lookup = dns_lookup;
  funcTable->toKodi.kodi_network->url_encode = url_encode;
}

void Interface_Network::DeInit(sFuncTable_Addon* funcTable)
{
  if (funcTable->toKodi.kodi_network)
  {
    free(funcTable->toKodi.kodi_network);
    funcTable->toKodi.kodi_network = nullptr;
  }
}

bool Interface_Network::wake_on_lan(void* kodiInstance, const char* mac)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || mac == nullptr)
  {
    CLog::Log(LOGERROR, "kodi::Network::%s - invalid data (addon='%p', mac='%p')", __FUNCTION__, addon, mac);
    return false;
  }

  try
  {
    return g_application.getNetwork().WakeOnLan(mac);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return false;
}

char* Interface_Network::get_ip_address(void* kodiInstance)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr)
  {
    CLog::Log(LOGERROR, "kodi::Network::%s - invalid data (addon='%p')", __FUNCTION__, addon);
    return nullptr;
  }

  try
  {
    std::string titleIP;
    CNetworkInterface* iface = g_application.getNetwork().GetFirstConnectedInterface();
    if (iface)
      titleIP = iface->GetCurrentIPAddress();
    else
      titleIP = "127.0.0.1";

    char* buffer = nullptr;
    if (!titleIP.empty())
      buffer = strdup(titleIP.c_str());
    return buffer;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

char* Interface_Network::dns_lookup(void* kodiInstance, const char* url, bool* ret)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || url == nullptr || ret == nullptr)
  {
    CLog::Log(LOGERROR, "kodi::Network::%s - invalid data (addon='%p', url='%p', ret='%p')", __FUNCTION__, addon, url, ret);
    return nullptr;
  }

  try
  {
    std::string string;
    *ret = CDNSNameCache::Lookup(url, string);
    char* buffer = nullptr;
    if (!string.empty())
      buffer = strdup(string.c_str());
    return buffer;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

char* Interface_Network::url_encode(void* kodiInstance, const char* url)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || url == nullptr)
  {
    CLog::Log(LOGERROR, "kodi::Network::%s - invalid data (addon='%p', url='%p')", __FUNCTION__, addon, url);
    return nullptr;
  }

  try
  {
    std::string string = CURL::Encode(url);
    char* buffer = nullptr;
    if (!string.empty())
      buffer = strdup(string.c_str());
    return buffer;
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return nullptr;
}

} /* namespace ADDON */
