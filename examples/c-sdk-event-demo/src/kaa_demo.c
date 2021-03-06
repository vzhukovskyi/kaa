/*
 * Copyright 2014-2015 CyberVision, Inc.
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <execinfo.h>
#include <stddef.h>
#include <sys/select.h>

#include <kaa/kaa.h>
#include <kaa/kaa_error.h>
#include <kaa/kaa_context.h>
#include <kaa/kaa_channel_manager.h>
#include <kaa/kaa_configuration_manager.h>
#include <kaa/kaa_event.h>
#include <kaa/kaa_user.h>
#include <kaa/kaa_defaults.h>
#include <kaa/gen/kaa_thermostat_event_class_family.h>

#include <kaa/utilities/kaa_log.h>
#include <kaa/utilities/kaa_mem.h>

#include <kaa/platform/ext_sha.h>
#include <kaa/platform/ext_transport_channel.h>
#include <kaa/platform-impl/kaa_tcp_channel.h>




/*
 * Kaa status and public key storage file names.
 */
#define KAA_KEY_STORAGE       "key.txt"
#define KAA_STATUS_STORAGE    "status.conf"


#define KAA_USER_ID            "user@email.com"
#define KAA_USER_ACCESS_TOKEN  "token"


#define THERMO_REQUEST_FQN          "org.kaaproject.kaa.schema.sample.event.thermo.ThermostatInfoRequest"
#define CHANGE_DEGREE_REQUEST_FQN   "org.kaaproject.kaa.schema.sample.event.thermo.ChangeDegreeRequest"


static kaa_context_t *kaa_context_ = NULL;

static char *kaa_public_key           = NULL;
static uint32_t kaa_public_key_length = 0;

static kaa_service_t BOOTSTRAP_SERVICE[] = { KAA_SERVICE_BOOTSTRAP };
static const int BOOTSTRAP_SERVICE_COUNT = sizeof(BOOTSTRAP_SERVICE) / sizeof(kaa_service_t);

static kaa_service_t OPERATIONS_SERVICES[] = { KAA_SERVICE_PROFILE
                                             , KAA_SERVICE_USER
                                             , KAA_SERVICE_EVENT
                                             , KAA_SERVICE_LOGGING
                                             , KAA_SERVICE_CONFIGURATION };
static const int OPERATIONS_SERVICES_COUNT = sizeof(OPERATIONS_SERVICES) / sizeof(kaa_service_t);

static kaa_transport_channel_interface_t bootstrap_channel;
static kaa_transport_channel_interface_t operations_channel;

static bool is_shutdown = false;


void kaa_on_thermostat_info_request(void *context, kaa_thermostat_event_class_family_thermostat_info_request_t *event, kaa_endpoint_id_p source)
{
    KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo ThermostatInfoRequest event received!");
    kaa_thermostat_event_class_family_thermostat_info_response_t *response =
            kaa_thermostat_event_class_family_thermostat_info_response_create();


    response->thermostat_info = kaa_thermostat_event_class_family_union_thermostat_info_or_null_branch_0_create();
    kaa_thermostat_event_class_family_thermostat_info_t *info = kaa_thermostat_event_class_family_thermostat_info_create();
    response->thermostat_info->data = info;

    int32_t *current_degree = (int32_t *) KAA_MALLOC(sizeof(int32_t));
    *current_degree = -5;

    int32_t *target_degree = (int32_t *) KAA_MALLOC(sizeof(int32_t));
    *target_degree = 10;

    info->degree = kaa_thermostat_event_class_family_union_int_or_null_branch_0_create();
    info->degree->data = current_degree;

    info->target_degree = kaa_thermostat_event_class_family_union_int_or_null_branch_0_create();
    info->target_degree->data = target_degree;

    info->is_set_manually = kaa_thermostat_event_class_family_union_boolean_or_null_branch_1_create();

    kaa_event_manager_send_kaa_thermostat_event_class_family_thermostat_info_response(kaa_context_->event_manager,
                                                                                      response, NULL);

    response->destroy(response); // Destroying event that was successfully sent

    event->destroy(event);
}


void kaa_on_thermostat_info_response(void *context, kaa_thermostat_event_class_family_thermostat_info_response_t *event, kaa_endpoint_id_p source)
{
    KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo ThermostatInfoResponse event received!");
    if (event->thermostat_info->type == KAA_THERMOSTAT_EVENT_CLASS_FAMILY_UNION_THERMOSTAT_INFO_OR_NULL_BRANCH_0) {
        kaa_thermostat_event_class_family_thermostat_info_t *info =
                (kaa_thermostat_event_class_family_thermostat_info_t *) event->thermostat_info->data;

        if (info->degree->type == KAA_THERMOSTAT_EVENT_CLASS_FAMILY_UNION_INT_OR_NULL_BRANCH_0) {
            int32_t *degree = (int32_t *) info->degree->data;
            KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo degree=%d", *degree);
        }
        if (info->target_degree->type == KAA_THERMOSTAT_EVENT_CLASS_FAMILY_UNION_INT_OR_NULL_BRANCH_0) {
            int32_t *target_degree = (int32_t *) info->target_degree->data;
            KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo target_degree=%d", *target_degree);
        }
        if (info->is_set_manually->type == KAA_THERMOSTAT_EVENT_CLASS_FAMILY_UNION_BOOLEAN_OR_NULL_BRANCH_0) {
            int8_t *is_set_manually = (int8_t *) info->is_set_manually->data;
            KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo is_set_manually=%s", (*is_set_manually) ? "true" : "false");
        }
    }

    event->destroy(event);
}

void kaa_on_change_degree_request(void *context, kaa_thermostat_event_class_family_change_degree_request_t *event, kaa_endpoint_id_p source)
{
    KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo ChangeDegreeRequest event received!");
    if (event->degree->type == KAA_THERMOSTAT_EVENT_CLASS_FAMILY_UNION_INT_OR_NULL_BRANCH_0) {
        int32_t *degree = (int32_t *) event->degree->data;
        KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo changing degree to %d", *degree);
    }
    event->destroy(event);
}


kaa_error_t kaa_on_event_listeners(void *context, const kaa_endpoint_id listeners[], size_t listeners_count)
{
    KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo found %u event listeners", listeners_count);

    // Creating Change degree request
    kaa_thermostat_event_class_family_change_degree_request_t *change_degree_request =
            kaa_thermostat_event_class_family_change_degree_request_create();

    change_degree_request->degree = kaa_thermostat_event_class_family_union_int_or_null_branch_0_create();
    int32_t *new_degree = (int32_t *) KAA_MALLOC(sizeof(int32_t));
    *new_degree = 10;
    change_degree_request->degree->data = new_degree;

    // Creating Thermo info request
    kaa_thermostat_event_class_family_thermostat_info_request_t *info_request =
            kaa_thermostat_event_class_family_thermostat_info_request_create();

    // Creating and sending the event block which consists of 2 events
    kaa_event_block_id trx_id = 0;
    kaa_error_t error_code = kaa_event_create_transaction(kaa_context_->event_manager, &trx_id);
    KAA_RETURN_IF_ERR(error_code);

    error_code = kaa_event_manager_add_kaa_thermostat_event_class_family_change_degree_request_event_to_block(
            kaa_context_->event_manager, change_degree_request, NULL, trx_id);
    KAA_RETURN_IF_ERR(error_code);

    change_degree_request->destroy(change_degree_request); // Destroying event that was successfully added

    error_code = kaa_event_manager_add_kaa_thermostat_event_class_family_thermostat_info_request_event_to_block(
            kaa_context_->event_manager, info_request, NULL, trx_id);
    KAA_RETURN_IF_ERR(error_code);

    info_request->destroy(info_request); // Destroying event that was successfully added

    error_code = kaa_event_finish_transaction(kaa_context_->event_manager, trx_id);
    KAA_RETURN_IF_ERR(error_code);

    return KAA_ERR_NONE;
}


kaa_error_t kaa_on_event_listeners_failed(void *context)
{
    KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo event listeners not found");
    return KAA_ERR_NONE;
}


kaa_error_t kaa_on_attached(void *context, const char *user_external_id, const char *endpoint_access_token)
{
    KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo attached to user %s, access token %s", user_external_id, endpoint_access_token);
    return KAA_ERR_NONE;
}


kaa_error_t kaa_on_detached(void *context, const char *endpoint_access_token)
{
    KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo detached from user access token %s", endpoint_access_token);
    return KAA_ERR_NONE;
}


kaa_error_t kaa_on_attach_success(void *context)
{
    KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo attach success");

    const char *fqns[] = { THERMO_REQUEST_FQN, CHANGE_DEGREE_REQUEST_FQN };
    kaa_event_listeners_callback_t listeners_callback = { NULL, &kaa_on_event_listeners, &kaa_on_event_listeners_failed };
    kaa_error_t error_code = kaa_event_manager_find_event_listeners(kaa_context_->event_manager, fqns, 2, &listeners_callback);
    if (error_code) {
        KAA_LOG_ERROR(kaa_context_->logger, error_code, "Failed to find event listeners");
        return error_code;
    }
    return KAA_ERR_NONE;
}


kaa_error_t kaa_on_attach_failed(void *context, user_verifier_error_code_t error_code, const char *reason)
{
    KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa Demo attach failed");
    return KAA_ERR_NONE;
}


/*
 * Initializes Kaa SDK.
 */
kaa_error_t kaa_sdk_init()
{
    printf("Initializing Kaa SDK...\n");

    kaa_error_t error_code = kaa_init(&kaa_context_);
    if (error_code) {
        printf("Error during kaa context creation %d", error_code);
        return error_code;
    }

    KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Adding transport channels");

    error_code = kaa_tcp_channel_create(&operations_channel
                                      , kaa_context_->logger
                                      , OPERATIONS_SERVICES
                                      , OPERATIONS_SERVICES_COUNT);
    KAA_RETURN_IF_ERR(error_code);

    error_code = kaa_tcp_channel_create(&bootstrap_channel
                                      , kaa_context_->logger
                                      , BOOTSTRAP_SERVICE
                                      , BOOTSTRAP_SERVICE_COUNT);
    KAA_RETURN_IF_ERR(error_code);

    error_code = kaa_channel_manager_add_transport_channel(kaa_context_->channel_manager
                                                         , &bootstrap_channel
                                                         , NULL);
    KAA_RETURN_IF_ERR(error_code);

    error_code = kaa_channel_manager_add_transport_channel(kaa_context_->channel_manager
                                                         , &operations_channel
                                                         , NULL);
    KAA_RETURN_IF_ERR(error_code);

    kaa_attachment_status_listeners_t listeners = { NULL, &kaa_on_attached, &kaa_on_detached, &kaa_on_attach_success, &kaa_on_attach_failed };
    error_code = kaa_user_manager_set_attachment_listeners(kaa_context_->user_manager, &listeners);
    KAA_RETURN_IF_ERR(error_code);

    error_code = kaa_user_manager_default_attach_to_user(kaa_context_->user_manager
                                                       , KAA_USER_ID
                                                       , KAA_USER_ACCESS_TOKEN);
    KAA_RETURN_IF_ERR(error_code);

    error_code = kaa_event_manager_set_kaa_thermostat_event_class_family_change_degree_request_listener(kaa_context_->event_manager
                                                                                                      , &kaa_on_change_degree_request
                                                                                                      , NULL);
    KAA_RETURN_IF_ERR(error_code);

    error_code = kaa_event_manager_set_kaa_thermostat_event_class_family_thermostat_info_request_listener(kaa_context_->event_manager
                                                                                                        , &kaa_on_thermostat_info_request
                                                                                                        , NULL);
    KAA_RETURN_IF_ERR(error_code);

    error_code = kaa_event_manager_set_kaa_thermostat_event_class_family_thermostat_info_response_listener(kaa_context_->event_manager
                                                                                                         , &kaa_on_thermostat_info_response
                                                                                                         , NULL);
    KAA_RETURN_IF_ERR(error_code);

    KAA_LOG_TRACE(kaa_context_->logger, KAA_ERR_NONE, "Kaa SDK started");
    return KAA_ERR_NONE;
}

/*
 * Kaa demo lifecycle routine.
 */
kaa_error_t kaa_demo_init()
{
    printf("%s", "Initializing Kaa driver...");
    kaa_error_t error_code = kaa_sdk_init();
    if (error_code) {
        printf("Failed to init Kaa SDK. Error code : %d", error_code);
        return error_code;
    }
    return KAA_ERR_NONE;
}

void kaa_demo_destroy()
{
    printf("%s", "Destroying Kaa driver...");

    kaa_tcp_channel_disconnect(&operations_channel);

    kaa_deinit(kaa_context_);
    if (kaa_public_key) {
        free(kaa_public_key);
        kaa_public_key_length = 0;
    }
}

int kaa_demo_event_loop()
{
    kaa_error_t error_code = kaa_start(kaa_context_);
    if (error_code) {
        KAA_LOG_FATAL(kaa_context_->logger, error_code,"Failed to start Kaa workflow");
        return -1;
    }

    uint16_t select_timeout;
    error_code = kaa_tcp_channel_get_max_timeout(&operations_channel, &select_timeout);
    if (error_code) {
        KAA_LOG_FATAL(kaa_context_->logger, error_code,"Failed to get Operations channel keepalive timeout");
        return -1;
    }

    if (select_timeout > 3) {
        select_timeout = 3;
    }

    fd_set read_fds, write_fds, except_fds;
    int ops_fd = 0, bootstrap_fd = 0;
    struct timeval select_tv = { 0, 0 };
    int max_fd = 0;

    while (!is_shutdown) {
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);

        max_fd = 0;

        kaa_tcp_channel_get_descriptor(&operations_channel, &ops_fd);
        if (max_fd < ops_fd)
            max_fd = ops_fd;
        kaa_tcp_channel_get_descriptor(&bootstrap_channel, &bootstrap_fd);
        if (max_fd < bootstrap_fd)
            max_fd = bootstrap_fd;

        if (kaa_tcp_channel_is_ready(&operations_channel, FD_READ))
            FD_SET(ops_fd, &read_fds);
        if (kaa_tcp_channel_is_ready(&operations_channel, FD_WRITE))
            FD_SET(ops_fd, &write_fds);

        if (kaa_tcp_channel_is_ready(&bootstrap_channel, FD_READ))
            FD_SET(bootstrap_fd, &read_fds);
        if (kaa_tcp_channel_is_ready(&bootstrap_channel, FD_WRITE))
            FD_SET(bootstrap_fd, &write_fds);

        select_tv.tv_sec = select_timeout;
        select_tv.tv_usec = 0;

        int poll_result = select(max_fd + 1, &read_fds, &write_fds, NULL, &select_tv);
        if (poll_result == 0) {
            kaa_tcp_channel_check_keepalive(&operations_channel);
            kaa_tcp_channel_check_keepalive(&bootstrap_channel);
        } else if (poll_result > 0) {
            if (bootstrap_fd >= 0) {
                if (FD_ISSET(bootstrap_fd, &read_fds)) {
                    KAA_LOG_DEBUG(kaa_context_->logger, KAA_ERR_NONE,"Processing IN event for the Bootstrap client socket %d", bootstrap_fd);
                    error_code = kaa_tcp_channel_process_event(&bootstrap_channel, FD_READ);
                    if (error_code)
                        KAA_LOG_ERROR(kaa_context_->logger, KAA_ERR_NONE,"Failed to process IN event for the Bootstrap client socket %d", bootstrap_fd);
                }
                if (FD_ISSET(bootstrap_fd, &write_fds)) {
                    KAA_LOG_DEBUG(kaa_context_->logger, KAA_ERR_NONE,"Processing OUT event for the Bootstrap client socket %d", bootstrap_fd);
                    error_code = kaa_tcp_channel_process_event(&bootstrap_channel, FD_WRITE);
                    if (error_code)
                        KAA_LOG_ERROR(kaa_context_->logger, error_code,"Failed to process OUT event for the Bootstrap client socket %d", bootstrap_fd);
                }
            }
            if (ops_fd >= 0) {
                if (FD_ISSET(ops_fd, &read_fds)) {
                    KAA_LOG_DEBUG(kaa_context_->logger, KAA_ERR_NONE,"Processing IN event for the Operations client socket %d", ops_fd);
                    error_code = kaa_tcp_channel_process_event(&operations_channel, FD_READ);
                    if (error_code)
                        KAA_LOG_ERROR(kaa_context_->logger, error_code,"Failed to process IN event for the Operations client socket %d", ops_fd);
                }
                if (FD_ISSET(ops_fd, &write_fds)) {
                    KAA_LOG_DEBUG(kaa_context_->logger, KAA_ERR_NONE,"Processing OUT event for the Operations client socket %d", ops_fd);
                    error_code = kaa_tcp_channel_process_event(&operations_channel, FD_WRITE);
                    if (error_code)
                        KAA_LOG_ERROR(kaa_context_->logger, error_code,"Failed to process OUT event for the Operations client socket %d", ops_fd);
                }
            }
        } else {
            KAA_LOG_ERROR(kaa_context_->logger, KAA_ERR_BAD_STATE,"Failed to poll descriptors: %s", strerror(errno));
            return -1;
        }
    }
    return 0;
}


int main(/*int argc, char *argv[]*/)
{
    kaa_error_t error_code = kaa_demo_init();
    if (error_code) {
        printf("Failed to initialize Kaa demo. Error code: %d\n", error_code);
        return error_code;
    }

    KAA_LOG_INFO(kaa_context_->logger, KAA_ERR_NONE, "Kaa demo started");

    int rval = kaa_demo_event_loop();
    kaa_demo_destroy();

    printf("Kaa demo stopped\n");

    return rval;
}

