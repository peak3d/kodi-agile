/*
 *      Copyright (C) 2012-2015 Team Kodi
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

#include "Application.h"
#include "addons/kodi-addon-dev-kit/include/kodi/libKODI_guilib.h"
#include "epg/Epg.h"
#include "messaging/ApplicationMessenger.h"
#include "messaging/helpers/DialogHelper.h"
#include "settings/AdvancedSettings.h"
#include "settings/Settings.h"
#include "utils/log.h"
#include "utils/StringUtils.h"
#include "utils/Variant.h"

#include "pvr/PVRManager.h"
#include "pvr/addons/PVRClients.h"
#include "pvr/channels/PVRChannelGroupsContainer.h"
#include "pvr/recordings/PVRRecordings.h"
#include "pvr/timers/PVRTimers.h"
#include "pvr/timers/PVRTimerInfoTag.h"
#include "pvr/timers/PVRTimerType.h"

#include "PVRClient.h"

#include <assert.h>
#include <memory>
#include <algorithm>

using namespace ADDON;
using namespace PVR;
using namespace EPG;
using namespace KODI::MESSAGING;

using KODI::MESSAGING::HELPERS::DialogResponse;

#define DEFAULT_INFO_STRING_VALUE "unknown"

std::unique_ptr<CPVRClient> CPVRClient::FromExtension(AddonProps props, const cp_extension_t* ext)
{
  std::string strAvahiType = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@avahi_type");
  std::string strAvahiIpSetting = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@avahi_ip_setting");
  std::string strAvahiPortSetting = CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@avahi_port_setting");
  return std::unique_ptr<CPVRClient>(new CPVRClient(std::move(props), strAvahiType,
      strAvahiIpSetting, strAvahiPortSetting));
}

CPVRClient::CPVRClient(AddonProps props)
  : CAddonDll(std::move(props)),
    m_apiVersion("0.0.0"),
    m_bAvahiServiceAdded(false),
    m_addonInstance(nullptr)
{
  ResetProperties();
}

CPVRClient::CPVRClient(AddonProps props, const std::string& strAvahiType, const std::string& strAvahiIpSetting,
    const std::string& strAvahiPortSetting)
  : CAddonDll(std::move(props)),
    m_strAvahiType(strAvahiType),
    m_strAvahiIpSetting(strAvahiIpSetting),
    m_strAvahiPortSetting(strAvahiPortSetting),
    m_apiVersion("0.0.0"),
    m_bAvahiServiceAdded(false),
    m_addonInstance(nullptr)
{
  ResetProperties();
}

CPVRClient::~CPVRClient(void)
{
  if (m_bAvahiServiceAdded)
    CZeroconfBrowser::GetInstance()->RemoveServiceType(m_strAvahiType);
  Destroy();
}

void CPVRClient::OnDisabled()
{
  CAddon::OnDisabled();
  CPVRManager::GetInstance().Clients()->UpdateAddons();
}

void CPVRClient::OnEnabled()
{
  CAddon::OnEnabled();
  CPVRManager::GetInstance().Clients()->UpdateAddons();
}

void CPVRClient::OnPostInstall(bool update, bool modal)
{
  CAddon::OnPostInstall(update, modal);
  CPVRManager::GetInstance().Clients()->UpdateAddons();
}

void CPVRClient::OnPreUnInstall()
{
  // stop the pvr manager, so running pvr add-ons are stopped and closed
  PVR::CPVRManager::GetInstance().Stop();
  CAddon::OnPreUnInstall();
}

void CPVRClient::OnPostUnInstall()
{
  CAddon::OnPostUnInstall();
  CPVRManager::GetInstance().Clients()->UpdateAddons();
}

ADDON::AddonPtr CPVRClient::GetRunningInstance() const
{
  ADDON::AddonPtr addon;
  CPVRManager::GetInstance().Clients()->GetClient(ID(), addon);
  return addon;
}

void CPVRClient::ResetProperties(int iClientId /* = PVR_INVALID_CLIENT_ID */)
{
  /* initialise members */
  m_strUserPath           = CSpecialProtocol::TranslatePath(Profile());
  m_props.strUserPath     = m_strUserPath.c_str();
  m_strClientPath         = CSpecialProtocol::TranslatePath(Path());
  m_props.strClientPath   = m_strClientPath.c_str();
  m_props.iEpgMaxDays     = CSettings::GetInstance().GetInt(CSettings::SETTING_EPG_DAYSTODISPLAY);
  m_menuhooks.clear();
  m_timertypes.clear();
  m_bReadyToUse           = false;
  m_connectionState       = PVR_CONNECTION_STATE_UNKNOWN;
  m_prevConnectionState   = PVR_CONNECTION_STATE_UNKNOWN;
  m_ignoreClient          = false;
  m_iClientId             = iClientId;
  m_strBackendVersion     = DEFAULT_INFO_STRING_VALUE;
  m_strConnectionString   = DEFAULT_INFO_STRING_VALUE;
  m_strFriendlyName       = DEFAULT_INFO_STRING_VALUE;
  m_strBackendName        = DEFAULT_INFO_STRING_VALUE;
  m_strBackendHostname.clear();
  m_bIsPlayingTV          = false;
  m_bIsPlayingRecording   = false;
  memset(&m_addonCapabilities, 0, sizeof(m_addonCapabilities));
  m_apiVersion = AddonVersion("0.0.0");
  
  memset(&m_struct, 0, sizeof(m_struct));
}

ADDON_STATUS CPVRClient::Create(int iClientId)
{
  ADDON_STATUS status(ADDON_STATUS_UNKNOWN);
  if (iClientId <= PVR_INVALID_CLIENT_ID)
    return status;

  /* reset all properties to defaults */
  ResetProperties(iClientId);

  /* initialise the add-on */
  bool bReadyToUse(false);
  CLog::Log(LOGDEBUG, "PVR - %s - creating PVR add-on instance '%s'", __FUNCTION__, Name().c_str());
  try
  {
    if ((status = CAddonDll::Create()) != ADDON_STATUS_OK)
      return status;
    
    if ((status = CAddonDll::CreateInstance(ADDON_INSTANCE_PVR, ID().c_str(), &m_props, &m_struct, this, &m_addonInstance)) != ADDON_STATUS_OK)
      return status;

    bReadyToUse = GetAddonProperties();
  }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }

  m_bReadyToUse = bReadyToUse;
  return status;
}

bool CPVRClient::DllLoaded(void) const
{
  try { return CAddonDll::DllLoaded(); }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }

  return false;
}

void CPVRClient::Destroy(void)
{
  if (!m_bReadyToUse)
    return;
  m_bReadyToUse = false;

  /* reset 'ready to use' to false */
  CLog::Log(LOGDEBUG, "PVR - %s - destroying PVR add-on '%s'", __FUNCTION__, GetFriendlyName().c_str());

  /* destroy the add-on */
  try
  { 
    CAddonDll::DestroyInstance(ADDON_INSTANCE_PVR, ID().c_str(), m_addonInstance);
    CAddonDll::Destroy();
    m_addonInstance = nullptr;
  }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }

  /* reset all properties to defaults */
  ResetProperties();
}

void CPVRClient::ReCreate(void)
{
  int iClientID(m_iClientId);
  Destroy();

  /* recreate the instance */
  Create(iClientID);
}

bool CPVRClient::ReadyToUse(void) const
{
  return m_bReadyToUse;
}

PVR_CONNECTION_STATE CPVRClient::GetConnectionState(void) const
{
  CSingleLock lock(m_critSection);
  return m_connectionState;
}

void CPVRClient::SetConnectionState(PVR_CONNECTION_STATE state)
{
  CSingleLock lock(m_critSection);

  m_prevConnectionState = m_connectionState;
  m_connectionState = state;

  if (m_connectionState == PVR_CONNECTION_STATE_CONNECTED)
    m_ignoreClient = false;
  else if (m_connectionState == PVR_CONNECTION_STATE_CONNECTING &&
           m_prevConnectionState == PVR_CONNECTION_STATE_UNKNOWN)
    m_ignoreClient = true;
}

PVR_CONNECTION_STATE CPVRClient::GetPreviousConnectionState(void) const
{
  CSingleLock lock(m_critSection);
  return m_prevConnectionState;
}

bool CPVRClient::IgnoreClient(void) const
{
  CSingleLock lock(m_critSection);
  return m_ignoreClient;
}

int CPVRClient::GetID(void) const
{
  return m_iClientId;
}

/*!
 * @brief Copy over group info from xbmcGroup to addonGroup.
 * @param xbmcGroup The group on XBMC's side.
 * @param addonGroup The group on the addon's side.
 */
void CPVRClient::WriteClientGroupInfo(const CPVRChannelGroup &xbmcGroup, PVR_CHANNEL_GROUP &addonGroup)
{
  memset(&addonGroup, 0, sizeof(addonGroup));

  addonGroup.bIsRadio     = xbmcGroup.IsRadio();
  strncpy(addonGroup.strGroupName, xbmcGroup.GroupName().c_str(), sizeof(addonGroup.strGroupName) - 1);
}

/*!
 * @brief Copy over recording info from xbmcRecording to addonRecording.
 * @param xbmcRecording The recording on XBMC's side.
 * @param addonRecording The recording on the addon's side.
 */
void CPVRClient::WriteClientRecordingInfo(const CPVRRecording &xbmcRecording, PVR_RECORDING &addonRecording)
{
  time_t recTime;
  xbmcRecording.RecordingTimeAsUTC().GetAsTime(recTime);

  memset(&addonRecording, 0, sizeof(addonRecording));

  addonRecording.recordingTime       = recTime - g_advancedSettings.m_iPVRTimeCorrection;
  strncpy(addonRecording.strRecordingId, xbmcRecording.m_strRecordingId.c_str(), sizeof(addonRecording.strRecordingId) - 1);
  strncpy(addonRecording.strTitle, xbmcRecording.m_strTitle.c_str(), sizeof(addonRecording.strTitle) - 1);
  strncpy(addonRecording.strPlotOutline, xbmcRecording.m_strPlotOutline.c_str(), sizeof(addonRecording.strPlotOutline) - 1);
  strncpy(addonRecording.strPlot, xbmcRecording.m_strPlot.c_str(), sizeof(addonRecording.strPlot) - 1);
  strncpy(addonRecording.strChannelName, xbmcRecording.m_strChannelName.c_str(), sizeof(addonRecording.strChannelName) - 1);
  addonRecording.iDuration           = xbmcRecording.GetDuration();
  addonRecording.iPriority           = xbmcRecording.m_iPriority;
  addonRecording.iLifetime           = xbmcRecording.m_iLifetime;
  addonRecording.iPlayCount          = xbmcRecording.m_playCount;
  addonRecording.iLastPlayedPosition = (int)xbmcRecording.m_resumePoint.timeInSeconds;
  addonRecording.bIsDeleted          = xbmcRecording.IsDeleted();
  strncpy(addonRecording.strDirectory, xbmcRecording.m_strDirectory.c_str(), sizeof(addonRecording.strDirectory) - 1);
  strncpy(addonRecording.strStreamURL, xbmcRecording.m_strStreamURL.c_str(), sizeof(addonRecording.strStreamURL) - 1);
  strncpy(addonRecording.strIconPath, xbmcRecording.m_strIconPath.c_str(), sizeof(addonRecording.strIconPath) - 1);
  strncpy(addonRecording.strThumbnailPath, xbmcRecording.m_strThumbnailPath.c_str(), sizeof(addonRecording.strThumbnailPath) - 1);
  strncpy(addonRecording.strFanartPath, xbmcRecording.m_strFanartPath.c_str(), sizeof(addonRecording.strFanartPath) - 1);
}

/*!
 * @brief Copy over timer info from xbmcTimer to addonTimer.
 * @param xbmcTimer The timer on XBMC's side.
 * @param addonTimer The timer on the addon's side.
 */
void CPVRClient::WriteClientTimerInfo(const CPVRTimerInfoTag &xbmcTimer, PVR_TIMER &addonTimer)
{
  time_t start, end, firstDay;
  xbmcTimer.StartAsUTC().GetAsTime(start);
  xbmcTimer.EndAsUTC().GetAsTime(end);
  xbmcTimer.FirstDayAsUTC().GetAsTime(firstDay);
  CEpgInfoTagPtr epgTag = xbmcTimer.GetEpgInfoTag();

  memset(&addonTimer, 0, sizeof(addonTimer));

  addonTimer.iClientIndex              = xbmcTimer.m_iClientIndex;
  addonTimer.iParentClientIndex        = xbmcTimer.m_iParentClientIndex;
  addonTimer.state                     = xbmcTimer.m_state;
  addonTimer.iTimerType                = xbmcTimer.GetTimerType() ? xbmcTimer.GetTimerType()->GetTypeId() : PVR_TIMER_TYPE_NONE;
  addonTimer.iClientChannelUid         = xbmcTimer.m_iClientChannelUid;
  strncpy(addonTimer.strTitle, xbmcTimer.m_strTitle.c_str(), sizeof(addonTimer.strTitle) - 1);
  strncpy(addonTimer.strEpgSearchString, xbmcTimer.m_strEpgSearchString.c_str(), sizeof(addonTimer.strEpgSearchString) - 1);
  addonTimer.bFullTextEpgSearch        = xbmcTimer.m_bFullTextEpgSearch;
  strncpy(addonTimer.strDirectory, xbmcTimer.m_strDirectory.c_str(), sizeof(addonTimer.strDirectory) - 1);
  addonTimer.iPriority                 = xbmcTimer.m_iPriority;
  addonTimer.iLifetime                 = xbmcTimer.m_iLifetime;
  addonTimer.iMaxRecordings            = xbmcTimer.m_iMaxRecordings;
  addonTimer.iPreventDuplicateEpisodes = xbmcTimer.m_iPreventDupEpisodes;
  addonTimer.iRecordingGroup           = xbmcTimer.m_iRecordingGroup;
  addonTimer.iWeekdays                 = xbmcTimer.m_iWeekdays;
  addonTimer.startTime                 = start - g_advancedSettings.m_iPVRTimeCorrection;
  addonTimer.endTime                   = end - g_advancedSettings.m_iPVRTimeCorrection;
  addonTimer.bStartAnyTime             = xbmcTimer.m_bStartAnyTime;
  addonTimer.bEndAnyTime               = xbmcTimer.m_bEndAnyTime;
  addonTimer.firstDay                  = firstDay - g_advancedSettings.m_iPVRTimeCorrection;
  addonTimer.iEpgUid                   = epgTag ? epgTag->UniqueBroadcastID() : PVR_TIMER_NO_EPG_UID;
  strncpy(addonTimer.strSummary, xbmcTimer.m_strSummary.c_str(), sizeof(addonTimer.strSummary) - 1);
  addonTimer.iMarginStart              = xbmcTimer.m_iMarginStart;
  addonTimer.iMarginEnd                = xbmcTimer.m_iMarginEnd;
  addonTimer.iGenreType                = epgTag ? epgTag->GenreType() : 0;
  addonTimer.iGenreSubType             = epgTag ? epgTag->GenreSubType() : 0;
}

/*!
 * @brief Copy over channel info from xbmcChannel to addonClient.
 * @param xbmcChannel The channel on XBMC's side.
 * @param addonChannel The channel on the addon's side.
 */
void CPVRClient::WriteClientChannelInfo(const CPVRChannelPtr &xbmcChannel, PVR_CHANNEL &addonChannel)
{
  assert(xbmcChannel.get());

  memset(&addonChannel, 0, sizeof(addonChannel));

  addonChannel.iUniqueId         = xbmcChannel->UniqueID();
  addonChannel.iChannelNumber    = xbmcChannel->ClientChannelNumber();
  addonChannel.iSubChannelNumber = xbmcChannel->ClientSubChannelNumber();
  strncpy(addonChannel.strChannelName, xbmcChannel->ClientChannelName().c_str(), sizeof(addonChannel.strChannelName) - 1);
  strncpy(addonChannel.strIconPath, xbmcChannel->IconPath().c_str(), sizeof(addonChannel.strIconPath) - 1);
  addonChannel.iEncryptionSystem = xbmcChannel->EncryptionSystem();
  addonChannel.bIsRadio          = xbmcChannel->IsRadio();
  addonChannel.bIsHidden         = xbmcChannel->IsHidden();
  strncpy(addonChannel.strInputFormat, xbmcChannel->InputFormat().c_str(), sizeof(addonChannel.strInputFormat) - 1);
  strncpy(addonChannel.strStreamURL, xbmcChannel->StreamURL().c_str(), sizeof(addonChannel.strStreamURL) - 1);
}

bool CPVRClient::IsCompatibleAPIVersion(const ADDON::AddonVersion &minVersion, const ADDON::AddonVersion &version)
{
  AddonVersion myMinVersion = AddonVersion(XBMC_PVR_MIN_API_VERSION);
  AddonVersion myVersion = AddonVersion(XBMC_PVR_API_VERSION);
  return (version >= myMinVersion && minVersion <= myVersion);
}

bool CPVRClient::IsCompatibleGUIAPIVersion(const ADDON::AddonVersion &minVersion, const ADDON::AddonVersion &version)
{
  AddonVersion myMinVersion = AddonVersion(KODI_GUILIB_MIN_API_VERSION);
  AddonVersion myVersion = AddonVersion(KODI_GUILIB_API_VERSION);
  return (version >= myMinVersion && minVersion <= myVersion);
}

bool CPVRClient::CheckAPIVersion(void)
{
  /* check the API version */
  AddonVersion minVersion = AddonVersion(XBMC_PVR_MIN_API_VERSION);
  try { m_apiVersion = AddonVersion(m_struct.GetPVRAPIVersion(m_addonInstance)); }
  catch (std::exception &e) { LogException(e, "GetPVRAPIVersion()"); return false;  }

  if (!IsCompatibleAPIVersion(minVersion, m_apiVersion))
  {
    CLog::Log(LOGERROR, "PVR - Add-on '%s' is using an incompatible API version. XBMC minimum API version = '%s', add-on API version '%s'", Name().c_str(), minVersion.asString().c_str(), m_apiVersion.asString().c_str());
    return false;
  }

  /* check the GUI API version */
  AddonVersion guiVersion = AddonVersion("0.0.0");
  minVersion = AddonVersion(KODI_GUILIB_MIN_API_VERSION);
  try { guiVersion = AddonVersion(m_struct.GetGUIAPIVersion(m_addonInstance)); }
  catch (std::exception &e) { LogException(e, "GetGUIAPIVersion()"); return false;  }

  /* Only do the check, if add-on depends on GUI API. */
  if (!guiVersion.empty() && !IsCompatibleGUIAPIVersion(minVersion, guiVersion))
  {
    CLog::Log(LOGERROR, "PVR - Add-on '%s' is using an incompatible GUI API version. XBMC minimum GUI API version = '%s', add-on GUI API version '%s'", Name().c_str(), minVersion.asString().c_str(), guiVersion.asString().c_str());
    return false;
  }

  return true;
}

bool CPVRClient::GetAddonProperties(void)
{
  std::string strBackendName, strConnectionString, strFriendlyName, strBackendVersion, strBackendHostname;
  PVR_ADDON_CAPABILITIES addonCapabilities;
  CPVRTimerTypes timerTypes;

  /* get the capabilities */
  try
  {
    memset(&addonCapabilities, 0, sizeof(addonCapabilities));
    PVR_ERROR retVal = m_struct.GetAddonCapabilities(m_addonInstance, &addonCapabilities);
    if (retVal != PVR_ERROR_NO_ERROR)
    {
      CLog::Log(LOGERROR, "PVR - couldn't get the capabilities for add-on '%s'. Please contact the developer of this add-on: %s", GetFriendlyName().c_str(), Author().c_str());
      return false;
    }
  }
  catch (std::exception &e) { LogException(e, "GetAddonCapabilities()"); return false; }

  /* get the name of the backend */
  try { strBackendName = m_struct.GetBackendName(m_addonInstance); }
  catch (std::exception &e) { LogException(e, "GetBackendName()"); return false;  }

  /* get the connection string */
  try { strConnectionString = m_struct.GetConnectionString(m_addonInstance); }
  catch (std::exception &e) { LogException(e, "GetConnectionString()"); return false;  }

  /* display name = backend name:connection string */
  strFriendlyName = StringUtils::Format("%s:%s", strBackendName.c_str(), strConnectionString.c_str());

  /* backend version number */
  try { strBackendVersion = m_struct.GetBackendVersion(m_addonInstance); }
  catch (std::exception &e) { LogException(e, "GetBackendVersion()"); return false;  }

  /* backend hostname */
  try { strBackendHostname = m_struct.GetBackendHostname(m_addonInstance); }
  catch (std::exception &e) { LogException(e, "GetBackendHostname()"); return false; }

  /* timer types */
  if (addonCapabilities.bSupportsTimers)
  {
    try
    {
      std::unique_ptr<PVR_TIMER_TYPE[]> types_array(new PVR_TIMER_TYPE[PVR_ADDON_TIMERTYPE_ARRAY_SIZE]);
      int size = PVR_ADDON_TIMERTYPE_ARRAY_SIZE;

      PVR_ERROR retval = m_struct.GetTimerTypes(m_addonInstance, types_array.get(), &size);

      if (retval == PVR_ERROR_NOT_IMPLEMENTED)
      {
        // begin compat section
        CLog::Log(LOGWARNING, "%s - Addon %s does not support timer types. It will work, but not benefit from the timer features introduced with PVR Addon API 2.0.0", __FUNCTION__, strFriendlyName.c_str());

        // Create standard timer types (mostly) matching the timer functionality available in Isengard.
        // This is for migration only and does not make changes to the addons obsolete. Addons should
        // work and benefit from some UI changes (e.g. some of the timer settings dialog enhancements),
        // but all old problems/bugs due to static attributes and values will remain the same as in
        // Isengard. Also, new features (like epg search) are not available to addons automatically.
        // This code can be removed once all addons actually support the respective PVR Addon API version.

        size = 0;
        // manual one time
        memset(&types_array[size], 0, sizeof(types_array[size]));
        types_array[size].iId         = size + 1;
        types_array[size].iAttributes = PVR_TIMER_TYPE_IS_MANUAL               |
                                        PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE |
                                        PVR_TIMER_TYPE_SUPPORTS_CHANNELS       |
                                        PVR_TIMER_TYPE_SUPPORTS_START_TIME     |
                                        PVR_TIMER_TYPE_SUPPORTS_END_TIME       |
                                        PVR_TIMER_TYPE_SUPPORTS_PRIORITY       |
                                        PVR_TIMER_TYPE_SUPPORTS_LIFETIME       |
                                        PVR_TIMER_TYPE_SUPPORTS_RECORDING_FOLDERS;
        ++size;

        // manual timer rule
        memset(&types_array[size], 0, sizeof(types_array[size]));
        types_array[size].iId         = size + 1;
        types_array[size].iAttributes = PVR_TIMER_TYPE_IS_MANUAL               |
                                        PVR_TIMER_TYPE_IS_REPEATING            |
                                        PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE |
                                        PVR_TIMER_TYPE_SUPPORTS_CHANNELS       |
                                        PVR_TIMER_TYPE_SUPPORTS_START_TIME     |
                                        PVR_TIMER_TYPE_SUPPORTS_END_TIME       |
                                        PVR_TIMER_TYPE_SUPPORTS_PRIORITY       |
                                        PVR_TIMER_TYPE_SUPPORTS_LIFETIME       |
                                        PVR_TIMER_TYPE_SUPPORTS_FIRST_DAY      |
                                        PVR_TIMER_TYPE_SUPPORTS_WEEKDAYS       |
                                        PVR_TIMER_TYPE_SUPPORTS_RECORDING_FOLDERS;
        ++size;

        if (addonCapabilities.bSupportsEPG)
        {
          // One-shot epg-based
          memset(&types_array[size], 0, sizeof(types_array[size]));
          types_array[size].iId         = size + 1;
          types_array[size].iAttributes = PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE    |
                                          PVR_TIMER_TYPE_REQUIRES_EPG_TAG_ON_CREATE |
                                          PVR_TIMER_TYPE_SUPPORTS_CHANNELS          |
                                          PVR_TIMER_TYPE_SUPPORTS_START_TIME        |
                                          PVR_TIMER_TYPE_SUPPORTS_END_TIME          |
                                          PVR_TIMER_TYPE_SUPPORTS_PRIORITY          |
                                          PVR_TIMER_TYPE_SUPPORTS_LIFETIME          |
                                          PVR_TIMER_TYPE_SUPPORTS_RECORDING_FOLDERS;
          ++size;
        }

        retval = PVR_ERROR_NO_ERROR;
        // end compat section
      }

      if (retval == PVR_ERROR_NO_ERROR)
      {
        timerTypes.reserve(size);
        for (int i = 0; i < size; ++i)
        {
          if (types_array[i].iId == PVR_TIMER_TYPE_NONE)
          {
            CLog::Log(LOGERROR, "PVR - invalid timer type supplied by add-on '%s'. Please contact the developer of this add-on: %s", GetFriendlyName().c_str(), Author().c_str());
            continue;
          }

          if (strlen(types_array[i].strDescription) == 0)
          {
            int id;
            if (types_array[i].iAttributes & PVR_TIMER_TYPE_IS_REPEATING)
            {
              id = (types_array[i].iAttributes & PVR_TIMER_TYPE_IS_MANUAL)
                 ? 822  // "Timer rule"
                 : 823; // "Timer rule (guide-based)"
            }
            else
            {
              id = (types_array[i].iAttributes & PVR_TIMER_TYPE_IS_MANUAL)
                 ? 820  // "One time"
                 : 821; // "One time (guide-based)
            }
            std::string descr(g_localizeStrings.Get(id));
            strncpy(types_array[i].strDescription, descr.c_str(), descr.size());
          }
          timerTypes.push_back(CPVRTimerTypePtr(new CPVRTimerType(types_array[i], m_iClientId)));
        }
      }
      else
      {
        CLog::Log(LOGERROR, "PVR - couldn't get the timer types for add-on '%s'. Please contact the developer of this add-on: %s", GetFriendlyName().c_str(), Author().c_str());
        return false;
      }
    }
    catch (std::exception &e) { LogException(e, "GetTimerTypes()"); return false; }
  }

  /* update the members */
  m_strBackendName      = strBackendName;
  m_strConnectionString = strConnectionString;
  m_strFriendlyName     = strFriendlyName;
  m_strBackendVersion   = strBackendVersion;
  m_addonCapabilities   = addonCapabilities;
  m_strBackendHostname  = strBackendHostname;
  m_timertypes          = timerTypes;

  return true;
}

PVR_ADDON_CAPABILITIES CPVRClient::GetAddonCapabilities(void) const
{
  PVR_ADDON_CAPABILITIES addonCapabilities(m_addonCapabilities);
  return addonCapabilities;
}

const std::string& CPVRClient::GetBackendName(void) const
{
  return m_strBackendName;
}

const std::string& CPVRClient::GetBackendVersion(void) const
{
  return m_strBackendVersion;
}

const std::string& CPVRClient::GetBackendHostname(void) const
{
  return m_strBackendHostname;
}

const std::string& CPVRClient::GetConnectionString(void) const
{
  return m_strConnectionString;
}

const std::string& CPVRClient::GetFriendlyName(void) const
{
  return m_strFriendlyName;
}

PVR_ERROR CPVRClient::GetDriveSpace(long long *iTotal, long long *iUsed)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  try { return m_struct.GetDriveSpace(m_addonInstance, iTotal, iUsed); }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }

  /* default to 0 on error */
  *iTotal = 0;
  *iUsed  = 0;

  return PVR_ERROR_UNKNOWN;
}

PVR_ERROR CPVRClient::StartChannelScan(void)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsChannelScan)
    return PVR_ERROR_NOT_IMPLEMENTED;

  try { return m_struct.OpenDialogChannelScan(m_addonInstance); }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }

  return PVR_ERROR_UNKNOWN;
}

PVR_ERROR CPVRClient::OpenDialogChannelAdd(const CPVRChannelPtr &channel)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsChannelSettings)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_CHANNEL addonChannel;
    WriteClientChannelInfo(channel, addonChannel);

    retVal = m_struct.OpenDialogChannelAdd(m_addonInstance, addonChannel);
    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::OpenDialogChannelSettings(const CPVRChannelPtr &channel)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsChannelSettings)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_CHANNEL addonChannel;
    WriteClientChannelInfo(channel, addonChannel);

    retVal = m_struct.OpenDialogChannelSettings(m_addonInstance, addonChannel);
    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::DeleteChannel(const CPVRChannelPtr &channel)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsChannelSettings)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_CHANNEL addonChannel;
    WriteClientChannelInfo(channel, addonChannel);

    retVal = m_struct.DeleteChannel(m_addonInstance, addonChannel);
    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::RenameChannel(const CPVRChannelPtr &channel)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsChannelSettings)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_CHANNEL addonChannel;
    WriteClientChannelInfo(channel, addonChannel);

    retVal = m_struct.RenameChannel(m_addonInstance, addonChannel);
    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

void CPVRClient::CallMenuHook(const PVR_MENUHOOK &hook, const CFileItem *item)
{
  if (!m_bReadyToUse)
    return;

  try {
    PVR_MENUHOOK_DATA hookData;
    hookData.cat = PVR_MENUHOOK_UNKNOWN;

    if (item)
    {
      if (item->IsEPG())
      {
        hookData.cat = PVR_MENUHOOK_EPG;
        hookData.data.iEpgUid = item->GetEPGInfoTag()->UniqueBroadcastID();
      }
      else if (item->IsPVRChannel())
      {
        hookData.cat = PVR_MENUHOOK_CHANNEL;
        WriteClientChannelInfo(item->GetPVRChannelInfoTag(), hookData.data.channel);
      }
      else if (item->IsUsablePVRRecording())
      {
        hookData.cat = PVR_MENUHOOK_RECORDING;
        WriteClientRecordingInfo(*item->GetPVRRecordingInfoTag(), hookData.data.recording);
      }
      else if (item->IsDeletedPVRRecording())
      {
        hookData.cat = PVR_MENUHOOK_DELETED_RECORDING;
        WriteClientRecordingInfo(*item->GetPVRRecordingInfoTag(), hookData.data.recording);
      }
      else if (item->IsPVRTimer())
      {
        hookData.cat = PVR_MENUHOOK_TIMER;
        WriteClientTimerInfo(*item->GetPVRTimerInfoTag(), hookData.data.timer);
      }
    }

    m_struct.MenuHook(m_addonInstance, hook, hookData);
  }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }
}

PVR_ERROR CPVRClient::GetEPGForChannel(const CPVRChannelPtr &channel, CEpg *epg, time_t start /* = 0 */, time_t end /* = 0 */, bool bSaveInDb /* = false*/)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsEPG)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_CHANNEL addonChannel;
    WriteClientChannelInfo(channel, addonChannel);

    ADDON_HANDLE_STRUCT handle;
    handle.callerAddress  = this;
    handle.dataAddress    = epg;
    handle.dataIdentifier = bSaveInDb ? 1 : 0; // used by the callback method CAddonCallbacksPVR::PVRTransferEpgEntry()
    retVal = m_struct.GetEpg(m_addonInstance, &handle,
        addonChannel,
        start ? start - g_advancedSettings.m_iPVRTimeCorrection : 0,
        end ? end - g_advancedSettings.m_iPVRTimeCorrection : 0);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::SetEPGTimeFrame(int iDays)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsEPG)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    retVal = m_struct.SetEPGTimeFrame(m_addonInstance, iDays);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

int CPVRClient::GetChannelGroupsAmount(void)
{
  int iReturn(-EINVAL);

  if (!m_bReadyToUse)
    return iReturn;

  if (!m_addonCapabilities.bSupportsChannelGroups)
    return iReturn;

  try { iReturn = m_struct.GetChannelGroupsAmount(m_addonInstance); }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }

  return iReturn;
}

PVR_ERROR CPVRClient::GetChannelGroups(CPVRChannelGroups *groups)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsChannelGroups)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    ADDON_HANDLE_STRUCT handle;
    handle.callerAddress = this;
    handle.dataAddress = groups;
    retVal = m_struct.GetChannelGroups(m_addonInstance, &handle, groups->IsRadio());

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::GetChannelGroupMembers(CPVRChannelGroup *group)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsChannelGroups)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    ADDON_HANDLE_STRUCT handle;
    handle.callerAddress = this;
    handle.dataAddress = group;

    PVR_CHANNEL_GROUP tag;
    WriteClientGroupInfo(*group, tag);

    CLog::Log(LOGDEBUG, "PVR - %s - get group members for group '%s' from add-on '%s'",
        __FUNCTION__, tag.strGroupName, GetFriendlyName().c_str());
    retVal = m_struct.GetChannelGroupMembers(m_addonInstance, &handle, tag);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

int CPVRClient::GetChannelsAmount(void)
{
  int iReturn(-EINVAL);
  if (m_bReadyToUse && (m_addonCapabilities.bSupportsTV || m_addonCapabilities.bSupportsRadio))
  {
    try { iReturn = m_struct.GetChannelsAmount(m_addonInstance); }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }

  return iReturn;
}

PVR_ERROR CPVRClient::GetChannels(CPVRChannelGroup &channels, bool radio)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if ((!m_addonCapabilities.bSupportsRadio && radio) ||
      (!m_addonCapabilities.bSupportsTV && !radio))
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);

  try
  {
    ADDON_HANDLE_STRUCT handle;
    handle.callerAddress = this;
    handle.dataAddress = (CPVRChannelGroup*) &channels;
    retVal = m_struct.GetChannels(m_addonInstance, &handle, radio);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

int CPVRClient::GetRecordingsAmount(bool deleted)
{
  int iReturn(-EINVAL);

  if (!m_addonCapabilities.bSupportsRecordings || (deleted && !m_addonCapabilities.bSupportsRecordingsUndelete))
    return iReturn;

  try
  {
    iReturn = m_struct.GetRecordingsAmount(m_addonInstance, deleted);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return iReturn;
}

PVR_ERROR CPVRClient::GetRecordings(CPVRRecordings *results, bool deleted)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsRecordings || (deleted && !m_addonCapabilities.bSupportsRecordingsUndelete))
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    ADDON_HANDLE_STRUCT handle;
    handle.callerAddress = this;
    handle.dataAddress = (CPVRRecordings*) results;
    retVal = m_struct.GetRecordings(m_addonInstance, &handle, deleted);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::DeleteRecording(const CPVRRecording &recording)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsRecordings)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_RECORDING tag;
    WriteClientRecordingInfo(recording, tag);

    retVal = m_struct.DeleteRecording(m_addonInstance, tag);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::UndeleteRecording(const CPVRRecording &recording)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsRecordingsUndelete)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_RECORDING tag;
    WriteClientRecordingInfo(recording, tag);

    retVal = m_struct.UndeleteRecording(m_addonInstance, tag);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::DeleteAllRecordingsFromTrash()
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsRecordingsUndelete)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    retVal = m_struct.DeleteAllRecordingsFromTrash(m_addonInstance);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::RenameRecording(const CPVRRecording &recording)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsRecordings)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_RECORDING tag;
    WriteClientRecordingInfo(recording, tag);

    retVal = m_struct.RenameRecording(m_addonInstance, tag);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::SetRecordingPlayCount(const CPVRRecording &recording, int count)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsRecordingPlayCount)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_RECORDING tag;
    WriteClientRecordingInfo(recording, tag);

    retVal = m_struct.SetRecordingPlayCount(m_addonInstance, tag, count);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::SetRecordingLastPlayedPosition(const CPVRRecording &recording, int lastplayedposition)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsLastPlayedPosition)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_RECORDING tag;
    WriteClientRecordingInfo(recording, tag);

    retVal = m_struct.SetRecordingLastPlayedPosition(m_addonInstance, tag, lastplayedposition);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

int CPVRClient::GetRecordingLastPlayedPosition(const CPVRRecording &recording)
{
  int iReturn(-EINVAL);
  if (!m_bReadyToUse)
    return iReturn;

  if (!m_addonCapabilities.bSupportsLastPlayedPosition)
    return iReturn;

  try
  {
    PVR_RECORDING tag;
    WriteClientRecordingInfo(recording, tag);

    iReturn = m_struct.GetRecordingLastPlayedPosition(m_addonInstance, tag);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return iReturn;
}

std::vector<PVR_EDL_ENTRY> CPVRClient::GetRecordingEdl(const CPVRRecording &recording)
{
  std::vector<PVR_EDL_ENTRY> edl;
  if (!m_bReadyToUse)
    return edl;

  if (!m_addonCapabilities.bSupportsRecordingEdl)
    return edl;

  try
  {
    PVR_RECORDING tag;
    WriteClientRecordingInfo(recording, tag);

    PVR_EDL_ENTRY edl_array[PVR_ADDON_EDL_LENGTH];
    int size = PVR_ADDON_EDL_LENGTH;
    PVR_ERROR retval = m_struct.GetRecordingEdl(m_addonInstance, tag, edl_array, &size);
    if (retval == PVR_ERROR_NO_ERROR)
    {
      edl.reserve(size);
      for (int i = 0; i < size; ++i)
      {
        edl.push_back(edl_array[i]);
      }
    }
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return edl;
}

int CPVRClient::GetTimersAmount(void)
{
  int iReturn(-EINVAL);
  if (!m_bReadyToUse)
    return iReturn;

  if (!m_addonCapabilities.bSupportsTimers)
    return iReturn;

  try { iReturn = m_struct.GetTimersAmount(m_addonInstance); }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }

  return iReturn;
}

PVR_ERROR CPVRClient::GetTimers(CPVRTimers *results)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsTimers)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    ADDON_HANDLE_STRUCT handle;
    handle.callerAddress = this;
    handle.dataAddress = (CPVRTimers*) results;
    retVal = m_struct.GetTimers(m_addonInstance, &handle);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::AddTimer(const CPVRTimerInfoTag &timer)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsTimers)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_TIMER tag;
    WriteClientTimerInfo(timer, tag);

    retVal = m_struct.AddTimer(m_addonInstance, tag);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::DeleteTimer(const CPVRTimerInfoTag &timer, bool bForce /* = false */)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsTimers)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_TIMER tag;
    WriteClientTimerInfo(timer, tag);

    retVal = m_struct.DeleteTimer(m_addonInstance, tag, bForce);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::RenameTimer(const CPVRTimerInfoTag &timer, const std::string &strNewName)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsTimers)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_TIMER tag;
    WriteClientTimerInfo(timer, tag);

    retVal = m_struct.UpdateTimer(m_addonInstance, tag);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::UpdateTimer(const CPVRTimerInfoTag &timer)
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  if (!m_addonCapabilities.bSupportsTimers)
    return PVR_ERROR_NOT_IMPLEMENTED;

  PVR_ERROR retVal(PVR_ERROR_UNKNOWN);
  try
  {
    PVR_TIMER tag;
    WriteClientTimerInfo(timer, tag);

    retVal = m_struct.UpdateTimer(m_addonInstance, tag);

    LogError(retVal, __FUNCTION__);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return retVal;
}

PVR_ERROR CPVRClient::GetTimerTypes(CPVRTimerTypes& results) const
{
  if (!m_bReadyToUse)
    return PVR_ERROR_SERVER_ERROR;

  results = m_timertypes;
  return PVR_ERROR_NO_ERROR;
}

int CPVRClient::ReadStream(void* lpBuf, int64_t uiBufSize)
{
  if (IsPlayingLiveStream())
  {
    try { return m_struct.ReadLiveStream(m_addonInstance, (unsigned char *)lpBuf, (int)uiBufSize); }
    catch (std::exception &e) { LogException(e, "ReadLiveStream()"); }
  }
  else if (IsPlayingRecording())
  {
    try { return m_struct.ReadRecordedStream(m_addonInstance, (unsigned char *)lpBuf, (int)uiBufSize); }
    catch (std::exception &e) { LogException(e, "ReadRecordedStream()"); }
  }
  return -EINVAL;
}

int64_t CPVRClient::SeekStream(int64_t iFilePosition, int iWhence/* = SEEK_SET*/)
{
  if (IsPlayingLiveStream())
  {
    try { return m_struct.SeekLiveStream(m_addonInstance, iFilePosition, iWhence); }
    catch (std::exception &e) { LogException(e, "SeekLiveStream()"); }
  }
  else if (IsPlayingRecording())
  {
    try { return m_struct.SeekRecordedStream(m_addonInstance, iFilePosition, iWhence); }
    catch (std::exception &e) { LogException(e, "SeekRecordedStream()"); }
  }
  return -EINVAL;
}

bool CPVRClient::SeekTime(int time, bool backwards, double *startpts)
{
  if (IsPlaying())
  {
    try { return m_struct.SeekTime(m_addonInstance, time, backwards, startpts); }
    catch (std::exception &e) { LogException(e, "SeekTime()"); }
  }
  return false;
}

int64_t CPVRClient::GetStreamPosition(void)
{
  if (IsPlayingLiveStream())
  {
    try { return m_struct.PositionLiveStream(m_addonInstance); }
    catch (std::exception &e) { LogException(e, "PositionLiveStream()"); }
  }
  else if (IsPlayingRecording())
  {
    try { return m_struct.PositionRecordedStream(m_addonInstance); }
    catch (std::exception &e) { LogException(e, "PositionRecordedStream()"); }
  }
  return -EINVAL;
}

int64_t CPVRClient::GetStreamLength(void)
{
  if (IsPlayingLiveStream())
  {
    try { return m_struct.LengthLiveStream(m_addonInstance); }
    catch (std::exception &e) { LogException(e, "LengthLiveStream()"); }
  }
  else if (IsPlayingRecording())
  {
    try { return m_struct.LengthRecordedStream(m_addonInstance); }
    catch (std::exception &e) { LogException(e, "LengthRecordedStream()"); }
  }
  return -EINVAL;
}

bool CPVRClient::SwitchChannel(const CPVRChannelPtr &channel)
{
  bool bSwitched(false);

  if (IsPlayingLiveStream() && CanPlayChannel(channel))
  {
    PVR_CHANNEL tag;
    WriteClientChannelInfo(channel, tag);
    try
    {
      bSwitched = m_struct.SwitchChannel(m_addonInstance, tag);
    }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }

  if (bSwitched)
  {
    CPVRChannelPtr currentChannel(g_PVRChannelGroups->GetByUniqueID(channel->UniqueID(), channel->ClientID()));
    CSingleLock lock(m_critSection);
    m_playingChannel = currentChannel;
  }

  return bSwitched;
}

bool CPVRClient::SignalQuality(PVR_SIGNAL_STATUS &qualityinfo)
{
  if (IsPlayingLiveStream())
  {
    try
    {
      return m_struct.SignalStatus(m_addonInstance, qualityinfo) == PVR_ERROR_NO_ERROR;
    }
    catch (std::exception &e)
    {
      LogException(e, __FUNCTION__);
    }
  }
  return false;
}

std::string CPVRClient::GetLiveStreamURL(const CPVRChannelPtr &channel)
{
  std::string strReturn;

  if (!m_bReadyToUse || !CanPlayChannel(channel))
    return strReturn;

  try
  {
    PVR_CHANNEL tag;
    WriteClientChannelInfo(channel, tag);
    strReturn = m_struct.GetLiveStreamURL(m_addonInstance, tag);
  }
  catch (std::exception &e)
  {
    LogException(e, __FUNCTION__);
  }

  return strReturn;
}

PVR_ERROR CPVRClient::GetStreamProperties(PVR_STREAM_PROPERTIES *props)
{
  if (!IsPlaying())
    return PVR_ERROR_REJECTED;

  try { return m_struct.GetStreamProperties(m_addonInstance, props); }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }

  return PVR_ERROR_UNKNOWN;
}

void CPVRClient::DemuxReset(void)
{
  if (m_bReadyToUse && m_addonCapabilities.bHandlesDemuxing)
  {
    try { m_struct.DemuxReset(m_addonInstance); }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }
}

void CPVRClient::DemuxAbort(void)
{
  if (m_bReadyToUse && m_addonCapabilities.bHandlesDemuxing)
  {
    try { m_struct.DemuxAbort(m_addonInstance); }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }
}

void CPVRClient::DemuxFlush(void)
{
  if (m_bReadyToUse && m_addonCapabilities.bHandlesDemuxing)
  {
    try { m_struct.DemuxFlush(m_addonInstance); }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }
}

DemuxPacket* CPVRClient::DemuxRead(void)
{
  if (m_bReadyToUse && m_addonCapabilities.bHandlesDemuxing)
  {
    try { return m_struct.DemuxRead(m_addonInstance); }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }
  return NULL;
}

bool CPVRClient::HaveMenuHooks(PVR_MENUHOOK_CAT cat) const
{
  bool bReturn(false);
  if (m_bReadyToUse && !m_menuhooks.empty())
  {
    for (auto hook : m_menuhooks)
    {
      if (hook.category == cat || hook.category == PVR_MENUHOOK_ALL)
      {
        bReturn = true;
        break;
      }
    }
  }
  return bReturn;
}

PVR_MENUHOOKS *CPVRClient::GetMenuHooks(void)
{
  return &m_menuhooks;
}

const char *CPVRClient::ToString(const PVR_ERROR error)
{
  switch (error)
  {
  case PVR_ERROR_NO_ERROR:
    return "no error";
  case PVR_ERROR_NOT_IMPLEMENTED:
    return "not implemented";
  case PVR_ERROR_SERVER_ERROR:
    return "server error";
  case PVR_ERROR_SERVER_TIMEOUT:
    return "server timeout";
  case PVR_ERROR_RECORDING_RUNNING:
    return "recording already running";
  case PVR_ERROR_ALREADY_PRESENT:
    return "already present";
  case PVR_ERROR_REJECTED:
    return "rejected by the backend";
  case PVR_ERROR_INVALID_PARAMETERS:
    return "invalid parameters for this method";
  case PVR_ERROR_FAILED:
    return "the command failed";
  case PVR_ERROR_UNKNOWN:
  default:
    return "unknown error";
  }
}

bool CPVRClient::LogError(const PVR_ERROR error, const char *strMethod) const
{
  if (error != PVR_ERROR_NO_ERROR)
  {
    CLog::Log(LOGERROR, "PVR - %s - addon '%s' returned an error: %s",
        strMethod, GetFriendlyName().c_str(), ToString(error));
    return false;
  }
  return true;
}

void CPVRClient::LogException(const std::exception &e, const char *strFunctionName) const
{
  CLog::Log(LOGERROR, "PVR - exception '%s' caught while trying to call '%s' on add-on '%s'. Please contact the developer of this add-on: %s", e.what(), strFunctionName, GetFriendlyName().c_str(), Author().c_str());
}

bool CPVRClient::CanPlayChannel(const CPVRChannelPtr &channel) const
{
  assert(channel.get());

  return (m_bReadyToUse &&
           ((m_addonCapabilities.bSupportsTV && !channel->IsRadio()) ||
            (m_addonCapabilities.bSupportsRadio && channel->IsRadio())));
}

bool CPVRClient::SupportsChannelGroups(void) const
{
  return m_addonCapabilities.bSupportsChannelGroups;
}

bool CPVRClient::SupportsChannelScan(void) const
{
  return m_addonCapabilities.bSupportsChannelScan;
}

bool CPVRClient::SupportsChannelSettings(void) const
{
  return m_addonCapabilities.bSupportsChannelSettings;
}

bool CPVRClient::SupportsEPG(void) const
{
  return m_addonCapabilities.bSupportsEPG;
}

bool CPVRClient::SupportsLastPlayedPosition(void) const
{
  return m_addonCapabilities.bSupportsLastPlayedPosition;
}

bool CPVRClient::SupportsRadio(void) const
{
  return m_addonCapabilities.bSupportsRadio;
}

bool CPVRClient::SupportsRecordings(void) const
{
  return m_addonCapabilities.bSupportsRecordings;
}

bool CPVRClient::SupportsRecordingsUndelete(void) const
{
  return m_addonCapabilities.bSupportsRecordingsUndelete;
}

bool CPVRClient::SupportsRecordingPlayCount(void) const
{
  return m_addonCapabilities.bSupportsRecordingPlayCount;
}

bool CPVRClient::SupportsRecordingEdl(void) const
{
  return m_addonCapabilities.bSupportsRecordingEdl;
}

bool CPVRClient::SupportsTimers(void) const
{
  return m_addonCapabilities.bSupportsTimers;
}

bool CPVRClient::SupportsTV(void) const
{
  return m_addonCapabilities.bSupportsTV;
}

bool CPVRClient::HandlesDemuxing(void) const
{
  return m_addonCapabilities.bHandlesDemuxing;
}

bool CPVRClient::HandlesInputStream(void) const
{
  return m_addonCapabilities.bHandlesInputStream;
}

bool CPVRClient::IsPlayingLiveStream(void) const
{
  CSingleLock lock(m_critSection);
  return m_bReadyToUse && m_bIsPlayingTV;
}

bool CPVRClient::IsPlayingLiveTV(void) const
{
  CSingleLock lock(m_critSection);
  return m_bReadyToUse && m_bIsPlayingTV && !m_playingChannel->IsRadio();
}

bool CPVRClient::IsPlayingLiveRadio(void) const
{
  CSingleLock lock(m_critSection);
  return m_bReadyToUse && m_bIsPlayingTV && m_playingChannel->IsRadio();
}

bool CPVRClient::IsPlayingEncryptedChannel(void) const
{
  CSingleLock lock(m_critSection);
  return m_bReadyToUse && m_bIsPlayingTV && m_playingChannel->IsEncrypted();
}

bool CPVRClient::IsPlayingRecording(void) const
{
  CSingleLock lock(m_critSection);
  return m_bReadyToUse && m_bIsPlayingRecording;
}

bool CPVRClient::IsPlaying(void) const
{
  return IsPlayingLiveStream() ||
         IsPlayingRecording();
}

CPVRChannelPtr CPVRClient::GetPlayingChannel() const
{
  CSingleLock lock(m_critSection);
  if (m_bReadyToUse && m_bIsPlayingTV)
    return m_playingChannel;

  return CPVRChannelPtr();
}

CPVRRecordingPtr CPVRClient::GetPlayingRecording(void) const
{
  CSingleLock lock(m_critSection);
  if (m_bReadyToUse && m_bIsPlayingRecording)
    return m_playingRecording;

  return CPVRRecordingPtr();
}

bool CPVRClient::OpenStream(const CPVRChannelPtr &channel, bool bIsSwitchingChannel)
{
  bool bReturn(false);
  CloseStream();

  if(!CanPlayChannel(channel))
  {
    CLog::Log(LOGDEBUG, "add-on '%s' can not play channel '%s'", GetFriendlyName().c_str(), channel->ChannelName().c_str());
  }
  else if (!channel->StreamURL().empty())
  {
    CLog::Log(LOGDEBUG, "opening live stream on url '%s'", channel->StreamURL().c_str());
    bReturn = true;

    // the Njoy N7 sometimes doesn't switch channels, but opens a stream to the previous channel
    // when not waiting for a short period.
    // added in 1.1.0
    AddonVersion checkVersion("1.1.0");
    if (m_apiVersion >= checkVersion)
    {
      unsigned int iWaitTimeMs = m_struct.GetChannelSwitchDelay(m_addonInstance);
      if (iWaitTimeMs > 0)
        XbmcThreads::ThreadSleep(iWaitTimeMs);
    }
  }
  else
  {
    CLog::Log(LOGDEBUG, "opening live stream for channel '%s'", channel->ChannelName().c_str());
    PVR_CHANNEL tag;
    WriteClientChannelInfo(channel, tag);

    try
    {
      bReturn = m_struct.OpenLiveStream(m_addonInstance, tag);
    }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }

  if (bReturn)
  {
    CPVRChannelPtr currentChannel(g_PVRChannelGroups->GetByUniqueID(channel->UniqueID(), channel->ClientID()));
    CSingleLock lock(m_critSection);
    m_playingChannel      = currentChannel;
    m_bIsPlayingTV        = true;
    m_bIsPlayingRecording = false;
  }

  return bReturn;
}

bool CPVRClient::OpenStream(const CPVRRecordingPtr &recording)
{
  bool bReturn(false);
  CloseStream();

  if (m_bReadyToUse && m_addonCapabilities.bSupportsRecordings)
  {
    PVR_RECORDING tag;
    WriteClientRecordingInfo(*recording, tag);

    try
    {
      bReturn = m_struct.OpenRecordedStream(m_addonInstance, tag);
    }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }

  if (bReturn)
  {
    CSingleLock lock(m_critSection);
    m_playingRecording    = recording;
    m_bIsPlayingTV        = false;
    m_bIsPlayingRecording = true;
  }

  return bReturn;
}

void CPVRClient::CloseStream(void)
{
  if (IsPlayingLiveStream())
  {
    try { m_struct.CloseLiveStream(m_addonInstance); }
    catch (std::exception &e) { LogException(e, "CloseLiveStream()"); }

    CSingleLock lock(m_critSection);
    m_bIsPlayingTV = false;
  }
  else if (IsPlayingRecording())
  {
    try { m_struct.CloseRecordedStream(m_addonInstance); }
    catch (std::exception &e) { LogException(e, "CloseRecordedStream()"); }

    CSingleLock lock(m_critSection);
    m_bIsPlayingRecording = false;
  }
}

void CPVRClient::PauseStream(bool bPaused)
{
  if (IsPlaying())
  {
    try { m_struct.PauseStream(m_addonInstance, bPaused); }
    catch (std::exception &e) { LogException(e, "PauseStream()"); }
  }
}

void CPVRClient::SetSpeed(int speed)
{
  if (IsPlaying())
  {
    try { m_struct.SetSpeed(m_addonInstance, speed); }
    catch (std::exception &e) { LogException(e, "SetSpeed()"); }
  }
}

bool CPVRClient::CanPauseStream(void) const
{
  bool bReturn(false);
  if (IsPlaying())
  {
    try
    {
      bReturn = m_struct.CanPauseStream(m_addonInstance);
    }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }

  return bReturn;
}

bool CPVRClient::CanSeekStream(void) const
{
  bool bReturn(false);
  if (IsPlaying())
  {
    try
    {
      bReturn = m_struct.CanSeekStream(m_addonInstance);
    }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }
  return bReturn;
}

bool CPVRClient::IsTimeshifting(void) const
{
  bool bReturn(false);
  if (IsPlaying())
  {
    try
    {
      if (m_struct.IsTimeshifting)
        bReturn = m_struct.IsTimeshifting(m_addonInstance);
    }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }
  return bReturn;
}

time_t CPVRClient::GetPlayingTime(void) const
{
  time_t time = 0;
  if (IsPlaying())
  {
    try
    {
      time = m_struct.GetPlayingTime(m_addonInstance);
    }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }
  // fallback if not implemented by addon
  if (time == 0)
  {
    CDateTime::GetUTCDateTime().GetAsTime(time);
  }
  return time;
}

time_t CPVRClient::GetBufferTimeStart(void) const
{
  time_t time = 0;
  if (IsPlaying())
  {
    try
    {
      time = m_struct.GetBufferTimeStart(m_addonInstance);
    }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }
  return time;
}

time_t CPVRClient::GetBufferTimeEnd(void) const
{
  time_t time = 0;
  if (IsPlaying())
  {
    try
    {
      time = m_struct.GetBufferTimeEnd(m_addonInstance);
    }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }
  return time;
}

bool CPVRClient::CanAutoconfigure(void) const
{
  /** can only auto-configure when avahi details are provided in addon.xml */
  return !m_strAvahiType.empty() &&
      !m_strAvahiIpSetting.empty() &&
      !m_strAvahiPortSetting.empty();
}

bool CPVRClient::AutoconfigureRegisterType(void)
{
  if (!m_strAvahiType.empty())
  {
    // AddServiceType() returns false when already registered
    m_bAvahiServiceAdded |= CZeroconfBrowser::GetInstance()->AddServiceType(m_strAvahiType);
    return true;
  }

  return false;
}

bool CPVRClient::Autoconfigure(void)
{
  bool bReturn(false);

  if (!CanAutoconfigure())
    return bReturn;

  std::string strHostPort;
  std::vector<CZeroconfBrowser::ZeroconfService> found_services = CZeroconfBrowser::GetInstance()->GetFoundServices();
  for(std::vector<CZeroconfBrowser::ZeroconfService>::iterator it = found_services.begin(); !bReturn && it != found_services.end(); ++it)
  {
    /** found the type that we are looking for */
    if ((*it).GetType() == m_strAvahiType && std::find(m_rejectedAvahiHosts.begin(), m_rejectedAvahiHosts.end(), *it) == m_rejectedAvahiHosts.end())
    {
      /** try to resolve */
      if(!CZeroconfBrowser::GetInstance()->ResolveService((*it)))
      {
        CLog::Log(LOGWARNING, "%s - %s service found but the host name couldn't be resolved", __FUNCTION__, (*it).GetName().c_str());
      }
      else
      {
        // %s service found at %s
        std::string strLogLine(StringUtils::Format(g_localizeStrings.Get(19689).c_str(), (*it).GetName().c_str(), (*it).GetIP().c_str()));
        CLog::Log(LOGDEBUG, "%s - %s", __FUNCTION__, strLogLine.c_str());

        if (DialogResponse::YES != 
          HELPERS::ShowYesNoDialogLines(CVariant{19688}, // Scanning for PVR services
                                        CVariant{strLogLine},
                                        CVariant{19690})) // Do you want to use this service?
        {
          CLog::Log(LOGDEBUG, "%s - %s service found but not enabled by the user", __FUNCTION__, (*it).GetName().c_str());
          m_rejectedAvahiHosts.push_back(*it);
        }
        else
        {
          /** update the settings and return */
          std::string strPort(StringUtils::Format("%d", (*it).GetPort()));
          UpdateSetting(m_strAvahiIpSetting, (*it).GetIP());
          UpdateSetting(m_strAvahiPortSetting, strPort);
          SaveSettings();
          CLog::Log(LOGNOTICE, "%s - auto-configured %s using host '%s' and port '%d'", __FUNCTION__, (*it).GetName().c_str(), (*it).GetIP().c_str(), (*it).GetPort());

          bReturn = true;
        }
      }
    }
  }

  return bReturn;
}

bool CPVRClient::IsRealTimeStream(void) const
{
  bool bReturn(false);
  if (IsPlaying())
  {
    try
    {
      bReturn = m_struct.IsRealTimeStream(m_addonInstance);
    }
    catch (std::exception &e) { LogException(e, __FUNCTION__); }
  }
  return bReturn;
}

void CPVRClient::OnSystemSleep(void)
{
  if (!m_bReadyToUse)
    return;

  try
  {
    m_struct.OnSystemSleep(m_addonInstance);
  }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }
}

void CPVRClient::OnSystemWake(void)
{
  if (!m_bReadyToUse)
    return;

  try
  {
    m_struct.OnSystemWake(m_addonInstance);
  }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }
}

void CPVRClient::OnPowerSavingActivated(void)
{
  if (!m_bReadyToUse)
    return;

  try
  {
    m_struct.OnPowerSavingActivated(m_addonInstance);
  }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }
}

void CPVRClient::OnPowerSavingDeactivated(void)
{
  if (!m_bReadyToUse)
    return;

  try
  {
    m_struct.OnPowerSavingDeactivated(m_addonInstance);
  }
  catch (std::exception &e) { LogException(e, __FUNCTION__); }
}
