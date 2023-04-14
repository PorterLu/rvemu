#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include "types.h"
#include "elfdef.h"

#define fatalf(fmt, ...) (fprintf(stderr, "fatal: %s:%d " fmt "\n", __FILE__, __LINE__, __VA_ARGS__), exit(1))
#define fatal(msg) fatalf("%s", msg)

#define ROUNDDOWN(x, k) ((x) & -(k))          // final result will biggest n*k <= x
#define ROUNDUP(x, k) (((x) + (k)-1) & -(k))  // final result will least n*k >= x
#define MIN(x, y) ((y) > (x) ? (x) : (y))     // get the smaller one from x and y
#define MAX(x, y) ((y) < (x) ? (x) : (y))     // get the bigger one from x and y

#define GUEST_MEMORY_OFFSET 0x088800000000ULL // get the memory base

#define TO_HOST(addr)   (addr + GUEST_MEMORY_OFFSET)  // convert it to a real address on machine
#define TO_GUEST(addr)  (addr - GUEST_MEMORY_OFFSET)  // convert it to a index in a array

/**
 * mmu.c
 * mmu: memory management unit
*/
typedef struct {
    u64 entry;
    u64 host_alloc;
    u64 alloc;
    u64 base;
} mmu_t;

void mmu_load_elf(mmu_t *, int);

/**
 * state.c
*/
typedef struct {
    u64 gp_regs[32];
    u64 pc;
} state_t;

/**
 * machine.c
*/
typedef struct {
    state_t state;
    mmu_t mmu;
} machine_t;

void machine_load_program(machine_t *, char *);

