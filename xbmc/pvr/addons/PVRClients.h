/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/AddonManager.h"
#include "addons/kodi-dev-kit/include/kodi/c-api/addon-instance/pvr/pvr_general.h"
#include "threads/CriticalSection.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ADDON
{
  struct AddonEvent;
}

namespace PVR
{
  class CPVRChannelGroupInternal;
  class CPVRChannelGroup;
  class CPVRChannelGroups;
  class CPVRProvidersContainer;
  class CPVRClient;
  class CPVREpg;
  class CPVRRecordings;
  class CPVRTimerType;
  class CPVRTimersContainer;

  typedef std::map<int, std::shared_ptr<CPVRClient>> CPVRClientMap;

  /**
   * Holds generic data about a backend (number of channels etc.)
   */
  struct SBackend
  {
    std::string name;
    std::string version;
    std::string host;
    int numTimers = 0;
    int numRecordings = 0;
    int numDeletedRecordings = 0;
    int numProviders = 0;
    int numChannelGroups = 0;
    int numChannels = 0;
    uint64_t diskUsed = 0;
    uint64_t diskTotal = 0;
  };

  class CPVRClients : public ADDON::IAddonMgrCallback
  {
  public:
    CPVRClients();
    ~CPVRClients() override;

    /*!
     * @brief Start all clients.
     */
    void Start();

    /*!
     * @brief Stop all clients.
     */
    void Stop();

    /*!
     * @brief Continue all clients.
     */
    void Continue();

    /*!
     * @brief Update add-ons from the AddonManager
     * @param changedAddonId The id of the changed addon, empty string denotes 'any addon'.
     */
    void UpdateAddons(const std::string& changedAddonId = "");

    /*!
     * @brief Restart a single client add-on.
     * @param id The add-on to restart.
     * @param bDataChanged True if the client's data changed, false otherwise (unused).
     * @return True if the client was found and restarted, false otherwise.
     */
    bool RequestRestart(const std::string& id, bool bDataChanged) override;

    /*!
     * @brief Stop a client.
     * @param id The client to stop.
     * @param bRestart If true, restart the client.
     * @return True if the client was found, false otherwise.
     */
    bool StopClient(const std::string& id, bool bRestart);

    /*!
     * @brief Handle addon events (enable, disable, ...).
     * @param event The addon event.
     */
    void OnAddonEvent(const ADDON::AddonEvent& event);

    /*!
     * @brief Get a client's numeric ID given its string ID.
     * @param strId The string ID.
     * @return The numeric ID matching the given string ID, -1 on error.
     */
    int GetClientId(const std::string& strId) const;

    /*!
     * @brief Get the number of created clients.
     * @return The amount of created clients.
     */
    int CreatedClientAmount() const;

    /*!
     * @brief Check whether there are any created clients.
     * @return True if at least one client is created.
     */
    bool HasCreatedClients() const;

    /*!
     * @brief Check whether a given client ID points to a created client.
     * @param iClientId The client ID.
     * @return True if the the client ID represents a created client, false otherwise.
     */
    bool IsCreatedClient(int iClientId) const;

    /*!
     * @brief Get the instance of the client, if it's created.
     * @param iClientId The ID of the client to get.
     * @param addon Will be filled with requested client on success, null otherwise.
     * @return True on success, false otherwise.
     */
    bool GetCreatedClient(int iClientId, std::shared_ptr<CPVRClient>& addon) const;

    /*!
     * @brief Get info required for providers. Include both enabled and disabled PVR add-ons
     * @return A list containing the information required to create client providers.
     */
    std::vector<CVariant> GetClientProviderInfos() const;

    /*!
     * @brief Get all created clients.
     * @param clients All created clients will be added to this map.
     * @return The amount of clients added to the map.
     */
    int GetCreatedClients(CPVRClientMap& clients) const;

    /*!
     * @brief Get the ID of the first created client.
     * @return the ID or -1 if no clients are created;
     */
    int GetFirstCreatedClientID();

    /*!
     * @brief Check whether there are any created, but not (yet) connected clients.
     * @return True if at least one client is ignored.
     */
    bool HasIgnoredClients() const;

    /*!
     * @brief Get the number of enabled clients.
     * @return The amount of enabled clients.
     */
    int EnabledClientAmount() const;

    /*!
     * @brief Check whether a given client ID points to an enabled client.
     * @param clientId The client ID.
     * @return True if the the client ID represents an enabled client, false otherwise.
     */
    bool IsEnabledClient(int clientId) const;

    /*!
     * @brief Get a list of the enabled client infos.
     * @return A list of enabled client infos.
     */
    std::vector<CVariant> GetEnabledClientInfos() const;

    //@}

    /*! @name general methods */
    //@{

    /*!
     * @brief Returns properties about all created clients
     * @return the properties
     */
    std::vector<SBackend> GetBackendProperties() const;

    //@}

    /*! @name Timer methods */
    //@{

    /*!
     * @brief Get all timers from the given clients
     * @param clients The clients to fetch data from. Leave empty to fetch data from all created clients.
     * @param timers Store the timers in this container.
     * @param failedClients in case of errors will contain the ids of the clients for which the timers could not be obtained.
     * @return true on success for all clients, false in case of error for at least one client.
     */
    bool GetTimers(const std::vector<std::shared_ptr<CPVRClient>>& clients,
                   CPVRTimersContainer* timers,
                   std::vector<int>& failedClients);

    /*!
     * @brief Get all supported timer types.
     * @param results The container to store the result in.
     * @return PVR_ERROR_NO_ERROR if the operation succeeded, the respective PVR_ERROR value otherwise.
     */
    PVR_ERROR GetTimerTypes(std::vector<std::shared_ptr<CPVRTimerType>>& results) const;

    //@}

    /*! @name Recording methods */
    //@{

    /*!
     * @brief Get all recordings from the given clients
     * @param clients The clients to fetch data from. Leave empty to fetch data from all created clients.
     * @param recordings Store the recordings in this container.
     * @param deleted If true, return deleted recordings, return not deleted recordings otherwise.
     * @param failedClients in case of errors will contain the ids of the clients for which the recordings could not be obtained.
     * @return PVR_ERROR_NO_ERROR if the operation succeeded, the respective PVR_ERROR value otherwise.
     */
    PVR_ERROR GetRecordings(const std::vector<std::shared_ptr<CPVRClient>>& clients,
                            CPVRRecordings* recordings,
                            bool deleted,
                            std::vector<int>& failedClients);

    /*!
     * @brief Delete all "soft" deleted recordings permanently on the backend.
     * @return PVR_ERROR_NO_ERROR if the operation succeeded, the respective PVR_ERROR value otherwise.
     */
    PVR_ERROR DeleteAllRecordingsFromTrash();

    //@}

    /*! @name EPG methods */
    //@{

    /*!
     * @brief Tell all clients the past time frame to use when notifying epg events back to Kodi.
     *
     * The clients might push epg events asynchronously to Kodi using the callback function
     * EpgEventStateChange. To be able to only push events that are actually of interest for Kodi,
     * clients need to know about the future epg time frame Kodi uses.
     *
     * @param[in] iPastDays number of days before "now".
     *                        @ref EPG_TIMEFRAME_UNLIMITED means that Kodi is interested in all
     *                        epg events, regardless of event times.
     * @return @ref PVR_ERROR_NO_ERROR if the operation succeeded, the respective @ref PVR_ERROR
     *         value otherwise.
     */
    PVR_ERROR SetEPGMaxPastDays(int iPastDays);

    /*!
     * @brief Tell all clients the future time frame to use when notifying epg events back to Kodi.
     *
     * The clients might push epg events asynchronously to Kodi using the callback function
     * EpgEventStateChange. To be able to only push events that are actually of interest for Kodi,
     * clients need to know about the future epg time frame Kodi uses.
     *
     * @param[in] iFutureDays number of days from "now".
     *                        @ref EPG_TIMEFRAME_UNLIMITED means that Kodi is interested in all
     *                        epg events, regardless of event times.
     * @return @ref PVR_ERROR_NO_ERROR if the operation succeeded, the respective @ref PVR_ERROR
     *         value otherwise.
     */
    PVR_ERROR SetEPGMaxFutureDays(int iFutureDays);

    //@}

    /*! @name Channel methods */
    //@{

    /*!
     * @brief Get all channels from the given clients.
     * @param clients The clients to fetch data from. Leave empty to fetch data from all created clients.
     * @param bRadio Whether to fetch radio or TV channels.
     * @param channels The container to store the channels.
     * @param failedClients in case of errors will contain the ids of the clients for which the channels could not be obtained.
     * @return PVR_ERROR_NO_ERROR if the channels were fetched successfully, last error otherwise.
     */
    PVR_ERROR GetChannels(const std::vector<std::shared_ptr<CPVRClient>>& clients,
                          bool bRadio,
                          std::vector<std::shared_ptr<CPVRChannel>>& channels,
                          std::vector<int>& failedClients);

    /*!
     * @brief Get all providers from backends.
     * @param clients The clients to fetch data from. Leave empty to fetch data from all created clients.
     * @param group The container to store the providers in.
     * @param failedClients in case of errors will contain the ids of the clients for which the providers could not be obtained.
     * @return PVR_ERROR_NO_ERROR if the providers were fetched successfully, last error otherwise.
     */
    PVR_ERROR GetProviders(const std::vector<std::shared_ptr<CPVRClient>>& clients,
                           CPVRProvidersContainer* providers,
                           std::vector<int>& failedClients);

    /*!
     * @brief Get all channel groups from the given clients.
     * @param clients The clients to fetch data from. Leave empty to fetch data from all created clients.
     * @param groups Store the channel groups in this container.
     * @param failedClients in case of errors will contain the ids of the clients for which the channel groups could not be obtained.
     * @return PVR_ERROR_NO_ERROR if the channel groups were fetched successfully, last error otherwise.
     */
    PVR_ERROR GetChannelGroups(const std::vector<std::shared_ptr<CPVRClient>>& clients,
                               CPVRChannelGroups* groups,
                               std::vector<int>& failedClients);

    /*!
     * @brief Get all group members of a channel group from the given clients.
     * @param clients The clients to fetch data from. Leave empty to fetch data from all created clients.
     * @param group The group to get the member for.
     * @param groupMembers The container for the group members.
     * @param failedClients in case of errors will contain the ids of the clients for which the channel group members could not be obtained.
     * @return PVR_ERROR_NO_ERROR if the channel group members were fetched successfully, last error otherwise.
     */
    PVR_ERROR GetChannelGroupMembers(
        const std::vector<std::shared_ptr<CPVRClient>>& clients,
        CPVRChannelGroup* group,
        std::vector<std::shared_ptr<CPVRChannelGroupMember>>& groupMembers,
        std::vector<int>& failedClients);

    /*!
     * @brief Get a list of clients providing a channel scan dialog.
     * @return All clients supporting channel scan.
     */
    std::vector<std::shared_ptr<CPVRClient>> GetClientsSupportingChannelScan() const;

    /*!
     * @brief Get a list of clients providing a channel settings dialog.
     * @return All clients supporting channel settings.
     */
    std::vector<std::shared_ptr<CPVRClient>> GetClientsSupportingChannelSettings(bool bRadio) const;

    /*!
     * @brief Get whether or not any client supports recording size.
     * @return True if any client supports recording size.
     */
    bool AnyClientSupportingRecordingsSize() const;

    /*!
     * @brief Get whether or not any client supports EPG.
     * @return True if any client supports EPG.
     */
    bool AnyClientSupportingEPG() const;

    /*!
     * @brief Get whether or not any client supports recordings.
     * @return True if any client supports recordings.
     */
    bool AnyClientSupportingRecordings() const;
    //@}

    /*!
     * @brief Get whether or not any client supports recordings delete.
     * @return True if any client supports recordings delete.
     */
    bool AnyClientSupportingRecordingsDelete() const;
    //@}

    /*! @name Power management methods */
    //@{

    /*!
     * @brief Propagate "system sleep" event to clients
     */
    void OnSystemSleep();

    /*!
     * @brief Propagate "system wakeup" event to clients
     */
    void OnSystemWake();

    /*!
     * @brief Propagate "power saving activated" event to clients
     */
    void OnPowerSavingActivated();

    /*!
     * @brief Propagate "power saving deactivated" event to clients
     */
    void OnPowerSavingDeactivated();

    //@}

    /*!
     * @brief Notify a change of an addon connection state.
     * @param client The changed client.
     * @param strConnectionString A human-readable string identifiying the addon.
     * @param newState The new connection state.
     * @param strMessage A human readable message providing additional information.
     */
    void ConnectionStateChange(CPVRClient* client,
                               const std::string& strConnectionString,
                               PVR_CONNECTION_STATE newState,
                               const std::string& strMessage);

  private:
    /*!
     * @brief Get the client instance for a given client id.
     * @param iClientId The id of the client to get.
     * @param addon The client.
     * @return True if the client was found, false otherwise.
     */
    bool GetClient(int iClientId, std::shared_ptr<CPVRClient>& addon) const;

    /*!
     * @brief Check whether a client is known.
     * @param id The addon id to check.
     * @return True if this client is known, false otherwise.
     */
    bool IsKnownClient(const std::string& id) const;

    /*!
     * @brief Check whether an given addon instance is a created pvr client.
     * @param id The addon id.
     * @return True if the the addon represents a created client, false otherwise.
     */
    bool IsCreatedClient(const std::string& id) const;

    /*!
     * @brief Get all created clients and clients not (yet) ready to use.
     * @param clientsReady Store the created clients in this map.
     * @param clientsNotReady Store the the ids of the not (yet) ready clients in this list.
     * @return PVR_ERROR_NO_ERROR in case all clients are ready, PVR_ERROR_SERVER_ERROR otherwise.
     */
    PVR_ERROR GetCallableClients(CPVRClientMap& clientsReady,
                                 std::vector<int>& clientsNotReady) const;

    typedef std::function<PVR_ERROR(const std::shared_ptr<CPVRClient>&)> PVRClientFunction;

    /*!
     * @brief Wraps calls to the given clients in order to do common pre and post function invocation actions.
     * @param strFunctionName The function name, for logging purposes.
     * @param clients The clients to wrap.
     * @param function The function to wrap. It has to have return type PVR_ERROR and must take a const reference to a std::shared_ptr<CPVRClient> as parameter.
     * @param failedClients Contains a list of the ids of clients for that the call failed, if any.
     * @return PVR_ERROR_NO_ERROR on success, any other PVR_ERROR_* value otherwise.
     */
    PVR_ERROR ForClients(const char* strFunctionName,
                         const std::vector<std::shared_ptr<CPVRClient>>& clients,
                         const PVRClientFunction& function,
                         std::vector<int>& failedClients) const;

    /*!
     * @brief Wraps calls to all created clients in order to do common pre and post function invocation actions.
     * @param strFunctionName The function name, for logging purposes.
     * @param function The function to wrap. It has to have return type PVR_ERROR and must take a const reference to a std::shared_ptr<CPVRClient> as parameter.
     * @return PVR_ERROR_NO_ERROR on success, any other PVR_ERROR_* value otherwise.
     */
    PVR_ERROR ForCreatedClients(const char* strFunctionName,
                                const PVRClientFunction& function) const;

    /*!
     * @brief Wraps calls to all created clients in order to do common pre and post function invocation actions.
     * @param strFunctionName The function name, for logging purposes.
     * @param function The function to wrap. It has to have return type PVR_ERROR and must take a const reference to a std::shared_ptr<CPVRClient> as parameter.
     * @param failedClients Contains a list of the ids of clients for that the call failed, if any.
     * @return PVR_ERROR_NO_ERROR on success, any other PVR_ERROR_* value otherwise.
     */
    PVR_ERROR ForCreatedClients(const char* strFunctionName,
                                const PVRClientFunction& function,
                                std::vector<int>& failedClients) const;

    mutable CCriticalSection m_critSection;
    CPVRClientMap m_clientMap;
  };
}
