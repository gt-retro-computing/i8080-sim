//
// Created by Will Gulian on 8/31/19.
//

#include <stdio.h>
#include "i8080_asm.h"
#include "i8080_state.h"


static char *decode_register_pair(uint8_t opcode) {
    switch ((0b110000u & opcode) >> 4u) {
        case 0:
            return "BC";
        case 1:
            return "DE";
        case 2:
            return "HL";
        case 3:
            return "SP";
        default:
            return "";
    }
}

static char *decode_condition(uint8_t opcode) {
    switch ((0b111000u & opcode) >> 3u) {
        case 0:
            return "nz";
        case 1:
            return "z";
        case 2:
            return "nc";
        case 3:
            return "c";
        case 4:
            return "po";
        case 5:
            return "pe";
        case 6:
            return "p";
        case 7:
            return "m";
        default:
            return "";
    }
}

static char *decode_raw_register(uint8_t idx) {
    switch (idx) {
        case 0:
            return "B";
        case 1:
            return "C";
        case 2:
            return "D";
        case 3:
            return "E";
        case 4:
            return "H";
        case 5:
            return "L";
        case 6:
            return "M";
        case 7:
            return "A";
        default:
            return "";
    }
}

#define decode_dst_register(opcode) decode_raw_register(((opcode) & 0b00111000u) >> 3u)
#define decode_src_register(opcode) decode_raw_register(((opcode) & 0b00000111u))


uint16_t gwemu_sprint_asm(char *buffer, uint8_t *opcode) {

    // no operand codes
    switch (opcode[0]) {
        case 0:
            sprintf(buffer, "nop");
            return 1;
        case 0b01110110:
            sprintf(buffer, "hlt");
            return 1;
        case 0b11101011:
            sprintf(buffer, "xchg");
            return 1;
        case 0b00000111:
            sprintf(buffer, "rlc");
            return 1;
        case 0b00001111:
            sprintf(buffer, "rrc");
            return 1;
        case 0b00010111:
            sprintf(buffer, "ral");
            return 1;
        case 0b00011111:
            sprintf(buffer, "rar");
            return 1;
        case 0b00100111:
            sprintf(buffer, "daa");
            return 1;
        case 0b00101111:
            sprintf(buffer, "cma");
            return 1;
        case 0b00110111:
            sprintf(buffer, "stc");
            return 1;
        case 0b00111111:
            sprintf(buffer, "cmc");
            return 1;
        case 0b11001001:
            sprintf(buffer, "ret");
            return 1;
        case 0b11101001:
            sprintf(buffer, "pchl");
            return 1;
        case 0b11100011:
            sprintf(buffer, "xthl");
            return 1;
        case 0b11111001:
            sprintf(buffer, "sphl");
            return 1;
        case 0b11111011:
            sprintf(buffer, "ei");
            return 1;
        case 0b11110011:
            sprintf(buffer, "di");
            return 1;
    }

    // one operand codes
    switch (opcode[0]) {
        case 0b11011011:
            sprintf(buffer, "in 0x%02x", opcode[1]);
            return 2;
        case 0b11010011:
            sprintf(buffer, "out 0x%02x", opcode[1]);
            return 2;
        case 0b11000110:
            sprintf(buffer, "adi 0x%02x", opcode[1]);
            return 2;
        case 0b11001110:
            sprintf(buffer, "aci 0x%02x", opcode[1]);
            return 2;
        case 0b11010110:
            sprintf(buffer, "sui 0x%02x", opcode[1]);
            return 2;
        case 0b11011110:
            sprintf(buffer, "sbi 0x%02x", opcode[1]);
            return 2;
        case 0b11100110:
            sprintf(buffer, "ani 0x%02x", opcode[1]);
            return 2;
        case 0b11110110:
            sprintf(buffer, "ori 0x%02x", opcode[1]);
            return 2;
        case 0b11101110:
            sprintf(buffer, "xri 0x%02x", opcode[1]);
            return 2;
        case 0b11111110:
            sprintf(buffer, "cpi 0x%02x", opcode[1]);
            return 2;
    }

    // two operand codes
    switch (opcode[0]) {
        case 0b11000011:
            sprintf(buffer, "jmp 0x%04x", opcode[1] | (uint16_t) (opcode[2] << 8u));
            return 3;
        case 0b11001101:
            sprintf(buffer, "call 0x%04x", opcode[1] | (uint16_t) (opcode[2] << 8u));
            return 3;
        case 0b00111010:
            sprintf(buffer, "lda 0x%04x", opcode[1] | (uint16_t) (opcode[2] << 8u));
            return 3;
        case 0b00110010:
            sprintf(buffer, "sta 0x%04x", opcode[1] | (uint16_t) (opcode[2] << 8u));
            return 3;
        case 0b00101010:
            sprintf(buffer, "lhld 0x%04x", opcode[1] | (uint16_t) (opcode[2] << 8u));
            return 3;
        case 0b00100010:
            sprintf(buffer, "shld 0x%04x", opcode[1] | (uint16_t) (opcode[2] << 8u));
            return 3;
    }

    // register pair opcodes
    switch (opcode[0] & 0b11001111u) {
        case 0b00000001:
            sprintf(buffer, "lxi %s, 0x%04x", decode_register_pair(opcode[0]), opcode[1] | (uint16_t) (opcode[2] << 8u));
            return 3;
        case 0b00001010:
            sprintf(buffer, "ldax %s", decode_register_pair(opcode[0]));
            return 1;
        case 0b00000010:
            sprintf(buffer, "stax %s", decode_register_pair(opcode[0]));
            return 1;
        case 0b00000011:
            sprintf(buffer, "inx %s", decode_register_pair(opcode[0]));
            return 1;
        case 0b00001011:
            sprintf(buffer, "dcx %s", decode_register_pair(opcode[0]));
            return 1;
        case 0b00001001:
            sprintf(buffer, "dad %s", decode_register_pair(opcode[0]));
            return 1;
        case 0b11000101:
            sprintf(buffer, "push %s", decode_register_pair(opcode[0]));
            return 1;
        case 0b11000001:
            sprintf(buffer, "pop %s", decode_register_pair(opcode[0]));
            return 1;
    }

    // mov is the only 2 register opcode
    if ((opcode[0] & 0b11000000u) == 0b01000000u) {
        sprintf(buffer, "mov %s, %s", decode_dst_register(opcode[0]), decode_src_register(opcode[0]));
        return 1;
    }

    // rst has a unique encoding
    if ((opcode[0] & 0b11000111u) == 0b11000111u) {
        sprintf(buffer, "rst 0x%02x", (opcode[0] & 0b111000u) >> 3u);
        return 1;
    }
    
    // single destination register opcodes
    switch (opcode[0] & 0b11000111u) {
        case 0b00000100:
            sprintf(buffer, "inr %s", decode_dst_register(opcode[0]));
            return 1;
        case 0b00000101:
            sprintf(buffer, "dcr %s", decode_dst_register(opcode[0]));
            return 1;
        case 0b00000110:
            sprintf(buffer, "mvi %s, 0x%02x", decode_dst_register(opcode[0]), opcode[1]);
            return 2;
    }
    
    // single register opcodes
    switch (opcode[0] & 0b11111000u) {
        case 0b10000000:
            sprintf(buffer, "add %s", decode_src_register(opcode[0]));
            return 1;
        case 0b10001000:
            sprintf(buffer, "adc %s", decode_src_register(opcode[0]));
            return 1;
        case 0b10010000:
            sprintf(buffer, "sub %s", decode_src_register(opcode[0]));
            return 1;
        case 0b10011000:
            sprintf(buffer, "sbb %s", decode_src_register(opcode[0]));
            return 1;
        case 0b10100000:
            sprintf(buffer, "ana %s", decode_src_register(opcode[0]));
            return 1;
        case 0b10110000:
            sprintf(buffer, "ora %s", decode_src_register(opcode[0]));
            return 1;
        case 0b10101000:
            sprintf(buffer, "xra %s", decode_src_register(opcode[0]));
            return 1;
        case 0b10111000:
            sprintf(buffer, "cmp %s", decode_src_register(opcode[0]));
            return 1;
    }

    // condition code opcodes
    switch (opcode[0] & 0b11000111u) {
        case 0b11000010:
            sprintf(buffer, "j%s, 0x%04x", decode_condition(opcode[0]), opcode[1] | (uint16_t) (opcode[2] << 8u));
            return 3;
        case 0b11000100:
            sprintf(buffer, "c%s, 0x%04x", decode_condition(opcode[0]), opcode[1] | (uint16_t) (opcode[2] << 8u));
            return 3;
        case 0b11000000:
            sprintf(buffer, "r%s", decode_condition(opcode[0]));
            return 1;
    }

    sprintf(buffer, "0x%02x", opcode[0]);
    return 1;
}

void gwemu_print_memory(FILE *f, struct i8080_state *state) {
    char asm_buf[200];
    uint16_t addr = 0;

    for (int i = 0; i < 100; i++) {
        uint16_t addr2 = addr;
        addr += gwemu_sprint_asm(asm_buf, &state->memory[addr]);

#if I8080_TRACING

        fprintf(f, "0x%04x\t%s\t\t%d\n", addr2, asm_buf, state->trace_mem[addr2].exec_count);
#else
        fprintf(f, "0x%04x\t%s\n", addr2, asm_buf);
#endif
    }

}







