#include <stdio.h>
#include "rvemu.h"

int main(int argc, char *argv[]) {
    printf("Hello, World\n");
    assert(argc > 1);

    machine_t machine = {0};
    machine_load_program(&machine, argv[1]); //read elf, and set init pc 

    printf("entry: %lx\n", machine.mmu.entry);
    return 0;
}
