#include "cpu/exec/template-start.h"

#define instr jge

static void do_execute(){
	DATA_TYPE_S disp = op_src->val;
	if(cpu.ZF==1||cpu.OF==cpu.SF)cpu.eip+=disp;
	print_asm_template1();
}
make_instr_helper(i);
#include "cpu/exec/template-end.h"