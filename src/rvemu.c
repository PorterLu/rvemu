#include <stdio.h>
#include "rvemu.h"

int main(int argc, char *argv[]) {
    printf("Hello, World\n");
    assert(argc > 1);

    machine_t machine = {0};
    machine_load_program(&machine, argv[1]); //read elf, and set init pc 

    printf("alloc_base: %lx\n", machine.mmu.base);
    printf("host alloc_base: %lx\n", machine.mmu.host_alloc);
    return 0;
}

