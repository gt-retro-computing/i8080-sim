//
// Created by Will Gulian on 8/28/19.
//

#ifndef I8080SIM_HEX_H
#define I8080SIM_HEX_H

#include <stdint.h>

struct hex_data {
    uint16_t start_addr;
    uint16_t length;
    uint8_t buf[256];
};

int hex_read_string(struct hex_data *state, uint8_t *data, size_t len);


#endif //I8080SIM_HEX_H
