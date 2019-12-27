#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define __CELL_ASSERT__
#include <assert.h>
#include <sys/syscall.h>
#include <sys/gpio.h>
#include <cell.h>
#include <ppu_asm_intrinsics.h>
#include <sstream>
#include <string>

#define HEXD_SGN(x) ((x.i & 0x8000000000000000ull) >> 63)
#define HEXD_EXP(x) ((x.i & 0x7FF0000000000000ull) >> 52)
#define HEXD_MAN(x) ((x.i & 0x000FFFFFFFFFFFFFull))

union hexd {
    uint64_t i;
	double f;
};

double manual_frsqrte(double f) {
    static const uint64_t mantissas[16] = {
        0x000F100000000000ull,
        0x000D800000000000ull,
        0x000C000000000000ull,
        0x000A800000000000ull,
        0x0009800000000000ull,
        0x0008800000000000ull,
        0x0008000000000000ull,
        0x0007000000000000ull,
        0x0006000000000000ull,
        0x0004C00000000000ull,
        0x0003C00000000000ull,
        0x0003000000000000ull,
        0x0002000000000000ull,
        0x0001800000000000ull,
        0x0001000000000000ull,
        0x0000800000000000ull
    };

    hexd result;
    result.f = f;

    // take 3 most significant bits of mantissa
    // if least significant bit of exponent is not set, add 8
    // this results in the idx to be used in the output mantissa array
    const uint64_t idx = ((HEXD_MAN(result) >> 49) & 7ull) + (!(HEXD_EXP(result) & 1) << 3);

    // example of exponent deriving procedure:
    // 0x41B - 0x3FF = 0x1C
    // 0x1C >> 1 = 0xE
    // 0x0E + 1 = 0xF
    // 0x3FF - 0xF = 0x3F0
    const uint64_t new_exp = (0x3FF - (((HEXD_EXP(result) - 0x3FF) >> 1) + 1)) << 52;

    // clear exponent bits and add new ones
    result.i &= ~0x7FF0000000000000ull;
    result.i |= new_exp;

    // clear mantissa bits and add new ones
    result.i &= ~0x000FFFFFFFFFFFFFull;
    result.i |= mantissas[idx];

    return result.f;
}

int main(void) {
    // yea
   	void* failed_buf = malloc(65536);

	printf("Hello, world!\n\n");

    hexd input;
	hexd output;
    hexd last_eq_input;
    hexd last_input;
    hexd last_output;

    last_eq_input.f = 0.0;
    last_input.f = 0.0;
    last_output.f = 0.0;

    bool first = true;
    
    // change this as fit
    for(input.f = 0.0000; input.f < 100000000000.01000; input.f += 10000) {
        output.f = manual_frsqrte(input.f); // can also change to __frsqrte for comparison
        if(output.i != last_output.i) {
            if(!first) {
                printf("%f [%016llX] to %f [%016llX] -- %f [%016llX]\n", last_eq_input.f, last_eq_input.i, last_input.f, last_input.i, last_output.f, last_output.i);
            }
            else {
                first = false;
            }
            
            last_eq_input = input;
            last_output = output;
        }

        last_input = input;
    }

    printf("\nDone!\n");

	free(failed_buf);
	return 0;
}