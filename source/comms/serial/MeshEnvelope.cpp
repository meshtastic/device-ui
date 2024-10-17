#include "MeshEnvelope.h"
#include "ILog.h"
#include "mesh-pb-constants.h"
#include <pb_decode.h>
#include <pb_encode.h>

extern const uint8_t MT_MAGIC_0 = 0x94;
extern const uint8_t MT_MAGIC_1 = 0xc3;

MeshEnvelope::MeshEnvelope(void) : envelope(PB_BUFSIZE + MT_HEADER_SIZE) {}

MeshEnvelope::MeshEnvelope(const uint8_t *pb_buf, size_t length) : envelope(pb_buf, pb_buf + length) {}

std::vector<uint8_t> &MeshEnvelope::encode(const meshtastic_ToRadio &toRadio)
{
    pb_ostream_t stream = pb_ostream_from_buffer(&envelope[MT_HEADER_SIZE], PB_BUFSIZE);
    if (pb_encode(&stream, meshtastic_ToRadio_fields, &toRadio)) {
        // Store the payload length in the header
        envelope[0] = MT_MAGIC_0;
        envelope[1] = MT_MAGIC_1;
        envelope[2] = (stream.bytes_written & 0xFF00) >> 8;
        envelope[3] = stream.bytes_written & 0xFF;

        if (stream.bytes_written <= PB_BUFSIZE) {
            ILOG_TRACE("encoding %d byte successful", stream.bytes_written);
            envelope.resize(stream.bytes_written + MT_HEADER_SIZE);
        } else {
            ILOG_ERROR("PB_BUFSIZE too small (<%d)", stream.bytes_written);
            envelope.resize(0);
        }
    } else {
        ILOG_ERROR("Couldn't encode toRadio");
        envelope.resize(0);
    }

    return envelope;
}

/**
 * @brief decoding of validated bytestream
 *
 * @param pb_size
 * @return meshtastic_FromRadio
 */
meshtastic_FromRadio MeshEnvelope::decode()
{
    meshtastic_FromRadio fromRadio = meshtastic_FromRadio_init_zero;
    uint16_t payload_len = envelope[2] << 8 | envelope[3];
    pb_istream_t stream = pb_istream_from_buffer(&envelope[MT_HEADER_SIZE], payload_len);
    bool status = pb_decode(&stream, meshtastic_FromRadio_fields, &fromRadio);

    if (!status) {
        ILOG_ERROR("Decoding failed!");
        return meshtastic_FromRadio(meshtastic_FromRadio_init_zero);
    }

    envelope.resize(payload_len + MT_HEADER_SIZE);
    return fromRadio;
}

/**
 * @brief search packet magic in byte stream and move it to beginning
 *
 * @param pb_buf inout: byte stream
 * @param pb_size inout: length of byte stream
 * @param size out: length of packet
 * @return true if stream contains full packet at beginning of buffer
 */
bool MeshEnvelope::validate(uint8_t *pb_buf, size_t &pb_size, size_t &payload_len)
{
    size_t startpos = 0;
    if (pb_size < MT_HEADER_SIZE) {
        return false;
    }

    while ((pb_buf[startpos] != MT_MAGIC_0 || pb_buf[startpos + 1] != MT_MAGIC_1) && startpos <= pb_size) {
        startpos++;
    }
    if (startpos >= pb_size) {
        ILOG_WARN("MeshEnvelope: no magic found, skipping %d bytes (%02x%02x%02x...)", pb_size, (int)pb_buf[0], (int)pb_buf[1],
                  (int)pb_buf[2]);
        pb_size = 0;
        return false;
    }

    payload_len = (pb_buf[startpos + 2] << 8) | pb_buf[startpos + 3];
    if (startpos > 0) {
        if (payload_len > PB_BUFSIZE) {
            ILOG_ERROR("Got packet claiming to be ridiculous length (%d bytes)", payload_len);
            pb_size = 0;
            payload_len = 0;
            return false;
        }

        // re-align magic header to front of buffer
        ILOG_TRACE("Skipping first %d bytes (%02x%02x%02x...)", startpos, (int)pb_buf[0], (int)pb_buf[1], (int)pb_buf[2]);
        pb_size -= startpos;
        memmove(&pb_buf[0], &pb_buf[startpos], pb_size);
    }

    if ((size_t)(payload_len + MT_HEADER_SIZE) > pb_size) {
        ILOG_TRACE("Partial packet received. Expected %d, have only %d", payload_len + MT_HEADER_SIZE, pb_size);
        return false;
    }

    ILOG_TRACE("Valididated packet, payload_len=%d, pb_size=%d", payload_len, pb_size);

    return true;
}

/**
 * @brief remove old packet from buffer and move next payload to front
 *
 * @param pb_buf inout: byte stream
 * @param pb_size inout: length of byte stream
 * @param size out: length of packet
 */
void MeshEnvelope::invalidate(uint8_t *pb_buf, size_t &pb_size, size_t &payload_len)
{
    pb_buf[0] = 0x00;
    pb_buf[1] = 0x00;
    if (pb_size < payload_len + MT_HEADER_SIZE) {
        pb_size = 0;
    } else {
        pb_size -= payload_len + MT_HEADER_SIZE;
    }
    memmove(&pb_buf[0], &pb_buf[payload_len + MT_HEADER_SIZE], pb_size);
}