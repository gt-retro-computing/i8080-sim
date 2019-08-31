//
// Created by Will Gulian on 8/31/19.
//

#ifndef I8080SIM_I8080_ASM_H
#define I8080SIM_I8080_ASM_H

#include <stdint.h>

#include "i8080_state.h"

uint16_t gwemu_sprint_asm(char *buffer, uint8_t *opcode);

void gwemu_print_memory(FILE *f, struct i8080_state *state);

#endif //I8080SIM_I8080_ASM_H
