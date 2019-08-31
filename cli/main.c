//
// Created by Will Gulian on 8/28/19.
//

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "core/i8080_state.h"
#include "core/i8080_asm.h"
#include "core/hex.h"

enum OutState {
    OUT_NONE = 0,
    OUT_HALT,
    OUT_PRINT,
    OUT_PRINTBIN,
    OUT_PRINTBININV,
};

void exit_usage(char *name) {
    fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", name);
    exit(EXIT_FAILURE);
}

void print_bin(uint8_t value) {
    char foo[9];
    uint8_t mask = 0x80;
    for (int i = 0; i < 8; i++) {
        foo[i] = ((value & mask) != 0) ? 'X' : '-';
        mask /= 2;
    }
    foo[8] = '\0';
    printf("%s", foo);
}

static enum OutState out_state = OUT_NONE;
static struct i8080_state state;
static struct hex_data hex_data;

static void out_port_print(uint8_t port, uint8_t value) {
    switch (out_state) {
        case OUT_HALT:
            state.halt = true;
            // fallthrough
        case OUT_PRINT:
            printf("out(0x%02x) => 0x%02x (%d)\n", port, value, value);
            break;
        case OUT_PRINTBIN:
            printf("out(0x%02x) => ", port);
            print_bin(value);
            printf("\n");
            break;
        case OUT_PRINTBININV:
            printf("out(0x%02x) => ", port);
            print_bin(value ^ 0xffu);
            printf("\n");
            break;
        default: // OUT_NONE
            break;
    }
}

int main(int argc, char **argv) {
    // initialize 8080 emulator
    memset(&state, 0, sizeof(struct i8080_state));
    state.skip_lock = 1;
    state.out_port = out_port_print;

    int opt;
    while ((opt = getopt(argc, argv, "p:d:f:s:T:R:A:O:P")) != -1) {
        switch (opt) {
            case 'p': // set start address
                state.pc = atoi(optarg);
                break;
            case 'd': { // specify hex string on command line
                int len = strlen(optarg);

                int result = hex_read_string(&hex_data, optarg, len);
                if (result < 0) {
                    fprintf(stderr, "Invalid hex string for -d arg\n");
                    exit(1);
                }

                // written with two counters so we wrap memory.
                for (uint16_t addr = hex_data.start_addr, i = 0; i < hex_data.length; addr++, i++) {
                    state.memory[addr] = hex_data.buf[i];
                }

                break;
            }
            case 'f': { // read hex strings from file
                FILE *f = fopen(optarg, "r");
                if (f == NULL) {
                    fprintf(stderr, "Failed to open file \n");
                    exit(1);
                }

                char *line = NULL;
                size_t len = 0;
                ssize_t read;
                int line_count = 0;

                while ((read = getline(&line, &len, f)) != -1) {
                    line_count++;

                    if (read == 0 || line[0] != ':') {
                        continue;
                    }

                    int result = hex_read_string(&hex_data, line, len);
                    if (result < 0) {
                        fprintf(stderr, "Invalid hex string on line %d of %s\n", line_count, optarg);
                        exit(1);
                    }

                    // written with two counters so we wrap memory.
                    for (uint16_t addr = hex_data.start_addr, i = 0; i < hex_data.length; addr++, i++) {
                        state.memory[addr] = hex_data.buf[i];
                    }

                    fprintf(stderr, "%s:%d - Wrote %d bytes\n", optarg, line_count, hex_data.length);
                }

                fclose(f);
                free(line);
                printf("read file\n");
                break;
            }
            case 's': { // step n instructions
                state.halt = false;

                int step_count = atoi(optarg);

                for (int i = 0; i < step_count; i++) {
                    gwemu_exec_step(&state);
                    if (state.halt) {
                        break;
                    }
                }

                break;
            }
            case 'R': { // run for n seconds
                state.halt = false;
                double timeout_s = atof(optarg);

                clock_t start = clock();
                clock_t target_end = start + (clock_t) (timeout_s * CLOCKS_PER_SEC);
                clock_t end = 0;

                unsigned long instruction_count = 0;

                while (!state.halt) {
                    for (int i = 0; i < 10000; i++) {
                        gwemu_exec_step(&state);
                        instruction_count++;
                    }

                    clock_t curr = clock();
                    if (curr >= target_end) {
                        end = curr;
                        break;
                    }
                }

                // in case of exit by halt
                if (end == 0) {
                    end = clock();
                }

                double total_time = ((double) (end - start)) / CLOCKS_PER_SEC;
                double ips = ((double) instruction_count) / total_time;

                fprintf(stderr, "Executed %f ips in %f seconds\n", ips, total_time);
                break;
            }
            case 'P': {

                printf(" A=0x%02x\n", state.a);

                int bc = (state.b << 8u) | state.c;
                int de = (state.d << 8u) | state.e;
                int hl = (state.h << 8u) | state.l;

                printf("BC=0x%04x (%d)\n", bc, bc);
                printf("DE=0x%04x (%d)\n", de, de);
                printf("HL=0x%04x (%d)\n", hl, hl);

                break;
            }
            case 'A': {
                uint8_t expected = (uint8_t) atoi(optarg);

                if (state.a != expected) {
                    fprintf(stderr, "-A %d but a=%d\n", expected, state.a);
                    exit(2);
                }

                break;
            }
            case 'O': {

                if (strcmp(optarg, "none") == 0) {
                    out_state = OUT_NONE;
                    break;
                }

                if (strcmp(optarg, "halt") == 0) {
                    out_state = OUT_HALT;
                    break;
                }

                if (strcmp(optarg, "print") == 0) {
                    out_state = OUT_PRINT;
                    break;
                }
                if (strcmp(optarg, "printbin") == 0) {
                    out_state = OUT_PRINTBIN;
                    break;
                }

                if (strcmp(optarg, "printbininv") == 0) {
                    out_state = OUT_PRINTBININV;
                    break;
                }

                fprintf(stderr, "unknown arg for -O: %s\n", optarg);
                exit(2);
            }
            default: /* '?' */
                exit_usage(argv[0]);
        }
    }

    gwemu_print_memory(stdout, &state);
    exit(EXIT_SUCCESS);

}


