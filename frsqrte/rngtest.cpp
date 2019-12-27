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
#include <time.h>
#include <string>

union hexd {
    uint64_t i;
	double f;
};

int main(void) {
    // yea
   	void* failed_buf = malloc(65536);

	printf("Hello, world!\n\n");

    hexd input;
	hexd output;
    
    srand(42069);

    // not an accurate benchmark, as the processor is obviously doing other stuff as well in the loop (like printing...)
    const clock_t start_time = clock();
    // 100k random doubles (produces about 17.5MB of output)
    for(int i = 0; i < 100000; i++) {
        input.i = 0;

        // doesn't randomize sign
        // this relies on RAND_MAX being 0x3FFFFFFF
        input.i |= (uint64_t)rand() << 33;
        input.i |= (uint64_t)rand() << 3;
        input.i |= (uint64_t)rand() & 7;

        output.f = __frsqrte(input.f);
        printf("%.9f [%016llX] -- %.9f [%016llX]\n", input.f, input.i, output.f, output.i);
    }
    const clock_t duration_msec = (clock() - start_time) / (CLOCKS_PER_SEC / 1000);
    
    printf("\nDone in %d.%03d seconds!\n", duration_msec / 1000, duration_msec % 1000);

	free(failed_buf);
	return 0;
}