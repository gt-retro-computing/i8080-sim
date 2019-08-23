#include <iostream>

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
            flags f;
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

int main() {
    i8080_state state{};
    printf("SysSize: %li bytes\n", sizeof(state));

    static_assert(sizeof(state.AF) == 2, "");
    static_assert(sizeof(state.BC) == 2, "");
    static_assert(sizeof(state.DE) == 2, "");
    static_assert(sizeof(state.HL) == 2, "");
    static_assert(sizeof(state.SP) == 2, "");
    static_assert(sizeof(state.PC) == 2, "");

    printf("AF=0x%04X\n", state.AF.af);

    return 0;
}