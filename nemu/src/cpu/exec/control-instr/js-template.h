#include "cpu/exec/template-start.h"

#define instr js

static void do_execute(){
	DATA_TYPE_S d = op_src->val;
	if(cpu.SF==1)cpu.eip+=d;
	print_asm_template1();
}
make_instr_helper(i);
#include "cpu/exec/template-end.h"
