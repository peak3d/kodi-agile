#pragma once
/*
 *      Copyright (C) 2012-2013 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <memory>
#include <string>
#include <vector>

#include "addons/Addon.h"
#include "addons/AddonDll.h"
#include "network/ZeroconfBrowser.h"

#include "pvr/channels/PVRChannel.h"

namespace EPG
{
  class CEpg;
}

namespace PVR
{
  class CPVRChannelGroup;
  class CPVRChannelGroupInternal;
  class CPVRChannelGroups;
  class CPVRTimers;
  class CPVRTimerInfoTag;
  class CPVRRecordings;
  class CPVREpgContainer;
  class CPVRClient;
  class CPVRTimerType;

  struct EpgEventStateChange;

  typedef std::vector<PVR_MENUHOOK> PVR_MENUHOOKS;
  typedef std::shared_ptr<CPVRClient> PVR_CLIENT;
  #define PVR_INVALID_CLIENT_ID (-2)

  typedef std::shared_ptr<CPVRTimerType> CPVRTimerTypePtr;
  typedef std::vector<CPVRTimerTypePtr>  CPVRTimerTypes;

  /*!
   * Interface from XBMC to a PVR add-on.
   *
   * Also translates XBMC's C++ structures to the addon's C structures.
   */
  class CPVRClient : public ADDON::CAddonDll
  {
  public:
    static std::unique_ptr<CPVRClient> FromExtension(ADDON::AddonProps props, const cp_extension_t* ext);

    explicit CPVRClient(ADDON::AddonProps props);
    CPVRClient(ADDON::AddonProps props, const std::string& strAvahiType,
        const std::string& strAvahiIpSetting, const std::string& strAvahiPortSetting);

    ~CPVRClient(void);

    virtual void OnDisabled() override;
    virtual void OnEnabled() override;
    virtual void OnPostInstall(bool update, bool modal) override;
    virtual void OnPreUnInstall() override;
    virtual void OnPostUnInstall() override;
    ADDON::AddonPtr GetRunningInstance() const override;

    /** @name PVR add-on methods */
    //@{

    /*!
     * @brief Initialise the instance of this add-on.
     * @param iClientId The ID of this add-on.
     */
    ADDON_STATUS Create(int iClientId);

    /*!
     * @return True when the dll for this add-on was loaded, false otherwise (e.g. unresolved symbols)
     */
    bool DllLoaded(void) const;

    /*!
     * @brief Destroy the instance of this add-on.
     */
    void Destroy(void);

    /*!
     * @brief Destroy and recreate this add-on.
     */
    void ReCreate(void);

    /*!
     * @return True if this instance is initialised (ADDON_Create returned true), false otherwise.
     */
    bool ReadyToUse(void) const;

    /*!
     * @brief Gets the backend connection state.
     * @return the backend connection state.
     */
    PVR_CONNECTION_STATE GetConnectionState(void) const;

    /*!
     * @brief Sets the backend connection state.
     * @param state the new backend connection state.
     */
    void SetConnectionState(PVR_CONNECTION_STATE state);

    /*!
     * @brief Gets the backend's previous connection state.
     * @return the backend's previous  connection state.
     */
    PVR_CONNECTION_STATE GetPreviousConnectionState(void) const;

    /*!
     * @brief signal to PVRMananager this client should be ignored
     * @return true if this client should be ignored
     */
    bool IgnoreClient(void) const;

    /*!
     * @return The ID of this instance.
     */
    int GetID(void) const;

    //@}
    /** @name PVR server methods */
    //@{

    /*!
     * @brief Query this add-on's capabilities.
     * @return pCapabilities The add-on's capabilities.
     */
    PVR_ADDON_CAPABILITIES GetCapabilities(void) const;

    /*!
     * @brief Get the stream properties of the stream that's currently being read.
     * @param pProperties The properties.
     * @return PVR_ERROR_NO_ERROR if the properties have been fetched successfully.
     */
    PVR_ERROR GetStreamProperties(PVR_STREAM_PROPERTIES *pProperties);

    /*!
     * @return The name reported by the backend.
     */
    const std::string& GetBackendName(void) const;

    /*!
     * @return The version string reported by the backend.
     */
    const std::string& GetBackendVersion(void) const;

    /*!
     * @brief the ip address or alias of the pvr backend server
     */
    const std::string& GetBackendHostname(void) const;

    /*!
     * @return The connection string reported by the backend.
     */
    const std::string& GetConnectionString(void) const;

    /*!
     * @return A friendly name for this add-on that can be used in log messages.
     */
    const std::string& GetFriendlyName(void) const;

    /*!
     * @brief Get the disk space reported by the server.
     * @param iTotal The total disk space.
     * @param iUsed The used disk space.
     * @return PVR_ERROR_NO_ERROR if the drive space has been fetched successfully.
     */
    PVR_ERROR GetDriveSpace(long long *iTotal, long long *iUsed);

    /*!
     * @brief Start a channel scan on the server.
     * @return PVR_ERROR_NO_ERROR if the channel scan has been started successfully.
     */
    PVR_ERROR StartChannelScan(void);

    /*!
     * @brief Request the client to open dialog about given channel to add
     * @param channel The channel to add
     * @return PVR_ERROR_NO_ERROR if the add has been fetched successfully.
     */
    PVR_ERROR OpenDialogChannelAdd(const CPVRChannelPtr &channel);

    /*!
     * @brief Request the client to open dialog about given channel settings
     * @param channel The channel to edit
     * @return PVR_ERROR_NO_ERROR if the edit has been fetched successfully.
     */
    PVR_ERROR OpenDialogChannelSettings(const CPVRChannelPtr &channel);

    /*!
     * @brief Request the client to delete given channel
     * @param channel The channel to delete
     * @return PVR_ERROR_NO_ERROR if the delete has been fetched successfully.
     */
    PVR_ERROR DeleteChannel(const CPVRChannelPtr &channel);

    /*!
     * @brief Request the client to rename given channel
     * @param channel The channel to rename
     * @return PVR_ERROR_NO_ERROR if the rename has been fetched successfully.
     */
    PVR_ERROR RenameChannel(const CPVRChannelPtr &channel);

    /*!
     * @return True if this add-on has menu hooks, false otherwise.
     */
    bool HaveMenuHooks(PVR_MENUHOOK_CAT cat) const;

    /*!
     * @return The menu hooks for this add-on.
     */
    PVR_MENUHOOKS *GetMenuHooks(void);

    /*!
     * @brief Call one of the menu hooks of this client.
     * @param hook The hook to call.
     * @param item The selected file item for which the hook was called.
     */
    void CallMenuHook(const PVR_MENUHOOK &hook, const CFileItem *item);

    //@}
    /** @name PVR EPG methods */
    //@{

    /*!
     * @brief Request an EPG table for a channel from the client.
     * @param channel The channel to get the EPG table for.
     * @param epg The table to write the data to.
     * @param start The start time to use.
     * @param end The end time to use.
     * @param bSaveInDb If true, tell the callback method to save any new entry in the database or not. see CAddonCallbacksPVR::PVRTransferEpgEntry()
     * @return PVR_ERROR_NO_ERROR if the table has been fetched successfully.
     */
    PVR_ERROR GetEPGForChannel(const CPVRChannelPtr &channel, EPG::CEpg *epg, time_t start = 0, time_t end = 0, bool bSaveInDb = false);

    /*!
     * Tell the client the time frame to use when notifying epg events back to Kodi. The client might push epg events asynchronously
     * to Kodi using the callback function EpgEventStateChange. To be able to only push events that are actually of interest for Kodi,
     * client needs to know about the epg time frame Kodi uses.
     * @param iDays number of days from "now". EPG_TIMEFRAME_UNLIMITED means that Kodi is interested in all epg events, regardless of event times.
     * @return PVR_ERROR_NO_ERROR if new value was successfully set.
     */
    PVR_ERROR SetEPGTimeFrame(int iDays);

    //@}
    /** @name PVR channel group methods */
    //@{

    /*!
      * @return The total amount of channel groups on the server or -1 on error.
      */
    int GetChannelGroupsAmount(void);

    /*!
     * @brief Request the list of all channel groups from the backend.
     * @param groups The groups container to get the groups for.
     * @return PVR_ERROR_NO_ERROR if the list has been fetched successfully.
     */
    PVR_ERROR GetChannelGroups(CPVRChannelGroups *groups);

    /*!
     * @brief Request the list of all group members from the backend.
     * @param groups The group to get the members for.
     * @return PVR_ERROR_NO_ERROR if the list has been fetched successfully.
     */
    PVR_ERROR GetChannelGroupMembers(CPVRChannelGroup *group);

    //@}
    /** @name PVR channel methods */
    //@{

    /*!
     * @return The total amount of channels on the server or -1 on error.
     */
    int GetChannelsAmount(void);

    /*!
     * @brief Request the list of all channels from the backend.
     * @param channels The channel group to add the channels to.
     * @param bRadio True to get the radio channels, false to get the TV channels.
     * @return PVR_ERROR_NO_ERROR if the list has been fetched successfully.
     */
    PVR_ERROR GetChannels(CPVRChannelGroup &channels, bool bRadio);

    //@}
    /** @name PVR recording methods */
    //@{

    /*!
     * @param deleted if set return deleted recording
     * @return The total amount of recordingd on the server or -1 on error.
     */
    int GetRecordingsAmount(bool deleted);

    /*!
     * @brief Request the list of all recordings from the backend.
     * @param results The container to add the recordings to.
     * @param deleted if set return deleted recording
     * @return PVR_ERROR_NO_ERROR if the list has been fetched successfully.
     */
    PVR_ERROR GetRecordings(CPVRRecordings *results, bool deleted);

    /*!
     * @brief Delete a recording on the backend.
     * @param recording The recording to delete.
     * @return PVR_ERROR_NO_ERROR if the recording has been deleted successfully.
     */
    PVR_ERROR DeleteRecording(const CPVRRecording &recording);

    /*!
     * @brief Undelete a recording on the backend.
     * @param recording The recording to undelete.
     * @return PVR_ERROR_NO_ERROR if the recording has been undeleted successfully.
     */
    PVR_ERROR UndeleteRecording(const CPVRRecording &recording);

    /*!
     * @brief Delete all recordings permanent which in the deleted folder on the backend.
     * @return PVR_ERROR_NO_ERROR if the recordings has been deleted successfully.
     */
    PVR_ERROR DeleteAllRecordingsFromTrash();

    /*!
     * @brief Rename a recording on the backend.
     * @param recording The recording to rename.
     * @return PVR_ERROR_NO_ERROR if the recording has been renamed successfully.
     */
    PVR_ERROR RenameRecording(const CPVRRecording &recording);

    /*!
     * @brief Set the play count of a recording on the backend.
     * @param recording The recording to set the play count.
     * @param count Play count.
     * @return PVR_ERROR_NO_ERROR if the recording's play count has been set successfully.
     */
    PVR_ERROR SetRecordingPlayCount(const CPVRRecording &recording, int count);

    /*!
    * @brief Set the last watched position of a recording on the backend.
    * @param recording The recording.
    * @param position The last watched position in seconds
    * @return PVR_ERROR_NO_ERROR if the position has been stored successfully.
    */
    PVR_ERROR SetRecordingLastPlayedPosition(const CPVRRecording &recording, int lastplayedposition);

    /*!
    * @brief Retrieve the last watched position of a recording on the backend.
    * @param recording The recording.
    * @return The last watched position in seconds or -1 on error
    */
    int GetRecordingLastPlayedPosition(const CPVRRecording &recording);

    /*!
    * @brief Retrieve the edit decision list (EDL) from the backend.
    * @param recording The recording.
    * @return The edit decision list (empty on error).
    */
    std::vector<PVR_EDL_ENTRY> GetRecordingEdl(const CPVRRecording &recording);

    //@}
    /** @name PVR timer methods */
    //@{

    /*!
     * @return The total amount of timers on the backend or -1 on error.
     */
    int GetTimersAmount(void);

    /*!
     * @brief Request the list of all timers from the backend.
     * @param results The container to store the result in.
     * @return PVR_ERROR_NO_ERROR if the list has been fetched successfully.
     */
    PVR_ERROR GetTimers(CPVRTimers *results);

    /*!
     * @brief Add a timer on the backend.
     * @param timer The timer to add.
     * @return PVR_ERROR_NO_ERROR if the timer has been added successfully.
     */
    PVR_ERROR AddTimer(const CPVRTimerInfoTag &timer);

    /*!
     * @brief Delete a timer on the backend.
     * @param timer The timer to delete.
     * @param bForce Set to true to delete a timer that is currently recording a program.
     * @return PVR_ERROR_NO_ERROR if the timer has been deleted successfully.
     */
    PVR_ERROR DeleteTimer(const CPVRTimerInfoTag &timer, bool bForce = false);

    /*!
     * @brief Rename a timer on the server.
     * @param timer The timer to rename.
     * @param strNewName The new name of the timer.
     * @return PVR_ERROR_NO_ERROR if the timer has been renamed successfully.
     */
    PVR_ERROR RenameTimer(const CPVRTimerInfoTag &timer, const std::string &strNewName);

    /*!
     * @brief Update the timer information on the server.
     * @param timer The timer to update.
     * @return PVR_ERROR_NO_ERROR if the timer has been updated successfully.
     */
    PVR_ERROR UpdateTimer(const CPVRTimerInfoTag &timer);

    /*!
     * @brief Get all timer types supported by the backend.
     * @param results The container to store the result in.
     * @return PVR_ERROR_NO_ERROR if the list has been fetched successfully.
     */
    PVR_ERROR GetTimerTypes(CPVRTimerTypes& results) const;

    //@}
    /** @name PVR live stream methods */
    //@{

    /*!
     * @brief Open a live stream on the server.
     * @param channel The channel to stream.
     * @param bIsSwitchingChannel True when switching channels, false otherwise.
     * @return True if the stream opened successfully, false otherwise.
     */
    bool OpenStream(const CPVRChannelPtr &channel, bool bIsSwitchingChannel);

    /*!
     * @brief Close an open live stream.
     */
    void CloseStream(void);

    /*!
     * @brief Read from an open live stream.
     * @param lpBuf The buffer to store the data in.
     * @param uiBufSize The amount of bytes to read.
     * @return The amount of bytes that were actually read from the stream.
     */
    int ReadStream(void* lpBuf, int64_t uiBufSize);

    /*!
     * @brief Seek in a live stream on a backend that supports timeshifting.
     * @param iFilePosition The position to seek to.
     * @param iWhence ?
     * @return The new position.
     */
    int64_t SeekStream(int64_t iFilePosition, int iWhence = SEEK_SET);

    /*!
     * @return The position in the stream that's currently being read.
     */
    int64_t GetStreamPosition(void);

    /*!
     * @return The total length of the stream that's currently being read.
     */
    int64_t GetStreamLength(void);

    /*!
     * @brief (Un)Pause a stream
     */
    void PauseStream(bool bPaused);

    /*!
     * @brief Switch to another channel. Only to be called when a live stream has already been opened.
     * @param channel The channel to switch to.
     * @return True if the switch was successful, false otherwise.
     */
    bool SwitchChannel(const CPVRChannelPtr &channel);

    /*!
     * @brief Get the signal quality of the stream that's currently open.
     * @param qualityinfo The signal quality.
     * @return True if the signal quality has been read successfully, false otherwise.
     */
    bool SignalQuality(PVR_SIGNAL_STATUS &qualityinfo);

    /*!
     * @brief Get the stream URL for a channel from the server. Used by the MediaPortal add-on.
     * @param channel The channel to get the stream URL for.
     * @return The requested URL.
     */
    std::string GetLiveStreamURL(const CPVRChannelPtr &channel);

    /*!
     * @brief Check whether PVR backend supports pausing the currently playing stream
     */
    bool CanPauseStream(void);

    /*!
     * @brief Check whether PVR backend supports seeking for the currently playing stream
     */
    bool CanSeekStream(void);

    /*!
     * Notify the pvr addon/demuxer that XBMC wishes to seek the stream by time
     * @param time The absolute time since stream start
     * @param backwards True to seek to keyframe BEFORE time, else AFTER
     * @param startpts can be updated to point to where display should start
     * @return True if the seek operation was possible
     * @remarks Optional, and only used if addon has its own demuxer. Return False if this add-on won't provide this function.
     */
    bool SeekTime(int time, bool backwards, double *startpts);

    /*!
     * Notify the pvr addon/demuxer that XBMC wishes to change playback speed
     * @param speed The requested playback speed
     * @remarks Optional, and only used if addon has its own demuxer.
     */
    void SetSpeed(int speed);

    //@}
    /** @name PVR recording stream methods */
    //@{

    /*!
     * @brief Open a recording on the server.
     * @param recording The recording to open.
     * @return True if the stream has been opened succesfully, false otherwise.
     */
    bool OpenStream(const CPVRRecordingPtr &recording);

    //@}
    /** @name PVR demultiplexer methods */
    //@{

    /*!
     * @brief Reset the demultiplexer in the add-on.
     */
    void DemuxReset(void);

    /*!
     * @brief Abort the demultiplexer thread in the add-on.
     */
    void DemuxAbort(void);

    /*!
     * @brief Flush all data that's currently in the demultiplexer buffer in the add-on.
     */
    void DemuxFlush(void);

    /*!
     * @brief Read a packet from the demultiplexer.
     * @return The packet.
     */
    DemuxPacket *DemuxRead(void);

    //@}

    bool SupportsChannelGroups(void) const;
    bool SupportsChannelScan(void) const;
    bool SupportsChannelSettings(void) const;
    bool SupportsEPG(void) const;
    bool SupportsLastPlayedPosition(void) const;
    bool SupportsRadio(void) const;
    bool SupportsRecordings(void) const;
    bool SupportsRecordingsUndelete(void) const;
    bool SupportsRecordingPlayCount(void) const;
    bool SupportsRecordingEdl(void) const;
    bool SupportsTimers(void) const;
    bool SupportsTV(void) const;
    bool HandlesDemuxing(void) const;
    bool HandlesInputStream(void) const;

    bool IsPlayingLiveStream(void) const;
    bool IsPlayingLiveTV(void) const;
    bool IsPlayingLiveRadio(void) const;
    bool IsPlayingEncryptedChannel(void) const;
    bool IsPlayingRecording(void) const;
    bool IsPlaying(void) const;
    CPVRRecordingPtr GetPlayingRecording(void) const;
    CPVRChannelPtr GetPlayingChannel() const;

    static const char *ToString(const PVR_ERROR error);

    /*!
     * @brief is timeshift active?
     */
    bool IsTimeshifting();

    /*!
     * @brief actual playing time
     */
    time_t GetPlayingTime();

    /*!
     * @brief time of oldest packet in timeshift buffer
     */
    time_t GetBufferTimeStart();

    /*!
     * @brief time of latest packet in timeshift buffer
     */
    time_t GetBufferTimeEnd();

    /*!
     * @return True if this add-on can be auto-configured via avahi, false otherwise
     */
    bool CanAutoconfigure(void) const;

    /*!
     * Registers the avahi type for this add-on
     * @return True if registered, false if not.
     */
    bool AutoconfigureRegisterType(void);

    /*!
     * Try to auto-configure this add-on via avahi
     * @return True if auto-configured and the configured was accepted by the user, false otherwise
     */
    bool Autoconfigure(void);

    /*!
     * @brief is real-time stream?
     */
    bool IsRealTimeStream();

    /*!
     * @brief reads the client's properties
     */
    bool GetAddonProperties(void);

    /*!
     * @brief Propagate power management events to this add-on
     */
    void OnSystemSleep();
    void OnSystemWake();
    void OnPowerSavingActivated();
    void OnPowerSavingDeactivated();

    /*!
    * @brief Transfer a channel group from the add-on to XBMC. The group will be created if it doesn't exist.
    * @param addonData A pointer to the add-on.
    * @param handle The handle parameter that XBMC used when requesting the channel groups list
    * @param entry The entry to transfer to XBMC
    */
    static void PVRTransferChannelGroup(void* kodiInstance, const ADDON_HANDLE handle, const PVR_CHANNEL_GROUP* entry);

    /*!
    * @brief Transfer a channel group member entry from the add-on to XBMC. The channel will be added to the group if the group can be found.
    * @param addonData A pointer to the add-on.
    * @param handle The handle parameter that XBMC used when requesting the channel group members list
    * @param entry The entry to transfer to XBMC
    */
    static void PVRTransferChannelGroupMember(void* kodiInstance, const ADDON_HANDLE handle, const PVR_CHANNEL_GROUP_MEMBER* entry);

    /*!
    * @brief Transfer an EPG tag from the add-on to XBMC
    * @param addonData A pointer to the add-on.
    * @param handle The handle parameter that XBMC used when requesting the EPG data
    * @param entry The entry to transfer to XBMC
    */
    static void PVRTransferEpgEntry(void* kodiInstance, const ADDON_HANDLE handle, const EPG_TAG* entry);

    /*!
    * @brief Transfer a channel entry from the add-on to XBMC
    * @param addonData A pointer to the add-on.
    * @param handle The handle parameter that XBMC used when requesting the channel list
    * @param entry The entry to transfer to XBMC
    */
    static void PVRTransferChannelEntry(void* kodiInstance, const ADDON_HANDLE handle, const PVR_CHANNEL* entry);

    /*!
    * @brief Transfer a timer entry from the add-on to XBMC
    * @param addonData A pointer to the add-on.
    * @param handle The handle parameter that XBMC used when requesting the timers list
    * @param entry The entry to transfer to XBMC
    */
    static void PVRTransferTimerEntry(void* kodiInstance, const ADDON_HANDLE handle, const PVR_TIMER* entry);

    /*!
    * @brief Transfer a recording entry from the add-on to XBMC
    * @param addonData A pointer to the add-on.
    * @param handle The handle parameter that XBMC used when requesting the recordings list
    * @param entry The entry to transfer to XBMC
    */
    static void PVRTransferRecordingEntry(void* kodiInstance, const ADDON_HANDLE handle, const PVR_RECORDING* entry);

    /*!
    * @brief Add or replace a menu hook for the context menu for this add-on
    * @param addonData A pointer to the add-on.
    * @param hook The hook to add.
    */
    static void PVRAddMenuHook(void* kodiInstance, PVR_MENUHOOK* hook);

    /*!
    * @brief Display a notification in XBMC that a recording started or stopped on the server
    * @param addonData A pointer to the add-on.
    * @param strName The name of the recording to display
    * @param strFileName The filename of the recording
    * @param bOnOff True when recording started, false when it stopped
    */
    static void PVRRecording(void* kodiInstance, const char* strName, const char* strFileName, bool bOnOff);

    /*!
    * @brief Request XBMC to update it's list of channels
    * @param addonData A pointer to the add-on.
    */
    static void PVRTriggerChannelUpdate(void* kodiInstance);

    /*!
    * @brief Request XBMC to update it's list of timers
    * @param addonData A pointer to the add-on.
    */
    static void PVRTriggerTimerUpdate(void* kodiInstance);

    /*!
    * @brief Request XBMC to update it's list of recordings
    * @param addonData A pointer to the add-on.
    */
    static void PVRTriggerRecordingUpdate(void* kodiInstance);

    /*!
    * @brief Request XBMC to update it's list of channel groups
    * @param addonData A pointer to the add-on.
    */
    static void PVRTriggerChannelGroupsUpdate(void* kodiInstance);

    /*!
    * @brief Schedule an EPG update for the given channel channel
    * @param addonData A pointer to the add-on
    * @param iChannelUid The unique id of the channel for this add-on
    */
    static void PVRTriggerEpgUpdate(void* kodiInstance, unsigned int iChannelUid);

    /*!
    * @brief Free a packet that was allocated with AllocateDemuxPacket
    * @param addonData A pointer to the add-on.
    * @param pPacket The packet to free.
    */
    static void PVRFreeDemuxPacket(void* kodiInstance, DemuxPacket* pPacket);

    /*!
    * @brief Allocate a demux packet. Free with FreeDemuxPacket
    * @param addonData A pointer to the add-on.
    * @param iDataSize The size of the data that will go into the packet
    * @return The allocated packet.
    */
    static DemuxPacket* PVRAllocateDemuxPacket(void* kodiInstance, int iDataSize = 0);

    /*!
    * @brief Notify a state change for a PVR backend connection
    * @param addonData A pointer to the add-on.
    * @param strConnectionString The connection string reported by the backend that can be displayed in the UI.
    * @param newState The new state.
    * @param strMessage A localized addon-defined string representing the new state, that can be displayed
    *        in the UI or NULL if the Kodi-defined default string for the new state shall be displayed.
    */
    static void PVRConnectionStateChange(void* kodiInstance, const char* strConnectionString, PVR_CONNECTION_STATE newState, const char *strMessage);

    /*!
    * @brief Notify a state change for an EPG event
    * @param addonData A pointer to the add-on.
    * @param tag The EPG event.
    * @param iUniqueChannelId The unique id of the channel for the EPG event
    * @param newState The new state.
    * @param newState The new state. For EPG_EVENT_CREATED and EPG_EVENT_UPDATED, tag must be filled with all available
    *        event data, not just a delta. For EPG_EVENT_DELETED, it is sufficient to fill EPG_TAG.iUniqueBroadcastId
    */
    static void PVREpgEventStateChange(void* kodiInstance, EPG_TAG* tag, unsigned int iUniqueChannelId, EPG_EVENT_STATE newState);


  private:
    /*!
     * @brief Resets all class members to their defaults. Called by the constructors.
     */
    void ResetProperties(int iClientId = PVR_INVALID_CLIENT_ID);

    /*!
     * @brief Copy over group info from xbmcGroup to addonGroup.
     * @param xbmcGroup The group on XBMC's side.
     * @param addonGroup The group on the addon's side.
     */
    static void WriteClientGroupInfo(const CPVRChannelGroup &xbmcGroup, PVR_CHANNEL_GROUP &addonGroup);

    /*!
     * @brief Copy over recording info from xbmcRecording to addonRecording.
     * @param xbmcRecording The recording on XBMC's side.
     * @param addonRecording The recording on the addon's side.
     */
    static void WriteClientRecordingInfo(const CPVRRecording &xbmcRecording, PVR_RECORDING &addonRecording);

    /*!
     * @brief Copy over timer info from xbmcTimer to addonTimer.
     * @param xbmcTimer The timer on XBMC's side.
     * @param addonTimer The timer on the addon's side.
     */
    static void WriteClientTimerInfo(const CPVRTimerInfoTag &xbmcTimer, PVR_TIMER &addonTimer);

    /*!
     * @brief Copy over channel info from xbmcChannel to addonClient.
     * @param xbmcChannel The channel on XBMC's side.
     * @param addonChannel The channel on the addon's side.
     */
    static void WriteClientChannelInfo(const CPVRChannelPtr &xbmcChannel, PVR_CHANNEL &addonChannel);

    static void UpdateEpgEvent(const EpgEventStateChange &ch, bool bQueued);

    /*!
     * @brief Whether a channel can be played by this add-on
     * @param channel The channel to check.
     * @return True when it can be played, false otherwise.
     */
    bool CanPlayChannel(const CPVRChannelPtr &channel) const;

    bool LogError(const PVR_ERROR error, const char *strMethod) const;

    bool                   m_bReadyToUse;          /*!< true if this add-on is initialised (ADDON_Create returned true), false otherwise */
    PVR_CONNECTION_STATE   m_connectionState;      /*!< the backend connection state */
    PVR_CONNECTION_STATE   m_prevConnectionState;  /*!< the previous backend connection state */
    bool                   m_ignoreClient;         /*!< signals to PVRManager to ignore this client until it has been connected */
    PVR_MENUHOOKS          m_menuhooks;            /*!< the menu hooks for this add-on */
    CPVRTimerTypes         m_timertypes;           /*!< timer types supported by this backend */
    int                    m_iClientId;            /*!< database ID of the client */

    /* cached data */
    std::string            m_strBackendName;       /*!< the cached backend version */
    std::string            m_strBackendVersion;    /*!< the cached backend version */
    std::string            m_strConnectionString;  /*!< the cached connection string */
    std::string            m_strFriendlyName;      /*!< the cached friendly name */
    PVR_ADDON_CAPABILITIES m_addonCapabilities;     /*!< the cached add-on capabilities */
    std::string            m_strBackendHostname;    /*!< the cached backend hostname */

    /* stored strings to make sure const char* members in PVR_PROPERTIES stay valid */
    std::string                                    m_strUserPath;         /*!< @brief translated path to the user profile */
    std::string                                    m_strClientPath;       /*!< @brief translated path to this add-on */
    std::string                                    m_strAvahiType;        /*!< avahi service type */
    std::string                                    m_strAvahiIpSetting;   /*!< add-on setting name to change to the found ip address */
    std::string                                    m_strAvahiPortSetting; /*!< add-on setting name to change to the found port number */
    std::vector<CZeroconfBrowser::ZeroconfService> m_rejectedAvahiHosts;  /*!< hosts that were rejected by the user */

    CCriticalSection m_critSection;

    bool                m_bIsPlayingTV;
    CPVRChannelPtr      m_playingChannel;
    bool                m_bIsPlayingRecording;
    CPVRRecordingPtr    m_playingRecording;
    ADDON::AddonVersion m_apiVersion;
    bool                m_bAvahiServiceAdded;
    sFuncTable_PVRClient m_struct;

    /*!
    * @{
    * @brief Functions below are used to handle exception between Kodi and his
    * add-on.
    *
    * ExceptionStdHandle(...):
    *   Used for to handle standard c++ exception partly generated from dev kits
    *   headers.
    *
    * ExceptionErrHandle(...):
    *   This is a special type basically used also in dev kit headers to give a
    *   exception with his ADDON_STATUS as integer value.
    *   Can be generated from headers by a massive fault detected on call of
    *   them.
    *
    * ExceptionUnkHandle(...);
    *   Used for everything else.
    *
    * With a call of them becomes the standardized function
    * Exception::LogUnkException(...) from ExceptionHandling.h used to write
    * log entry. This is always the first call, to see if it still crashes the
    * source of them.
    *
    * After them becomes the PVR add-on Destroyed and complete disabled.
    *
    * As last step comes a dialog to inform the used about the Exception.
    *
    * How it is set on add-on calls
    * ~~~~~~~~~~~~~{.cpp}
    * try
    * {
    *   ...
    * }
    * catch (std::exception& ex) { ExceptionStdHandle(ex, __FUNCTION__); }
    * catch (int ex)             { ExceptionErrHandle(ex, __FUNCTION__); }
    * catch (...)                { ExceptionUnkHandle(__FUNCTION__); }
    * ~~~~~~~~~~~~~
    *
    * @note this way becomes also used on add-on typed e.g. adsp ...
    */
    void ExceptionStdHandle(std::exception& ex, const char* function);
    void ExceptionErrHandle(int ex, const char* function);
    void ExceptionUnkHandle(const char* function);
    /* @}*/

    void* m_addonInstance;
  };
}
