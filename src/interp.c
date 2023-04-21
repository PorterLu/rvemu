#include "rvemu.h"

typedef void(func_t)(state_t *, insn_t *);

static func_t *funcs[] = {};

/*
*   while(true){
*       read_inst
*       decode
*       break out to handle?
*       update pc
*   }
*/

void exec_block_interp(state_t *state) {
    static insn_t insn = {0};
    while(true) {
        u32 data = *(u32 *)TO_HOST(state->pc);
        insn_decode(&insn, data);

        funcs[insn.type](state, &insn);

        state->gp_regs[zero] = 0;

        if (insn.cont) break;

        state->pc += insn.rvc ? 2 : 4;
    }
} 