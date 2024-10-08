//
//
// Copyright 2015 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//

#ifndef GRPC_SRC_CORE_EXT_TRANSPORT_CHTTP2_TRANSPORT_FRAME_SETTINGS_H
#define GRPC_SRC_CORE_EXT_TRANSPORT_CHTTP2_TRANSPORT_FRAME_SETTINGS_H

#include <grpc/slice.h>
#include <grpc/support/port_platform.h>
#include <stddef.h>
#include <stdint.h>

#include "src/core/ext/transport/chttp2/transport/http2_settings.h"
#include "src/core/ext/transport/chttp2/transport/legacy_frame.h"
#include "src/core/lib/iomgr/error.h"
#include "src/core/util/manual_constructor.h"

typedef enum {
  GRPC_CHTTP2_SPS_ID0,
  GRPC_CHTTP2_SPS_ID1,
  GRPC_CHTTP2_SPS_VAL0,
  GRPC_CHTTP2_SPS_VAL1,
  GRPC_CHTTP2_SPS_VAL2,
  GRPC_CHTTP2_SPS_VAL3
} grpc_chttp2_settings_parse_state;

struct grpc_chttp2_settings_parser {
  grpc_chttp2_settings_parse_state state;
  grpc_core::Http2Settings* target_settings;
  grpc_core::ManualConstructor<grpc_core::Http2Settings> incoming_settings;
  uint8_t is_ack;
  uint16_t id;
  uint32_t value;
};

// Create an ack settings frame as per RFC
// https://datatracker.ietf.org/doc/html/rfc9113#name-settings-format
grpc_slice grpc_chttp2_settings_ack_create(void);

// The function has two tasks
// 1. Initialise the parser with information that will be needed later, at the
//    time of parsing the entire settings frame.
// 2. Running checks to see if the settings frame content is valid or not.
//    Returns an error with a helpful error string if something is wrong.
//
// TODO(tjagtap) : [PH2][P2] : Use ABSL_MUST_USE_RESULT for return value.
grpc_error_handle grpc_chttp2_settings_parser_begin_frame(
    grpc_chttp2_settings_parser* parser, uint32_t length, uint8_t flags,
    grpc_core::Http2Settings& settings);

// Mandatory to call grpc_chttp2_settings_parser_begin_frame calling before
// calling grpc_chttp2_settings_parser_parse. However our code does not assert
// that yet.
// This function parses the settings frame. A lot of the parsed content is saved
// in the parser object itself.
// This function may return different errors, be sure to check them.
//
// TODO(tjagtap) : [PH2][P2] : Assert that that
// grpc_chttp2_settings_parser_begin_frame is called before this function. Also
// assert that this did not return an error. It does not make sense to call
// this function if grpc_chttp2_settings_parser_begin_frame returned an error.
//
// TODO(tjagtap) : [PH2][P2] : Use ABSL_MUST_USE_RESULT for return value.
grpc_error_handle grpc_chttp2_settings_parser_parse(void* parser,
                                                    grpc_chttp2_transport* t,
                                                    grpc_chttp2_stream* s,
                                                    const grpc_slice& slice,
                                                    int is_last);

#endif  // GRPC_SRC_CORE_EXT_TRANSPORT_CHTTP2_TRANSPORT_FRAME_SETTINGS_H
