#pragma once

#include "mesh-pb-constants.h"
#include <stddef.h>
#include <vector>

const size_t  PB_BUFSIZE = 512;
const int     MT_HEADER_SIZE = 4;

/**
 * @brief This class encodes/decodes protobuf streams with magic headers
 * 
 */
class MeshEnvelope {
  public:
    // ctor (1) creates empty vector to add bytes later for encoding
    MeshEnvelope(void);
    // ctor (2) creates envelope with validated bytestream for decoding
    MeshEnvelope(const uint8_t* pb_buf, size_t length);

    // encode envelope created in (1) with data in toRadio
    std::vector<uint8_t>& encode(const meshtastic_ToRadio& toRadio);
    // decode buffer given in (2)
    meshtastic_FromRadio decode(void);

    // check for valid packet in byte stream, strip all bytes in front of packet
    static bool validate(uint8_t* pb_buf, size_t& pb_size, size_t& payload_len);
    // invalidate first packet that has been handled already, prepare for next
    static void invalidate(uint8_t* pb_buf, size_t& pb_size, size_t& payload_len);

    ~MeshEnvelope() {}

  private:
    std::vector<uint8_t> envelope;
};