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

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "kaa_test.h"

#include "kaa_error.h"
#include "kaa_common.h"
#include "kaa_defaults.h"
#include "kaa_status.h"
#include "kaa_context.h"
#include "kaa_platform_utils.h"
#include "platform/ext_sha.h"
#include "utilities/kaa_log.h"
#include "utilities/kaa_mem.h"
#include "platform/ext_sha.h"



extern kaa_error_t kaa_status_create(kaa_status_t **kaa_status_p);
extern void        kaa_status_destroy(kaa_status_t *self);

extern kaa_error_t kaa_meta_data_request_get_size(size_t *expected_size);
extern kaa_error_t kaa_meta_data_request_serialize(kaa_status_t *status
                                                 , kaa_platform_message_writer_t* writer
                                                 , uint32_t request_id);



static kaa_logger_t *logger = NULL;
static kaa_status_t *status = NULL;


void test_meta_extension_get_size_failed()
{
    KAA_TRACE_IN(logger);

    kaa_error_t error_code = kaa_meta_data_request_get_size(NULL);
    ASSERT_NOT_EQUAL(error_code, KAA_ERR_NONE);
}

void test_meta_extension_get_size()
{
    KAA_TRACE_IN(logger);

    const size_t expected_meta_extension_size = KAA_EXTENSION_HEADER_SIZE
                                              + sizeof(uint32_t) /* request id */
                                              + sizeof(uint32_t) /* timeout */
                                              + SHA_1_DIGEST_LENGTH
                                              + SHA_1_DIGEST_LENGTH
                                              + KAA_APPLICATION_TOKEN_LENGTH;

    size_t meta_extension_size;
    kaa_error_t error_code = kaa_meta_data_request_get_size(&meta_extension_size);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);
    ASSERT_EQUAL(expected_meta_extension_size, meta_extension_size);
}

void test_meta_extension_serialize_failed()
{
    KAA_TRACE_IN(logger);

    kaa_error_t error_code;
    const size_t buffer_size = 6;
    char buffer[buffer_size];
    kaa_platform_message_writer_t *writer;

    error_code = kaa_platform_message_writer_create(&writer, buffer, buffer_size);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);

    error_code = kaa_meta_data_request_serialize(NULL, NULL, 0);
    ASSERT_NOT_EQUAL(error_code, KAA_ERR_NONE);

    error_code = kaa_meta_data_request_serialize(NULL, (kaa_platform_message_writer_t *)!NULL, 0);
    ASSERT_NOT_EQUAL(error_code, KAA_ERR_NONE);

    kaa_platform_message_writer_destroy(writer);
}

void test_meta_extension_serialize()
{
    KAA_TRACE_IN(logger);

    size_t meta_extension_size;
    kaa_error_t error_code = kaa_meta_data_request_get_size(&meta_extension_size);
    char buffer[meta_extension_size];

    kaa_platform_message_writer_t *writer;
    error_code = kaa_platform_message_writer_create(&writer, buffer, meta_extension_size);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);

    uint32_t expected_timeout = KAA_SYNC_TIMEOUT;
    kaa_digest expected_public_key_hash = {0x74, 0xc7, 0x51, 0x43, 0x00, 0xf7, 0xb8, 0x21, 0x2c, 0xc3, 0x6b, 0xa5, 0x9c, 0xb4, 0x03, 0xef, 0xc2, 0x5c, 0x65, 0x6c};
    kaa_digest expected_profile_hash = {0xfa, 0x71, 0xb5, 0x02, 0xe7, 0xdf, 0x96, 0x86, 0x6c, 0xdc, 0xe1, 0x4a, 0x17, 0x35, 0x7f, 0xd9, 0xa8, 0xfb, 0x71, 0x09};

    error_code = ext_copy_sha_hash(status->endpoint_public_key_hash, expected_public_key_hash);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);

    error_code = ext_copy_sha_hash(status->profile_hash, expected_profile_hash);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);

    error_code = kaa_meta_data_request_serialize(status, writer, 1);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);

    kaa_platform_message_reader_t *reader;
    error_code = kaa_platform_message_reader_create(&reader, buffer, meta_extension_size);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);

    uint8_t extension_type;
    uint32_t extension_options;
    uint32_t extension_payload;

    error_code = kaa_platform_message_read_extension_header(
                    reader, &extension_type, &extension_options, &extension_payload);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);

    ASSERT_EQUAL(extension_type, KAA_META_DATA_EXTENSION_TYPE);
    ASSERT_EQUAL(extension_options, (TIMEOUT_VALUE | PUBLIC_KEY_HASH_VALUE | PROFILE_HASH_VALUE | APP_TOKEN_VALUE));
    ASSERT_EQUAL(extension_payload, meta_extension_size - KAA_EXTENSION_HEADER_SIZE);

    uint32_t request_id;
    uint32_t timeout;
    kaa_digest public_key_hash;
    kaa_digest profile_hash;
    char application_token[KAA_APPLICATION_TOKEN_LENGTH];

    error_code = kaa_platform_message_read(reader, &request_id, sizeof(uint32_t));
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);
    ASSERT_EQUAL(KAA_NTOHL(request_id), 1);
    error_code = kaa_platform_message_read(reader, &timeout, sizeof(uint32_t));
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);
    timeout = KAA_NTOHL(timeout);
    ASSERT_EQUAL(expected_timeout, timeout);

    error_code = kaa_platform_message_read_aligned(reader, public_key_hash, SHA_1_DIGEST_LENGTH);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);
    error_code = (memcmp(public_key_hash, expected_public_key_hash, SHA_1_DIGEST_LENGTH) == 0 ? KAA_ERR_NONE : KAA_ERR_READ_FAILED);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);

    error_code = kaa_platform_message_read_aligned(reader, profile_hash, SHA_1_DIGEST_LENGTH);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);
    error_code = (memcmp(profile_hash, expected_profile_hash, SHA_1_DIGEST_LENGTH) == 0 ? KAA_ERR_NONE : KAA_ERR_READ_FAILED);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);

    error_code = kaa_platform_message_read_aligned(reader, application_token, KAA_APPLICATION_TOKEN_LENGTH);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);
    error_code = (memcmp(application_token, APPLICATION_TOKEN, KAA_APPLICATION_TOKEN_LENGTH) == 0 ? KAA_ERR_NONE : KAA_ERR_READ_FAILED);
    ASSERT_EQUAL(error_code, KAA_ERR_NONE);

    kaa_platform_message_reader_destroy(reader);
    kaa_platform_message_writer_destroy(writer);
}



int test_init()
{
    kaa_error_t error = kaa_log_create(&logger, KAA_MAX_LOG_MESSAGE_LENGTH, KAA_MAX_LOG_LEVEL, NULL);
    if (error || !logger) {
        return error;
    }

    error = kaa_status_create(&status);
    if (error || !status) {
        return error;
    }

    return 0;
}

int test_deinit()
{
    kaa_status_destroy(status);
    kaa_log_destroy(logger);

    return 0;
}



KAA_SUITE_MAIN(MetaExtension, test_init, test_deinit,
        KAA_TEST_CASE(meta_extension_get_size_failed, test_meta_extension_get_size_failed)
        KAA_TEST_CASE(meta_extension_get_size, test_meta_extension_get_size)
        KAA_TEST_CASE(meta_extension_serialize_failed, test_meta_extension_serialize_failed)
        KAA_TEST_CASE(meta_extension_serialize, test_meta_extension_serialize)
)
