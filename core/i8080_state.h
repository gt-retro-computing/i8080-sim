//
// Created by Codetector on 8/25/19.
//

#include "stdint.h"

#ifndef I8080SIM_I8080_STATE_H
#define I8080SIM_I8080_STATE_H

struct flags {
    uint8_t s:1;
    uint8_t z:1;
    uint8_t _padding1:1;
    uint8_t ac:1;
    uint8_t _padding2:1;
    uint8_t p:1;
    uint8_t _padding3:1;
    uint8_t c:1;
};

struct i8080_state {
    uint8_t a;
    struct flags f;

    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;

    uint8_t memory[0x10000];
    uint8_t int_enable:1;
    uint8_t halt:1;
};

void gwemu_exec_step(struct i8080_state *);

#endif //I8080SIM_I8080_STATE_H
