/*
 * Copyright 2014 CyberVision, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "kaa/logging/Log.hpp"
#include "kaa/logging/LoggingUtils.hpp"
#include "kaa/channel/SyncDataProcessor.hpp"

namespace kaa {

SyncDataProcessor::SyncDataProcessor(
                      IMetaDataTransportPtr       metaDataTransport
                    , IBootstrapTransportPtr      bootstrapTransport
                    , IProfileTransportPtr        profileTransport
                    , IConfigurationTransportPtr  configurationTransport
                    , INotificationTransportPtr   notificationTransport
                    , IUserTransportPtr           userTransport
                    , IEventTransportPtr          eventTransport
                    , ILoggingTransportPtr        loggingTransport
                    , IRedirectionTransportPtr    redirectionTransport
                    , IKaaClientStateStoragePtr   clientStatus)
        : metaDataTransport_(metaDataTransport)
        , bootstrapTransport_(bootstrapTransport)
        , profileTransport_(profileTransport)
        , configurationTransport_(configurationTransport)
        , notificationTransport_(notificationTransport)
        , userTransport_(userTransport)
        , eventTransport_(eventTransport)
        , loggingTransport_(loggingTransport)
        , redirectionTransport_(redirectionTransport)
        , clientStatus_(clientStatus)
        , requestId(0)
{

}

std::vector<std::uint8_t> SyncDataProcessor::compileRequest(const std::map<TransportType, ChannelDirection>& transportTypes)
{
    SyncRequest request;

    request.requestId = ++requestId;
    request.bootstrapSyncRequest.set_null();
    request.configurationSyncRequest.set_null();
    request.eventSyncRequest.set_null();
    request.logSyncRequest.set_null();
    request.notificationSyncRequest.set_null();
    request.profileSyncRequest.set_null();
    request.userSyncRequest.set_null();

    KAA_LOG_DEBUG(boost::format("Compiling sync request. RequestId: %1%") % requestId);

    auto metaRequest = metaDataTransport_->createSyncRequestMetaData();
    request.syncRequestMetaData.set_SyncRequestMetaData(*metaRequest);
    KAA_LOG_DEBUG(boost::format("Compiled SyncRequestMetaData: %1%")
                        % LoggingUtils::MetaDataSyncRequestToString(request.syncRequestMetaData));

    for (const auto& t : transportTypes) {
        bool isDownDirection = (t.second == ChannelDirection::DOWN);
        switch (t.first) {
            case TransportType::BOOTSTRAP :
                if (isDownDirection) {
                    request.bootstrapSyncRequest.set_null();
                } else if (bootstrapTransport_) {
                    auto ptr = bootstrapTransport_->createBootstrapSyncRequest();
                    if (ptr) {
                        request.bootstrapSyncRequest.set_BootstrapSyncRequest(*ptr);
                    } else {
                        request.bootstrapSyncRequest.set_null();
                    }
                } else {
                    KAA_LOG_WARN("Bootstrap transport was not specified.");
                }
                KAA_LOG_DEBUG(boost::format("Compiled BootstrapSyncRequest: %1%")
                    % LoggingUtils::BootstrapSyncRequestToString(request.bootstrapSyncRequest));
            break;
            case TransportType::PROFILE :
                if (isDownDirection) {
                    request.profileSyncRequest.set_null();
                } else if (profileTransport_) {
                    auto ptr = profileTransport_->createProfileRequest();
                    if (ptr) {
                        request.profileSyncRequest.set_ProfileSyncRequest(*ptr);
                    } else {
                        request.profileSyncRequest.set_null();
                    }
                } else {
                    KAA_LOG_WARN("Profile transport was not specified.");
                }
                KAA_LOG_DEBUG(boost::format("Compiled ProfileSyncRequest: %1%")
                    % LoggingUtils::ProfileSyncRequestToString(request.profileSyncRequest));
                break;
            case TransportType::CONFIGURATION:
                if (configurationTransport_) {
                    auto ptr = configurationTransport_->createConfigurationRequest();
                    if (ptr) {
                        request.configurationSyncRequest.set_ConfigurationSyncRequest(*ptr);
                    } else {
                        request.configurationSyncRequest.set_null();
                    }
                } else {
                    KAA_LOG_WARN("Configuration transport was not specified.");
                }
                KAA_LOG_DEBUG(boost::format("Compiled ConfigurationSyncRequest: %1%")
                   % LoggingUtils::ConfigurationSyncRequestToString(request.configurationSyncRequest));
                break;
            case TransportType::NOTIFICATION:
                if (notificationTransport_) {
                    if (isDownDirection) {
                        request.notificationSyncRequest.set_NotificationSyncRequest(*notificationTransport_->createEmptyNotificationRequest());
                    } else {
                        auto ptr = notificationTransport_->createNotificationRequest();
                        if (ptr) {
                            request.notificationSyncRequest.set_NotificationSyncRequest(*ptr);
                        } else {
                            request.notificationSyncRequest.set_null();
                        }
                    }
                } else {
                    KAA_LOG_WARN("Notification transport was not specified.");
                }
                KAA_LOG_DEBUG(boost::format("Compiled NotificationSyncRequest: %1%")
                   % LoggingUtils::NotificationSyncRequestToString(request.notificationSyncRequest));
                break;
            case TransportType::USER:
                if (isDownDirection) {
                    UserSyncRequest user;
                    user.endpointAttachRequests.set_null();
                    user.endpointDetachRequests.set_null();
                    user.userAttachRequest.set_null();
                    request.userSyncRequest.set_UserSyncRequest(user);
                } else if (userTransport_) {
                    auto ptr = userTransport_->createUserRequest();
                    if (ptr) {
                        request.userSyncRequest.set_UserSyncRequest(*ptr);
                    } else {
                        request.userSyncRequest.set_null();
                    }
                } else {
                    KAA_LOG_WARN("User transport was not specified.");
                }
                KAA_LOG_DEBUG(boost::format("Compiled UserSyncRequest: %1%")
                    % LoggingUtils::UserSyncRequestToString(request.userSyncRequest));
                break;
            case TransportType::EVENT:
                if (isDownDirection) {
                    EventSyncRequest event;
                    event.eventListenersRequests.set_null();
                    event.events.set_null();
                    request.eventSyncRequest.set_EventSyncRequest(event);
                } else if (eventTransport_) {
                    auto ptr = eventTransport_->createEventRequest(requestId);
                    if (ptr) {
                        request.eventSyncRequest.set_EventSyncRequest(*ptr);
                    } else {
                        request.eventSyncRequest.set_null();
                    }
                } else {
                    KAA_LOG_WARN("Event transport was not specified.");
                }
                KAA_LOG_DEBUG(boost::format("Compiled EventSyncRequest: %1%")
                    % LoggingUtils::EventSyncRequestToString(request.eventSyncRequest));
                break;
            case TransportType::LOGGING:
                if (isDownDirection) {
                    LogSyncRequest log;
                    log.logEntries.set_null();
                    log.requestId = 0;
                    request.logSyncRequest.set_LogSyncRequest(log);
                } else if (loggingTransport_) {
                    auto ptr = loggingTransport_->createLogSyncRequest();
                    if (ptr) {
                        request.logSyncRequest.set_LogSyncRequest(*ptr);
                    } else {
                        request.logSyncRequest.set_null();
                    }
                } else {
                    KAA_LOG_WARN("Log upload transport was not specified.");
                }
                KAA_LOG_DEBUG(boost::format("Compiled LogSyncRequest: %1%")
                    % LoggingUtils::LogSyncRequestToString(request.logSyncRequest));
                break;
            default:
                break;
        }
    }

    std::vector<std::uint8_t> encodedData;
    requestConverter_.toByteArray(request, encodedData);

    return encodedData;
}

void SyncDataProcessor::processResponse(const std::vector<std::uint8_t> &response)
{
    SyncResponse syncResponse = responseConverter_.fromByteArray(response.data(), response.size());
    std::int32_t requestId = syncResponse.requestId;
    KAA_LOG_INFO(boost::format("Got SyncResponse: requestId: %1%, result: %2%")
        % requestId % LoggingUtils::SyncResponseResultTypeToString(syncResponse.status));

    if (!syncResponse.bootstrapSyncResponse.is_null()) {
        KAA_LOG_DEBUG(boost::format("Got BootstrapSyncResponse: %1%")
            % LoggingUtils::BootstrapSyncResponseToString(syncResponse.bootstrapSyncResponse));
        if (bootstrapTransport_) {
            bootstrapTransport_->onBootstrapResponse(syncResponse.bootstrapSyncResponse.get_BootstrapSyncResponse());
        } else {
            KAA_LOG_ERROR("Got bootstrap sync response, but profile transport was not set!");
        }
    }

    if (!syncResponse.profileSyncResponse.is_null()) {
        KAA_LOG_DEBUG(boost::format("Got ProfileSyncResponse: %1%")
            % LoggingUtils::ProfileSyncResponseToString(syncResponse.profileSyncResponse));
        if (profileTransport_) {
            profileTransport_->onProfileResponse(syncResponse.profileSyncResponse.get_ProfileSyncResponse());
        } else {
            KAA_LOG_ERROR("Got profile sync response, but profile transport was not set!");
        }
    } else if (syncResponse.status == SyncResponseResultType::PROFILE_RESYNC) {
        if (profileTransport_) {
            profileTransport_->onProfileResync();
        } else {
            KAA_LOG_ERROR("Got profile resync request, but profile transport was not set!");
        }
    }

    if (!syncResponse.configurationSyncResponse.is_null()) {
        KAA_LOG_DEBUG(boost::format("Got ConfigurationSyncResponse: %1%")
                % LoggingUtils::ConfigurationSyncResponseToString(syncResponse.configurationSyncResponse));
        if (configurationTransport_) {
            configurationTransport_->onConfigurationResponse(syncResponse.configurationSyncResponse.get_ConfigurationSyncResponse());
        } else {
            KAA_LOG_ERROR("Got configuration sync response, but configuration transport was not set!");
        }
    }

    if (eventTransport_) {
        eventTransport_->onSyncResponseId(requestId);
        if (!syncResponse.eventSyncResponse.is_null()) {
            KAA_LOG_DEBUG(boost::format("Got EventSyncResponse: %1%")
                    % LoggingUtils::EventSyncResponseToString(syncResponse.eventSyncResponse));
                eventTransport_->onEventResponse(syncResponse.eventSyncResponse.get_EventSyncResponse());
        }
    } else {
        KAA_LOG_ERROR("Event transport was not set!");
    }

    if (!syncResponse.notificationSyncResponse.is_null()) {
        KAA_LOG_DEBUG(boost::format("Got NotificationSyncResponse: %1%")
                % LoggingUtils::NotificationSyncResponseToString(syncResponse.notificationSyncResponse));
        if (notificationTransport_) {
            notificationTransport_->onNotificationResponse(syncResponse.notificationSyncResponse.get_NotificationSyncResponse());
        } else {
            KAA_LOG_ERROR("Got notification sync response, but notification transport was not set!");
        }
    }

    if (!syncResponse.userSyncResponse.is_null()) {
        KAA_LOG_DEBUG(boost::format("Got UserSyncResponse: %1%")
                % LoggingUtils::UserSyncResponseToString(syncResponse.userSyncResponse));
        if (userTransport_) {
            userTransport_->onUserResponse(syncResponse.userSyncResponse.get_UserSyncResponse());
        } else {
            KAA_LOG_ERROR("Got user sync response, but user transport was not set!");
        }
    }

    if (!syncResponse.logSyncResponse.is_null()) {
        KAA_LOG_DEBUG(boost::format("Got LogSyncResponse: %1%")
                % LoggingUtils::LogSyncResponseToString(syncResponse.logSyncResponse));
        if (loggingTransport_) {
            loggingTransport_->onLogSyncResponse(syncResponse.logSyncResponse.get_LogSyncResponse());
        } else {
            KAA_LOG_ERROR("Got log upload sync response, but logging transport was not set!");
        }
    }

    if (!syncResponse.redirectSyncResponse.is_null()) {
        KAA_LOG_DEBUG(boost::format("Got RedirectSyncResponse: %1%")
                % LoggingUtils::RedirectSyncResponseToString(syncResponse.redirectSyncResponse));
        if (redirectionTransport_) {
            redirectionTransport_->onRedirectionResponse(syncResponse.redirectSyncResponse.get_RedirectSyncResponse());
        } else {
            KAA_LOG_ERROR("Got redirection sync response, but redirection transport was not set!");
        }
    }

    KAA_LOG_DEBUG("Processed SyncResponse");
    clientStatus_->save();
}

}  // namespace kaa

