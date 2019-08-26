//
// Created by Codetector on 8/25/19.
//

#include "stdint.h"

#ifndef I8080SIM_I8080_STATE_H
#define I8080SIM_I8080_STATE_H
struct flags {
    uint8_t sign:1;
    uint8_t zero:1;
    uint8_t _padding1:1;
    uint8_t aux_carry:1;
    uint8_t  _padding2:1;
    uint8_t parity:1;
    uint8_t  _padding3:1;
    uint8_t carry:1;
};

struct i8080_state {
    union {
        uint16_t af;
        struct {
            uint8_t a;
            struct flags f;
        } sub;
    } AF;

    union {
        uint16_t bc;
        struct {
            uint8_t b;
            uint8_t c;
        } sub;
    } BC;
    union {
        uint16_t de;
        struct {
            uint8_t d;
            uint8_t e;
        } sub;
    } DE;
    union {
        uint16_t hl;
        struct {
            uint8_t h;
            uint8_t l;
        } sub;
    } HL;

    union {
        uint16_t sp;
    } SP;

    union {
        uint16_t pc;
    } PC;

    uint8_t memory[0x10000];
};

#endif //I8080SIM_I8080_STATE_H
