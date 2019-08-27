//
// Created by Codetector on 8/25/19.
//

#include "i8080_state.h"


static int parity(int x, int size) {
    int i;
    int p = 0;
    x = (x & ((1 << size) - 1));
    for (i = 0; i < size; i++) {
        if (x & 0x1) p++;
        x = x >> 1;
    }
    return (0 == (p & 0x1));
}

static uint8_t machineIN(struct i8080_state *state, uint8_t port) {
    uint8_t a = 0;
    return a;
}

static void machineOUT(struct i8080_state *state, uint8_t port) {
    /* switch(port) */
    /* { */
    /*     case 2: */
    /*         state->port.write2 = state->a & 0x7; */
    /*         break; */
    /*     case 4: */
    /*         state->port.shift0 = state->port.shift1; */
    /*         state->port.shift1 = state->a; */
    /*         break; */
    /* } */
}

void gwemu_exec_step(struct i8080_state *state) {
    uint8_t *opcode = &state->memory[state->PC.pc++];
    switch (*opcode) {
        case 0x00: // NOP
            break;
        case 0x01: // LXI B, word
            state->BC.sub.c = opcode[1];
            state->BC.sub.b = opcode[2];
            state->PC.pc += 2; // Advance 2 more bytes
            break;
        case 0x02: // STAX B
        {
            uint16_t pair = (state->BC.sub.b << 8U) | (state->BC.sub.c);
            state->memory[pair] = state->AF.sub.a;
            break;
        }
        case 0x03: // INX B
        {
            uint16_t pair = (state->BC.sub.b << 8U) | (state->BC.sub.c);
            pair = pair + 1;
            state->BC.sub.b = (pair & 0xff00U) >> 8;
            state->BC.sub.c = pair & 0xffU;
            break;
        }
        case 0x04: // INR B
            state->BC.sub.b = state->BC.sub.b + 1;
            state->AF.sub.f.z = ((state->BC.sub.b) == 0);
            state->AF.sub.f.s = ((state->BC.sub.b & 0x80U) == 0x80);
            state->AF.sub.f.p = parity(state->BC.sub.b, 8);
            state->AF.sub.f.ac = ((state->BC.sub.b & 0xfU) == 0xf);
            break;
        case 0x05: // DCR B
            state->BC.sub.b = state->BC.sub.b - 1;
            state->AF.sub.f.z = ((state->BC.sub.b) == 0);
            state->AF.sub.f.s = ((state->BC.sub.b & 0x80U) == 0x80);
            state->AF.sub.f.p = parity(state->BC.sub.b, 8);
            state->AF.sub.f.ac = ((state->BC.sub.b & 0xfU) == 0x0);
            break;
        case 0x06: // MVI B, byte
            state->BC.sub.b = opcode[1];
            state->PC.pc++; // for the data byte
            break;
        case 0x07: // RLC
        {
            uint8_t x = state->AF.sub.a;
            state->AF.sub.a = ((x << 1) | (x & 0x80) >> 7);
            state->AF.sub.f.c = ((x & 0x80) == 0x80);
            break;
        }
        case 0x09: // DAD B
        {
            uint32_t pairHL = (state->HL.sub.h << 8) | (state->HL.sub.l);
            uint32_t pairBC = (state->BC.sub.b << 8) | (state->BC.sub.c);
            pairHL = pairHL + pairBC;
            state->AF.sub.f.c = (pairHL > 0xff);
            state->HL.sub.h = (pairHL & 0xff00) >> 8;
            state->HL.sub.l = pairHL & 0xff;
            break;
        }
        case 0x0A: // LDAX B
        {
            uint16_t pair = (state->BC.sub.b << 8) | (state->BC.sub.c);
            state->AF.sub.a = state->memory[pair];
            break;
        }
        case 0x0B: // DCX B
        {
            uint16_t pair = (state->BC.sub.b << 8) | (state->BC.sub.c);
            pair = pair - 1;
            state->BC.sub.b = (pair & 0xff00) >> 8;
            state->BC.sub.c = pair & 0xff;
            break;
        }
        case 0x0C: // INR C
            state->BC.sub.c = state->BC.sub.c + 1;
            state->AF.sub.f.z = ((state->BC.sub.c) == 0);
            state->AF.sub.f.s = ((state->BC.sub.c & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->BC.sub.c, 8);
            state->AF.sub.f.ac = ((state->BC.sub.c & 0xf) == 0xf);
            break;
        case 0x0D: // DCR C
            state->BC.sub.c = state->BC.sub.c - 1;
            state->AF.sub.f.z = ((state->BC.sub.c) == 0);
            state->AF.sub.f.s = ((state->BC.sub.c & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->BC.sub.c, 8);
            state->AF.sub.f.ac = ((state->BC.sub.c & 0xf) == 0x0);
            break;
        case 0x0E: // MVI C, byte
            state->BC.sub.c = opcode[1];
            state->PC.pc++; // for the data byte
            break;
        case 0x0F: // RRC
        {
            uint8_t x = state->AF.sub.a;
            state->AF.sub.a = ((x & 1) << 7 | (x >> 1));
            state->AF.sub.f.c = ((x & 1) == 1);
            break;
        }
        case 0x11: // LXI D, word
            state->DE.sub.e = opcode[1];
            state->DE.sub.d = opcode[2];
            state->PC.pc += 2; // Advance 2 more bytes
            break;
        case 0x12: // STAX D
        {
            uint16_t pair = (state->DE.sub.d << 8) | (state->DE.sub.e);
            state->memory[pair] = state->AF.sub.a;
            break;
        }
        case 0x13: // INX D
        {
            uint16_t pair = (state->DE.sub.d << 8) | (state->DE.sub.e);
            pair = pair + 1;
            state->DE.sub.d = (pair & 0xff00) >> 8;
            state->DE.sub.e = pair & 0xff;
            break;
        }
        case 0x14: // INR D
            state->DE.sub.d = state->DE.sub.d + 1;
            state->AF.sub.f.z = ((state->DE.sub.d) == 0);
            state->AF.sub.f.s = ((state->DE.sub.d & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->DE.sub.d, 8);
            state->AF.sub.f.ac = ((state->DE.sub.d & 0xf) == 0xf);
            break;
        case 0x15: // DCR D
            state->DE.sub.d = state->DE.sub.d - 1;
            state->AF.sub.f.z = ((state->DE.sub.d) == 0);
            state->AF.sub.f.s = ((state->DE.sub.d & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->DE.sub.d, 8);
            state->AF.sub.f.ac = ((state->DE.sub.d & 0xf) == 0x0);
            break;
        case 0x16: // MVI D, byte
            state->DE.sub.d = opcode[1];
            state->PC.pc++; // for the data byte
            break;
        case 0x17: // RAL
        {
            uint8_t x = state->AF.sub.a;
            state->AF.sub.a = ((x << 1) | state->AF.sub.f.c);
            state->AF.sub.f.c = ((x & 0x80) == 0x80);
            break;
        }
        case 0x19: // DAD D
        {
            uint32_t pairHL = (state->HL.sub.h << 8) | (state->HL.sub.l);
            uint32_t pairDE = (state->DE.sub.d << 8) | (state->DE.sub.e);
            pairHL = pairHL + pairDE;
            state->AF.sub.f.c = (pairHL > 0xff);
            state->HL.sub.h = (pairHL & 0xff00) >> 8;
            state->HL.sub.l = pairHL & 0xff;
            break;
        }
        case 0x1A: // LDAX D
        {
            uint16_t pair = (state->DE.sub.d << 8) | (state->DE.sub.e);
            state->AF.sub.a = state->memory[pair];
            break;
        }
        case 0x1B: // DCX D
        {
            uint16_t pair = (state->DE.sub.d << 8) | (state->DE.sub.e);
            pair = pair - 1;
            state->BC.sub.b = (pair & 0xff00) >> 8;
            state->DE.sub.e = pair & 0xff;
            break;
        }
        case 0x1C: // INR E
            state->DE.sub.e = state->DE.sub.e + 1;
            state->AF.sub.f.z = ((state->DE.sub.e) == 0);
            state->AF.sub.f.s = ((state->DE.sub.e & 0x80U) == 0x80);
            state->AF.sub.f.p = parity(state->DE.sub.e, 8);
            state->AF.sub.f.ac = ((state->DE.sub.e & 0xfU) == 0xf);
            break;
        case 0x1D: // DCR E
            state->DE.sub.e = state->DE.sub.e - 1;
            state->AF.sub.f.z = ((state->DE.sub.e) == 0);
            state->AF.sub.f.s = ((state->DE.sub.e & 0x80U) == 0x80);
            state->AF.sub.f.p = parity(state->DE.sub.e, 8);
            state->AF.sub.f.ac = ((state->DE.sub.e & 0xFU) == 0x0);
            break;
        case 0x1E: // MVI E, byte
            state->DE.sub.e = opcode[1];
            state->PC.pc++; // for the data byte
            break;
        case 0x1F: // RAR
        {
            uint8_t x = state->AF.sub.a;
            state->AF.sub.a = ((state->AF.sub.f.c << 7) | (x >> 1));
            state->AF.sub.f.c = ((x & 1) == 1);
            break;
        }
        case 0x20: {
            break;
        }
        case 0x21: // LXI H, word
            state->HL.sub.l = opcode[1];
            state->HL.sub.h = opcode[2];
            state->PC.pc += 2; // Advance 2 more bytes
            break;
        case 0x22: // SHLD adr
        {
            uint16_t adr = (opcode[2] << 8) | (opcode[1]);
            state->memory[adr] = state->HL.sub.l;
            state->memory[adr + 1] = state->HL.sub.h;
            state->PC.pc += 2;
            break;
        }
        case 0x23: // INX H
        {
            uint16_t pair = (state->HL.sub.h << 8) | (state->HL.sub.l);
            pair = pair + 1;
            state->HL.sub.h = (pair & 0xff00) >> 8;
            state->HL.sub.l = pair & 0xff;
            break;
        }
        case 0x24: // INR H
            state->HL.sub.h = state->HL.sub.h + 1;
            state->AF.sub.f.z = ((state->HL.sub.h) == 0);
            state->AF.sub.f.s = ((state->HL.sub.h & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->HL.sub.h, 8);
            state->AF.sub.f.ac = ((state->HL.sub.h & 0xf) == 0xf);
            break;
        case 0x25: // DCR H
            state->HL.sub.h = state->HL.sub.h - 1;
            state->AF.sub.f.z = ((state->HL.sub.h) == 0);
            state->AF.sub.f.s = ((state->HL.sub.h & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->HL.sub.h, 8);
            state->AF.sub.f.ac = ((state->HL.sub.h & 0xf) == 0x0);
            break;
        case 0x26: // MVI H, byte
            state->HL.sub.h = opcode[1];
            state->PC.pc++; // for the data byte
            break;
        case 0x29: // DAD H
        {
            uint32_t pairHL = (state->HL.sub.h << 8) | (state->HL.sub.l);
            pairHL = pairHL + pairHL;
            state->AF.sub.f.c = (pairHL > 0xff);
            state->HL.sub.h = (pairHL & 0xff00) >> 8;
            state->HL.sub.l = pairHL & 0xff;
            break;
        }
        case 0x2A: //LHLD addr
        {
            uint16_t adr = (opcode[2] << 8) | (opcode[1]);
            state->HL.sub.l = state->memory[adr];
            state->HL.sub.h = state->memory[adr + 1];
            state->PC.pc += 2;
            break;
        }
        case 0x2B: // DCX H
        {
            uint16_t pair = (state->HL.sub.h << 8) | (state->HL.sub.l);
            pair = pair - 1;
            state->BC.sub.b = (pair & 0xff00) >> 8;
            state->HL.sub.l = pair & 0xff;
            break;
        }
        case 0x2C: // INR L
            state->HL.sub.l = state->HL.sub.l + 1;
            state->AF.sub.f.z = ((state->HL.sub.l) == 0);
            state->AF.sub.f.s = ((state->HL.sub.l & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->HL.sub.l, 8);
            state->AF.sub.f.ac = ((state->HL.sub.l & 0xf) == 0xf);
            break;
        case 0x2D: // DCR L
            state->HL.sub.l = state->HL.sub.l - 1;
            state->AF.sub.f.z = ((state->HL.sub.l) == 0);
            state->AF.sub.f.s = ((state->HL.sub.l & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->HL.sub.l, 8);
            state->AF.sub.f.ac = ((state->HL.sub.l & 0xf) == 0x0);
            break;
        case 0x2E: // MVI l, byte
            state->HL.sub.l = opcode[1];
            state->PC.pc++; // for the data byte
            break;
        case 0x2F: // CMA (not)
            state->AF.sub.a = ~state->AF.sub.a;
            break;
        case 0x31: // LXI SP, word
            state->SP.sp = (opcode[2] << 8) | (opcode[1]);
            state->PC.pc += 2; // Advance 2 more bytes
            break;
        case 0x32: // STA adr
        {
            uint16_t adr = (opcode[2] << 8) | (opcode[1]);
            state->memory[adr] = state->AF.sub.a;
            state->PC.pc += 2;
            break;
        }
        case 0x33: // INX M
        {
            uint16_t pair = (state->HL.sub.h << 8) | (state->HL.sub.l);
            pair = pair + 1;
            state->HL.sub.h = (pair & 0xff00) >> 8;
            state->HL.sub.l = pair & 0xff;
            break;
        }
        case 0x34: // INR M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->memory[offset] = state->memory[offset] + 1;
            state->AF.sub.f.z = ((state->memory[offset]) == 0);
            state->AF.sub.f.s = ((state->memory[offset] & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->memory[offset], 8);
            state->AF.sub.f.ac = ((state->memory[offset] & 0xf) == 0xf);
            break;
        }
        case 0x35: // DCR M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->memory[offset] = state->memory[offset] - 1;
            state->AF.sub.f.z = ((state->memory[offset]) == 0);
            state->AF.sub.f.s = ((state->memory[offset] & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->memory[offset], 8);
            state->AF.sub.f.ac = ((state->memory[offset] & 0xf) == 0x0);
            break;
        }
        case 0x36: // MVI M, byte
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->memory[offset] = opcode[1];
            state->PC.pc++; // for the data byte
            break;
        }
        case 0x37: // STC
            state->AF.sub.f.c = 1;
            break;
        case 0x38:
            break;
        case 0x39: // DAD SP
        {
            uint32_t pairHL = (state->HL.sub.h << 8) | (state->HL.sub.l);
            pairHL = pairHL + (uint32_t) state->SP.sp;
            state->AF.sub.f.c = (pairHL > 0xff);
            state->HL.sub.h = (pairHL & 0xff00) >> 8;
            state->HL.sub.l = pairHL & 0xff;
            break;
        }
        case 0x3A: // LDA adr
        {
            uint16_t adr = (opcode[2] << 8) | (opcode[1]);
            state->AF.sub.a = state->memory[adr];
            state->PC.pc += 2;
            break;
        }
        case 0x3B: // DCX SP
            state->SP.sp = state->SP.sp - 1;
            state->BC.sub.b = (state->SP.sp & 0xff00) >> 8;
            state->HL.sub.l = state->SP.sp & 0xff;
            break;
        case 0x3C: // INR A
            state->AF.sub.a = state->AF.sub.a + 1;
            state->AF.sub.f.z = ((state->AF.sub.a) == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.ac = ((state->AF.sub.a & 0xf) == 0xf);
            break;
        case 0x3D: // DCR A
            state->AF.sub.a = state->AF.sub.a - 1;
            state->AF.sub.f.z = ((state->AF.sub.a) == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.ac = ((state->AF.sub.a & 0xf) == 0x0);
            break;
        case 0x3E: // MVI A, byte
            state->AF.sub.a = opcode[1];
            state->PC.pc++; // for the data byte
            break;
        case 0x3F: // CMC
            state->AF.sub.f.c = !state->AF.sub.f.c;
            break;
        case 0x40: // MOV B, B
            state->BC.sub.b = state->BC.sub.b;
            break;
        case 0x41: // MOV B, C
            state->BC.sub.b = state->BC.sub.c;
            break;
        case 0x42: // MOV B, D
            state->BC.sub.b = state->DE.sub.d;
            break;
        case 0x43: // MOV B, E
            state->BC.sub.b = state->DE.sub.e;
            break;
        case 0x44: // MOV B, H
            state->BC.sub.b = state->HL.sub.h;
            break;
        case 0x45: // MOV B, L
            state->BC.sub.b = state->HL.sub.l;
            break;
        case 0x46: // MOV B, M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->BC.sub.b = state->memory[offset];
            break;
        }
        case 0x47: // MOV B, A
            state->BC.sub.b = state->AF.sub.a;
            break;
        case 0x48: // MOV C, B
            state->BC.sub.c = state->BC.sub.b;
            break;
        case 0x49: // MOV C, C
            state->BC.sub.c = state->BC.sub.c;
            break;
        case 0x4A: // MOV C, D
            state->BC.sub.c = state->DE.sub.d;
            break;
        case 0x4B: // MOV C, E
            state->BC.sub.c = state->DE.sub.e;
            break;
        case 0x4C: // MOV C, H
            state->BC.sub.c = state->HL.sub.h;
            break;
        case 0x4D: // MOV C, L
            state->BC.sub.c = state->HL.sub.l;
            break;
        case 0x4E: // MOV C, M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->BC.sub.c = state->memory[offset];
            break;
        }
        case 0x4F: // MOV C, A
            state->BC.sub.c = state->AF.sub.a;
            break;
        case 0x50: // MOV D, B
            state->DE.sub.d = state->BC.sub.b;
            break;
        case 0x51: // MOV D, C
            state->DE.sub.d = state->BC.sub.c;
            break;
        case 0x52: // MOV D, D
            state->DE.sub.d = state->DE.sub.d;
            break;
        case 0x53: // MOV D, E
            state->DE.sub.d = state->DE.sub.e;
            break;
        case 0x54: // MOV D, H
            state->DE.sub.d = state->HL.sub.h;
            break;
        case 0x55: // MOV D, L
            state->DE.sub.d = state->HL.sub.l;
            break;
        case 0x56: // MOV D, M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->DE.sub.d = state->memory[offset];
            break;
        }
        case 0x57: // MOV D, A
            state->DE.sub.d = state->AF.sub.a;
            break;
        case 0x58: // MOV E, B
            state->DE.sub.e = state->BC.sub.b;
            break;
        case 0x59: // MOV E, C
            state->DE.sub.e = state->BC.sub.c;
            break;
        case 0x5A: // MOV E, D
            state->DE.sub.e = state->DE.sub.d;
            break;
        case 0x5B: // MOV E, E
            state->DE.sub.e = state->DE.sub.e;
            break;
        case 0x5C: // MOV E, H
            state->DE.sub.e = state->HL.sub.h;
            break;
        case 0x5D: // MOV E, L
            state->DE.sub.e = state->HL.sub.l;
            break;
        case 0x5E: // MOV E, M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->DE.sub.e = state->memory[offset];
            break;
        }
        case 0x5F: // MOV E, A
            state->DE.sub.e = state->AF.sub.a;
            break;
        case 0x60: // MOV H, B
            state->HL.sub.h = state->BC.sub.b;
            break;
        case 0x61: // MOV H, C
            state->HL.sub.h = state->BC.sub.c;
            break;
        case 0x62: // MOV H, D
            state->HL.sub.h = state->DE.sub.d;
            break;
        case 0x63: // MOV H, E
            state->HL.sub.h = state->DE.sub.e;
            break;
        case 0x64: // MOV H, H
            state->HL.sub.h = state->HL.sub.h;
            break;
        case 0x65: // MOV H, L
            state->HL.sub.h = state->HL.sub.l;
            break;
        case 0x66: // MOV H, M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->HL.sub.h = state->memory[offset];
            break;
        }
        case 0x67: // MOV H, A
            state->HL.sub.h = state->AF.sub.a;
            break;
        case 0x68: // MOV L, B
            state->HL.sub.l = state->BC.sub.b;
            break;
        case 0x69: // MOV L, C
            state->HL.sub.l = state->BC.sub.c;
            break;
        case 0x6A: // MOV L, D
            state->HL.sub.l = state->DE.sub.d;
            break;
        case 0x6B: // MOV L, E
            state->HL.sub.l = state->DE.sub.e;
            break;
        case 0x6C: // MOV L, H
            state->HL.sub.l = state->HL.sub.h;
            break;
        case 0x6D: // MOV L, L
            state->HL.sub.l = state->HL.sub.l;
            break;
        case 0x6E: // MOV L, M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->HL.sub.l = state->memory[offset];
            break;
        }
        case 0x6F: // MOV L, A
            state->HL.sub.l = state->AF.sub.a;
            break;
        case 0x70: // MOV M, B
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->memory[offset] = state->BC.sub.b;
            break;
        }
        case 0x71: // MOV M, C
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->memory[offset] = state->BC.sub.c;
            break;
        }
        case 0x72: // MOV M, D
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->memory[offset] = state->DE.sub.d;
            break;
        }
        case 0x73: // MOV M, E
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->memory[offset] = state->DE.sub.e;
            break;
        }
        case 0x74: // MOV M, H
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->memory[offset] = state->HL.sub.h;
            break;
        }
        case 0x75: // MOV M, L
        {
            uint16_t offset = (state->HL.sub.h << 8U) | (state->HL.sub.l);
            state->memory[offset] = state->HL.sub.l;
            break;
        }
        case 0x76: // HLT
            state->halt = 1;
        case 0x77: // MOV B, A
        {
            uint16_t offset = (state->HL.sub.h << 8U) | (state->HL.sub.l);
            state->memory[offset] = state->AF.sub.a;
            break;
        }
        case 0x78: // MOV A, B
            state->AF.sub.a = state->BC.sub.b;
            break;
        case 0x79: // MOV A, C
            state->AF.sub.a = state->BC.sub.c;
            break;
        case 0x7A: // MOV A, D
            state->AF.sub.a = state->DE.sub.d;
            break;
        case 0x7B: // MOV A, E
            state->AF.sub.a = state->DE.sub.e;
            break;
        case 0x7C: // MOV A, H
            state->AF.sub.a = state->HL.sub.h;
            break;
        case 0x7D: // MOV A, L
            state->AF.sub.a = state->HL.sub.l;
            break;
        case 0x7E: // MOV A, M
        {
            uint16_t offset = (state->HL.sub.h << 8U) | (state->HL.sub.l);
            state->AF.sub.a = state->memory[offset];
            break;
        }
        case 0x7F: // MOV A, A
            state->AF.sub.a = state->AF.sub.a;
            break;
        case 0x80: // ADD B
        {
            // do math with higher precision so we can capture the
            // carry out
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->BC.sub.b;

            // Zero flag: if the result is zero,
            // set the flag to zero
            // else clear the flag
            if ((answer & 0xffU) == 0)
                state->AF.sub.f.z = 1;
            else
                state->AF.sub.f.z = 0;

            // Sign flag: if bit 7 is set,
            // set the sign flag
            // else clear the sign flag
            if (answer & 0x80U)
                state->AF.sub.f.s = 1;
            else
                state->AF.sub.f.s = 0;

            // Carry flag
            if (answer > 0xff)
                state->AF.sub.f.c = 1;
            else
                state->AF.sub.f.c = 0;

            // Parity is handled by a subroutine
            state->AF.sub.f.p = parity(answer & 0xff, 8);

            // Auxiliary Carry
            if ((state->AF.sub.a & 0xf) + (state->BC.sub.b & 0xf) > 0xf)
                state->AF.sub.f.ac = 1;
            else
                state->AF.sub.f.ac = 0;

            state->AF.sub.a = answer & 0xff;
            break;
        }
            // The code for ADD can be condensed like this
        case 0x81: // ADD C
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->BC.sub.c;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->BC.sub.c & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x82: // ADD D
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->DE.sub.d;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->DE.sub.d & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x83: // ADD E
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->DE.sub.e;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->DE.sub.e & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x84: // ADD H
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->HL.sub.h;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->HL.sub.h & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x85: // ADD L
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->HL.sub.l;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->HL.sub.l & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x86: // ADD M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            uint16_t answer = (uint16_t) state->AF.sub.a + state->memory[offset];
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->memory[offset] & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x87: // ADD A
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->AF.sub.a;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->AF.sub.a & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x88: // ADC B
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->BC.sub.b + state->AF.sub.f.c;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->BC.sub.b & 0xf) + state->AF.sub.f.c) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x89: // ADC C
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->BC.sub.c + state->AF.sub.f.c;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->BC.sub.c & 0xf) + state->AF.sub.f.c) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x8A: // ADC D
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->DE.sub.d + state->AF.sub.f.c;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->DE.sub.d & 0xf) + state->AF.sub.f.c) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x8B: // ADC E
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->DE.sub.e + state->AF.sub.f.c;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->DE.sub.e & 0xf) + state->AF.sub.f.c) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x8C: // ADC H
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->HL.sub.h + state->AF.sub.f.c;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->HL.sub.h & 0xf) + state->AF.sub.f.c) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x8D: // ADC L
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->HL.sub.l + state->AF.sub.f.c;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->HL.sub.l & 0xf) + state->AF.sub.f.c) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x8E: // ADC M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            uint16_t answer = (uint16_t) state->AF.sub.a + state->memory[offset] + state->AF.sub.f.c;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->memory[offset] & 0xf) + state->AF.sub.f.c) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x8F: // ADC A
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) state->AF.sub.a + state->AF.sub.f.c;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (state->AF.sub.a & 0xf) + state->AF.sub.f.c) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x90: // SUB B
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - (uint16_t) state->BC.sub.b;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x91: // SUB C
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - (uint16_t) state->BC.sub.c;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x92: // SUB D
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - (uint16_t) state->DE.sub.d;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->DE.sub.d) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x93: // SUB E
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - (uint16_t) state->DE.sub.e;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->DE.sub.e) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x94: // SUB H
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - (uint16_t) state->HL.sub.h;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->HL.sub.h) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x95: // SUB L
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - (uint16_t) state->HL.sub.l;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->HL.sub.l) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x96: // SUB M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            uint16_t answer = (uint16_t) state->AF.sub.a - state->memory[offset];
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->memory[offset]) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x97: // SUB A
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - (uint16_t) state->AF.sub.a;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->AF.sub.a) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x98: // SBB B
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - ((uint16_t) state->BC.sub.b + state->AF.sub.f.c);
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.b + state->AF.sub.f.c) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x99: // SBB C
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - ((uint16_t) state->BC.sub.c + state->AF.sub.f.c);
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c + state->AF.sub.f.c) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x9A: // SBB D
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - ((uint16_t) state->DE.sub.d + state->AF.sub.f.c);
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->DE.sub.d + state->AF.sub.f.c) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x9B: // SBB E
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - ((uint16_t) state->DE.sub.e + state->AF.sub.f.c);
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->DE.sub.e + state->AF.sub.f.c) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x9C: // SBB H
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - ((uint16_t) state->HL.sub.h + state->AF.sub.f.c);
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->HL.sub.h + state->AF.sub.f.c) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x9D: // SBB L
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - ((uint16_t) state->HL.sub.l + state->AF.sub.f.c);
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->HL.sub.l + state->AF.sub.f.c) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x9E: // SBB M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            uint16_t answer = (uint16_t) state->AF.sub.a - (state->memory[offset] + state->AF.sub.f.c);
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->memory[offset] + state->AF.sub.f.c) & 0xf)) >
                                  0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0x9F: // SBB A
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - ((uint16_t) state->AF.sub.a + state->AF.sub.f.c);
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->AF.sub.a + state->AF.sub.f.c) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0xA0: // ANA B
            state->AF.sub.a = state->AF.sub.a & state->BC.sub.b;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xA1: // ANA C
            state->AF.sub.a = state->AF.sub.a & state->BC.sub.c;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xA2: // ANA D
            state->AF.sub.a = state->AF.sub.a & state->DE.sub.d;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xA3: // ANA E
            state->AF.sub.a = state->AF.sub.a & state->DE.sub.e;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xA4: // ANA H
            state->AF.sub.a = state->AF.sub.a & state->HL.sub.h;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xA5: // ANA L
            state->AF.sub.a = state->AF.sub.a & state->HL.sub.l;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xA6: // ANA M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->AF.sub.a = state->AF.sub.a & state->memory[offset];
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        }
        case 0xA7: // ANA A
            state->AF.sub.a = state->AF.sub.a & state->AF.sub.a;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xA8: // XRA B
            state->AF.sub.a = state->AF.sub.a ^ state->BC.sub.b;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xA9: // XRA C
            state->AF.sub.a = state->AF.sub.a ^ state->BC.sub.c;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xAA: // XRA D
            state->AF.sub.a = state->AF.sub.a ^ state->DE.sub.d;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xAB: // XRA E
            state->AF.sub.a = state->AF.sub.a ^ state->DE.sub.e;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xAC: // XRA H
            state->AF.sub.a = state->AF.sub.a ^ state->HL.sub.h;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xAD: // XRA L
            state->AF.sub.a = state->AF.sub.a ^ state->HL.sub.l;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xAE: // XRA M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->AF.sub.a = state->AF.sub.a ^ state->memory[offset];
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        }
        case 0xAF: // XRA A
            state->AF.sub.a = state->AF.sub.a ^ state->AF.sub.a;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xB0: // ORA B
            state->AF.sub.a = state->AF.sub.a | state->BC.sub.b;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xB1: // ORA C
            state->AF.sub.a = state->AF.sub.a | state->BC.sub.c;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xB2: // ORA D
            state->AF.sub.a = state->AF.sub.a | state->DE.sub.d;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xB3: // ORA E
            state->AF.sub.a = state->AF.sub.a | state->DE.sub.e;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xB4: // ORA H
            state->AF.sub.a = state->AF.sub.a | state->HL.sub.h;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xB5: // ORA L
            state->AF.sub.a = state->AF.sub.a | state->HL.sub.l;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xB6: // ORA M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            state->AF.sub.a = state->AF.sub.a | state->memory[offset];
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        }
        case 0xB7: // ORA A
            state->AF.sub.a = state->AF.sub.a | state->AF.sub.a;
            state->AF.sub.f.z = (state->AF.sub.a == 0);
            state->AF.sub.f.s = ((state->AF.sub.a & 0x80) == 0x80);
            state->AF.sub.f.p = parity(state->AF.sub.a, 8);
            state->AF.sub.f.c = 0; // clears CY
            state->AF.sub.f.ac = 0; // clears AC
            break;
        case 0xB8: // CMP B
        {
            // Like SUB B but without storing
            uint16_t res = (uint16_t) state->AF.sub.a - (uint16_t) state->BC.sub.b;
            state->AF.sub.f.z = ((res & 0xff) == 0);
            state->AF.sub.f.s = ((res & 0x80) == 0x80);
            state->AF.sub.f.c = (res > 0xff);
            state->AF.sub.f.p = parity(res & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c) & 0xf)) > 0xf);
            break;
        }
        case 0xB9: // CMP C
        {
            uint16_t res = (uint16_t) state->AF.sub.a - (uint16_t) state->BC.sub.c;
            state->AF.sub.f.z = ((res & 0xff) == 0);
            state->AF.sub.f.s = ((res & 0x80) == 0x80);
            state->AF.sub.f.c = (res > 0xff);
            state->AF.sub.f.p = parity(res & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c) & 0xf)) > 0xf);
            break;
        }
        case 0xBA: // CMP D
        {
            uint16_t res = (uint16_t) state->AF.sub.a - (uint16_t) state->DE.sub.d;
            state->AF.sub.f.z = ((res & 0xff) == 0);
            state->AF.sub.f.s = ((res & 0x80) == 0x80);
            state->AF.sub.f.c = (res > 0xff);
            state->AF.sub.f.p = parity(res & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c) & 0xf)) > 0xf);
            break;
        }
        case 0xBB: // CMP E
        {
            uint16_t res = (uint16_t) state->AF.sub.a - (uint16_t) state->DE.sub.e;
            state->AF.sub.f.z = ((res & 0xff) == 0);
            state->AF.sub.f.s = ((res & 0x80) == 0x80);
            state->AF.sub.f.c = (res > 0xff);
            state->AF.sub.f.p = parity(res & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c) & 0xf)) > 0xf);
            break;
        }
        case 0xBC: // CMP H
        {
            uint16_t res = (uint16_t) state->AF.sub.a - (uint16_t) state->HL.sub.h;
            state->AF.sub.f.z = ((res & 0xff) == 0);
            state->AF.sub.f.s = ((res & 0x80) == 0x80);
            state->AF.sub.f.c = (res > 0xff);
            state->AF.sub.f.p = parity(res & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c) & 0xf)) > 0xf);
            break;
        }
        case 0xBD: // CMP L
        {
            uint16_t res = (uint16_t) state->AF.sub.a - (uint16_t) state->HL.sub.l;
            state->AF.sub.f.z = ((res & 0xff) == 0);
            state->AF.sub.f.s = ((res & 0x80) == 0x80);
            state->AF.sub.f.c = (res > 0xff);
            state->AF.sub.f.p = parity(res & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c) & 0xf)) > 0xf);
            break;
        }
        case 0xBE: // CMP M
        {
            uint16_t offset = (state->HL.sub.h << 8) | (state->HL.sub.l);
            uint16_t res = (uint16_t) state->AF.sub.a - (uint16_t) state->memory[offset];
            state->AF.sub.f.z = ((res & 0xff) == 0);
            state->AF.sub.f.s = ((res & 0x80) == 0x80);
            state->AF.sub.f.c = (res > 0xff);
            state->AF.sub.f.p = parity(res & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c) & 0xf)) > 0xf);
            break;
        }
        case 0xBF: // CMP A
        {
            uint16_t res = (uint16_t) state->AF.sub.a - (uint16_t) state->HL.sub.l;
            state->AF.sub.f.z = ((res & 0xff) == 0);
            state->AF.sub.f.s = ((res & 0x80) == 0x80);
            state->AF.sub.f.c = (res > 0xff);
            state->AF.sub.f.p = parity(res & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c) & 0xf)) > 0xf);
            break;
        }
            /*....*/
        case 0xC0: // RNZ
            if (state->AF.sub.f.z == 0) {
                state->PC.pc = state->memory[state->SP.sp] | (state->memory[state->SP.sp + 1] << 8);
                state->SP.sp += 2;
            }
            break;
        case 0xC1: // POP B
            state->BC.sub.c = state->memory[state->SP.sp];
            state->BC.sub.b = state->memory[state->SP.sp + 1];
            state->SP.sp += 2;
            break;
        case 0xC2: // JNZ address
            if (state->AF.sub.f.z == 0)
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            else
                // branch not taken
                state->PC.pc += 2;
            break;
        case 0xC3: // JMP address
            state->PC.pc = (opcode[2] << 8) | opcode[1];
            break;
        case 0xC4: // CNZ address
        {
            if (state->AF.sub.f.z == 0) {
                uint16_t ret = state->PC.pc + 2;
                state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
                state->memory[state->SP.sp - 2] = (ret & 0xff);
                state->SP.sp = state->SP.sp - 2;
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            } else
                state->PC.pc += 2;
            break;
        }
        case 0xC5: // PUSH B
            state->memory[state->SP.sp - 1] = state->BC.sub.b;
            state->memory[state->SP.sp - 2] = state->BC.sub.c;
            state->SP.sp = state->SP.sp - 2;
            break;
        case 0xC6: // ADI byte
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) opcode[1];
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (opcode[1] & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0xC7: // RST 0
        {
            uint16_t ret = state->PC.pc + 2;
            state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
            state->memory[state->SP.sp - 2] = (ret & 0xff);
            state->SP.sp = state->SP.sp - 2;
            state->PC.pc = 0x0;
            break;
        }
        case 0xC8: // RZ
            if (state->AF.sub.f.z == 1) {
                state->PC.pc = state->memory[state->SP.sp] | (state->memory[state->SP.sp + 1] << 8);
                state->SP.sp += 2;
            }
            break;
        case 0xC9: // RET
            state->PC.pc = state->memory[state->SP.sp] | (state->memory[state->SP.sp + 1] << 8);
            state->SP.sp += 2;
            break;
        case 0xCA: // JZ address
            if (state->AF.sub.f.z == 1)
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            else
                // branch not taken
                state->PC.pc += 2;
            break;
        case 0xCC: // CZ address
        {
            if (state->AF.sub.f.z == 1) {
                uint16_t ret = state->PC.pc + 2;
                state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
                state->memory[state->SP.sp - 2] = (ret & 0xff);
                state->SP.sp = state->SP.sp - 2;
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            } else
                state->PC.pc += 2;
            break;
        }
        case 0xCD: // CALL address
        {
            uint16_t ret = state->PC.pc + 2;
            state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
            state->memory[state->SP.sp - 2] = (ret & 0xff);
            state->SP.sp = state->SP.sp - 2;
            state->PC.pc = (opcode[2] << 8) | opcode[1];
            break;
        }
        case 0xCE: // ACI byte
        {
            uint16_t answer = (uint16_t) state->AF.sub.a + (uint16_t) opcode[1] + state->AF.sub.f.c;
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (opcode[1] & 0xf) + state->AF.sub.f.c) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0xCF: // RST 1
        {
            uint16_t ret = state->PC.pc + 2;
            state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
            state->memory[state->SP.sp - 2] = (ret & 0xff);
            state->SP.sp = state->SP.sp - 2;
            state->PC.pc = 0x8;
            break;
        }
        case 0xD0: // RNC
            if (state->AF.sub.f.c == 0) {
                state->PC.pc = state->memory[state->SP.sp] | (state->memory[state->SP.sp + 1] << 8);
                state->SP.sp += 2;
            }
            break;
        case 0xD1: // POP D
            state->DE.sub.e = state->memory[state->SP.sp];
            state->DE.sub.d = state->memory[state->SP.sp + 1];
            state->SP.sp += 2;
            break;
        case 0xD2: // JNC address
            if (state->AF.sub.f.c == 0)
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            else
                // branch not taken
                state->PC.pc += 2;
            break;
        case 0xD3: // OUT byte
        {
            uint8_t port = opcode[1];
            machineOUT(state, port);
            state->PC.pc++;
            break;
        }
        case 0xD4: // CNC address
        {
            if (state->AF.sub.f.c == 0) {
                uint16_t ret = state->PC.pc + 2;
                state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
                state->memory[state->SP.sp - 2] = (ret & 0xff);
                state->SP.sp = state->SP.sp - 2;
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            } else
                state->PC.pc += 2;
            break;
        }
        case 0xD5: // PUSH D
            state->memory[state->SP.sp - 1] = state->DE.sub.d;
            state->memory[state->SP.sp - 2] = state->DE.sub.e;
            state->SP.sp = state->SP.sp - 2;
            break;
        case 0xD6: // SUI byte
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - (uint16_t) opcode[1];
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(opcode[1]) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0xD7: // RST 2
        {
            uint16_t ret = state->PC.pc + 2;
            state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
            state->memory[state->SP.sp - 2] = (ret & 0xff);
            state->SP.sp = state->SP.sp - 2;
            state->PC.pc = 0x10;
            break;
        }
        case 0xD8: // RC
            if (state->AF.sub.f.c == 1) {
                state->PC.pc = state->memory[state->SP.sp] | (state->memory[state->SP.sp + 1] << 8);
                state->SP.sp += 2;
            }
            break;
        case 0xDA: // JC address
            if (state->AF.sub.f.c == 1)
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            else
                // branch not taken
                state->PC.pc += 2;
            break;
        case 0xDB: // IN byte
        {
            uint8_t port = opcode[1];
            state->AF.sub.a = machineIN(state, port);
            state->PC.pc++;
            break;
        }
        case 0xDC: // CC address
        {
            if (state->AF.sub.f.c == 1) {
                uint16_t ret = state->PC.pc + 2;
                state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
                state->memory[state->SP.sp - 2] = (ret & 0xff);
                state->SP.sp = state->SP.sp - 2;
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            } else
                state->PC.pc += 2;
            break;
        }
        case 0xDE: // SCI byte
        {
            uint16_t answer = (uint16_t) state->AF.sub.a - ((uint16_t) opcode[1] + state->AF.sub.f.c);
            state->AF.sub.f.z = ((answer & 0xff) == 0);
            state->AF.sub.f.s = ((answer & 0x80) == 0x80);
            state->AF.sub.f.c = (answer > 0xff);
            state->AF.sub.f.p = parity(answer & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(opcode[1] + state->AF.sub.f.c) & 0xf)) > 0xf);
            state->AF.sub.a = answer & 0xff;
            break;
        }
        case 0xDF: // RST 3
        {
            uint16_t ret = state->PC.pc + 2;
            state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
            state->memory[state->SP.sp - 2] = (ret & 0xff);
            state->SP.sp = state->SP.sp - 2;
            state->PC.pc = 0x18;
            break;
        }
        case 0xE0: // RPO
            if (state->AF.sub.f.p == 0) {
                state->PC.pc = state->memory[state->SP.sp] | (state->memory[state->SP.sp + 1] << 8);
                state->SP.sp += 2;
            }
            break;
        case 0xE1: // POP H
            state->HL.sub.l = state->memory[state->SP.sp];
            state->HL.sub.h = state->memory[state->SP.sp + 1];
            state->SP.sp += 2;
            break;
        case 0xE2: // JPO address
            if (state->AF.sub.f.p == 0)
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            else
                // branch not taken
                state->PC.pc += 2;
            break;
        case 0xE3: // XTHL
        {
            uint8_t swap = state->HL.sub.l;
            state->HL.sub.l = state->memory[state->SP.sp];
            state->memory[state->SP.sp] = swap;

            swap = state->HL.sub.h;
            state->HL.sub.h = state->memory[state->SP.sp + 1];
            state->memory[state->SP.sp + 1] = swap;
            break;
        }
        case 0xE4: // CPO address
        {
            if (state->AF.sub.f.p == 0) {
                uint16_t ret = state->PC.pc + 2;
                state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
                state->memory[state->SP.sp - 2] = (ret & 0xff);
                state->SP.sp = state->SP.sp - 2;
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            } else
                state->PC.pc += 2;
            break;
        }
        case 0xE5: // PUSH H
            state->memory[state->SP.sp - 1] = state->HL.sub.h;
            state->memory[state->SP.sp - 2] = state->HL.sub.l;
            state->SP.sp = state->SP.sp - 2;
            break;
        case 0xE6: // ANI byte
        {
            uint8_t x = state->AF.sub.a & opcode[1];
            state->AF.sub.f.z = (x == 0);
            state->AF.sub.f.s = ((x & 0x80) == 0x80);
            state->AF.sub.f.p = parity(x, 8);
            state->AF.sub.f.c = 0; // Data book says op clears carry flags
            state->AF.sub.f.ac = 0;
            state->AF.sub.a = x;
            state->PC.pc++; // for the data byte
            break;
        }
        case 0xE7: // RST 4
        {
            uint16_t ret = state->PC.pc + 2;
            state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
            state->memory[state->SP.sp - 2] = (ret & 0xff);
            state->SP.sp = state->SP.sp - 2;
            state->PC.pc = 0x20;
            break;
        }
        case 0xE8: // RPE
            if (state->AF.sub.f.p == 1) {
                state->PC.pc = state->memory[state->SP.sp] | (state->memory[state->SP.sp + 1] << 8);
                state->SP.sp += 2;
            }
            break;
        case 0xE9: // PCHL
            state->PC.pc = (state->HL.sub.h << 8) | (state->HL.sub.l);
            break;
        case 0xEA: // JPE address
            if (state->AF.sub.f.p == 1)
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            else
                // branch not taken
                state->PC.pc += 2;
            break;
        case 0xEB: // XCHG
        {
            uint8_t swap = state->HL.sub.l;
            state->HL.sub.l = state->DE.sub.e;
            state->DE.sub.e = swap;

            swap = state->HL.sub.h;
            state->HL.sub.h = state->DE.sub.d;
            state->DE.sub.d = swap;
            break;
        }
        case 0xEC: // CPE address
        {
            if (state->AF.sub.f.p == 1) {
                uint16_t ret = state->PC.pc + 2;
                state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
                state->memory[state->SP.sp - 2] = (ret & 0xff);
                state->SP.sp = state->SP.sp - 2;
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            } else
                state->PC.pc += 2;
            break;
        }
        case 0xEE: // XRI byte
        {
            uint8_t x = state->AF.sub.a ^ opcode[1];
            state->AF.sub.f.z = (x == 0);
            state->AF.sub.f.s = ((x & 0x80) == 0x80);
            state->AF.sub.f.p = parity(x, 8);
            state->AF.sub.f.c = 0; // Data book says op clears carry flags
            state->AF.sub.f.ac = 0;
            state->AF.sub.a = x;
            state->PC.pc++; // for the data byte
            break;
        }
        case 0xEF: // RST 5
        {
            uint16_t ret = state->PC.pc + 2;
            state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
            state->memory[state->SP.sp - 2] = (ret & 0xff);
            state->SP.sp = state->SP.sp - 2;
            state->PC.pc = 0x28;
            break;
        }
        case 0xF0: // RP
            if (state->AF.sub.f.s == 0) {
                state->PC.pc = state->memory[state->SP.sp] | (state->memory[state->SP.sp + 1] << 8);
                state->SP.sp += 2;
            }
            break;
        case 0xF1: // POP PSW
        {
            state->AF.sub.a = state->memory[state->SP.sp + 1];
            uint8_t psw = state->memory[state->SP.sp];
            state->AF.sub.f.z = ((psw & 0x01) == 0x01);
            state->AF.sub.f.s = ((psw & 0x02) == 0x02);
            state->AF.sub.f.p = ((psw & 0x04) == 0x04);
            state->AF.sub.f.c = ((psw & 0x08) == 0x08);
            state->AF.sub.f.ac = ((psw & 0x10) == 0x10);
            state->SP.sp += 2;
            break;
        }
        case 0xF2: // JP address
            if (state->AF.sub.f.s == 0)
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            else
                // branch not taken
                state->PC.pc += 2;
            break;
        case 0xF3: // DI
            state->int_enable = 0;
            break;
        case 0xF4: // CP address
        {
            if (state->AF.sub.f.s == 0) {
                uint16_t ret = state->PC.pc + 2;
                state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
                state->memory[state->SP.sp - 2] = (ret & 0xff);
                state->SP.sp = state->SP.sp - 2;
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            } else
                state->PC.pc += 2;
            break;
        }
        case 0xF5: // PUSH PSW
        {
            state->memory[state->SP.sp - 1] = state->AF.sub.a;
            uint8_t psw = (state->AF.sub.f.z |
                           state->AF.sub.f.s << 1 |
                           state->AF.sub.f.p << 2 |
                           state->AF.sub.f.c << 3 |
                           state->AF.sub.f.ac << 4);
            state->memory[state->SP.sp - 2] = psw;
            state->SP.sp = state->SP.sp - 2;
            break;
        }
        case 0xF6: // ORI byte
        {
            uint8_t x = state->AF.sub.a | opcode[1];
            state->AF.sub.f.z = (x == 0);
            state->AF.sub.f.s = ((x & 0x80) == 0x80);
            state->AF.sub.f.p = parity(x, 8);
            state->AF.sub.f.c = 0; // Data book says op clears carry flags
            state->AF.sub.f.ac = 0;
            state->AF.sub.a = x;
            state->PC.pc++; // for the data byte
            break;
        }
        case 0xF7: // RST 6
        {
            uint16_t ret = state->PC.pc + 2;
            state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
            state->memory[state->SP.sp - 2] = (ret & 0xff);
            state->SP.sp = state->SP.sp - 2;
            state->PC.pc = 0x30;
            break;
        }
        case 0xF8: // RM
            if (state->AF.sub.f.s == 1) {
                state->PC.pc = state->memory[state->SP.sp] | (state->memory[state->SP.sp + 1] << 8);
                state->SP.sp += 2;
            }
            break;
        case 0xF9: // SPHL
            state->SP.sp = (state->HL.sub.h << 8) | (state->HL.sub.l);
            break;
        case 0xFA: // JM address
            if (state->AF.sub.f.s == 1)
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            else
                // branch not taken
                state->PC.pc += 2;
            break;
        case 0xFB: // EI
            state->int_enable = 1;
            break;
        case 0xFC: // CM address
        {
            if (state->AF.sub.f.s == 1) {
                uint16_t ret = state->PC.pc + 2;
                state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
                state->memory[state->SP.sp - 2] = (ret & 0xff);
                state->SP.sp = state->SP.sp - 2;
                state->PC.pc = (opcode[2] << 8) | opcode[1];
            } else
                state->PC.pc += 2;
            break;
        }
        case 0xFE: // CPI byte
        {
            uint16_t res = (uint16_t) state->AF.sub.a - (uint16_t) opcode[1];
            state->AF.sub.f.z = ((res & 0xff) == 0);
            state->AF.sub.f.s = ((res & 0x80) == 0x80);
            state->AF.sub.f.c = (res > 0xff);
            state->AF.sub.f.p = parity(res & 0xff, 8);
            state->AF.sub.f.ac = (((state->AF.sub.a & 0xf) + (-(state->BC.sub.c) & 0xf)) > 0xf);
            state->PC.pc++; // for the data byte
            break;
        }
        case 0xFF: // RST 7
        {
            uint16_t ret = state->PC.pc + 2;
            state->memory[state->SP.sp - 1] = (ret >> 8) & 0xff;
            state->memory[state->SP.sp - 2] = (ret & 0xff);
            state->SP.sp = state->SP.sp - 2;
            state->PC.pc = 0x38;
            break;
        }
        default:
            return; //NOP
    }
}