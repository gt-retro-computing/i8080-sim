#include <stdint.h>
#include <stddef.h>

#include "hex.h"

static int read_hex_nib(uint8_t byte) {
    if (byte >= '0' && byte <= '9') {
        return byte - '0';
    } else if (byte >= 'a' && byte <= 'f') {
        return byte - 'a' + 10;
    } else if (byte >= 'A' && byte <= 'F') {
        return byte - 'A' + 10;
    } else {
        return -1;
    }
}

static int read_hex_byte(const uint8_t *data) {
    int high_value = read_hex_nib(*data);
    int low_value = read_hex_nib(*(data + 1));
    if (high_value < 0 || low_value < 0) {
        return -1;
    }
    return (high_value << 4) + low_value;
}

int hex_read_string(struct hex_data *state, uint8_t *data, size_t len) {
    if (*data != ':') {
        return -1;
    }
    data++;

    if (len < 9) {
        // we don't have enough characters to make a HEX data header
        return -1;
    }

    uint8_t checksum_total = 0;

    int byte_count = read_hex_byte(data);
    data += 2;
    int high_addr = read_hex_byte(data);
    data += 2;
    int low_addr = read_hex_byte(data);
    data += 2;
    int record_type = read_hex_byte(data);
    data += 2;

    if (record_type != 0 || byte_count < 0 || high_addr < 0 || low_addr < 0) {
        return -1;
    }

    checksum_total += (uint8_t) byte_count;
    checksum_total += (uint8_t) high_addr;
    checksum_total += (uint8_t) low_addr;
    checksum_total += (uint8_t) record_type;

    state->start_addr = (uint16_t) (((high_addr & 0xFF) << 8) + (low_addr & 0xFF));
    state->length = (uint16_t) byte_count;

    if (len < 11 + byte_count * 2) {
        // we don't have enough characters to make the HEX data header + data + checksum
        return -1;
    }

    for (uint16_t i = 0; i < state->length; i++) {
        int byte = read_hex_byte(data);
        if (byte < 0) {
            return -1;
        }
        state->buf[i] = (uint8_t) byte;
        checksum_total += (uint8_t) byte;
        data += 2;
    }

    int checksum = read_hex_byte(data);
    if (checksum < 0) {
        return -1;
    }

    checksum_total += (uint8_t) checksum;

    if (checksum_total != 0) {
        return -1;
    }

    return 0;
}