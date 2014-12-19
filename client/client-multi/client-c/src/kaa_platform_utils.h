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


#ifndef KAA_PLATFORM_UTILS_H_
#define KAA_PLATFORM_UTILS_H_

#include <stddef.h>
#include <stdint.h>

#include "kaa_error.h"
#include "kaa_platform_common.h"

#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KAA_HTONS(hostshort)    htons((hostshort));
#define KAA_HTONL(hostlong)     htonl((hostlong));

#define KAA_NTOHS(netshort)     ntohs((netshort));
#define KAA_NTOHL(netlong)      ntohl((netlong));



typedef struct kaa_platform_message_writer_t_ kaa_platform_message_writer_t;



kaa_error_t kaa_platform_message_writer_create(kaa_platform_message_writer_t** writer_p
                                             , const char *buf
                                             , size_t len);

void kaa_platform_message_writer_destroy(kaa_platform_message_writer_t* writer);

kaa_error_t kaa_platform_message_write(kaa_platform_message_writer_t* writer
                                     , const void *data
                                     , size_t data_size);

kaa_error_t kaa_platform_message_write_aligned(kaa_platform_message_writer_t* writer
                                             , const void *data
                                             , size_t data_size);

kaa_error_t kaa_platform_message_extension_header_write(kaa_platform_message_writer_t* writer
                                                      , uint8_t extension_type
                                                      , uint32_t options
                                                      , uint32_t payload_size);

const char* kaa_platform_message_writer_get_buffer(kaa_platform_message_writer_t* writer);



typedef struct kaa_platform_message_reader_t_ kaa_platform_message_reader_t;

kaa_error_t kaa_platform_message_reader_create(kaa_platform_message_reader_t **reader_p
                                             , const char *buffer
                                             , size_t len);

void kaa_platform_message_reader_destroy(kaa_platform_message_reader_t *reader);

kaa_error_t kaa_platform_message_read(kaa_platform_message_reader_t *reader
                                    , void *buffer
                                    , size_t expected_size);

kaa_error_t kaa_platform_message_read_aligned(kaa_platform_message_reader_t *reader
                                            , void *buffer
                                            , size_t expected_size);

kaa_error_t kaa_platform_message_read_extension_header(kaa_platform_message_reader_t *reader
                                                     , uint8_t *extension_type
                                                     , uint32_t *extension_options
                                                     , uint32_t *extension_payload_length);

kaa_error_t kaa_platform_message_skip(kaa_platform_message_reader_t *reader, size_t size);

static inline size_t kaa_aligned_size_get(size_t size)
{
    return (size + (KAA_ALIGNMENT - (size % KAA_ALIGNMENT)));
}



#ifdef __cplusplus
}      /* extern "C" */
#endif

#endif /* KAA_PLATFORM_UTILS_H_ */