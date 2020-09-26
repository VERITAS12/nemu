#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute(){
	DATA_TYPE_S disp = op_src->val;
	if (op_src->type == OP_TYPE_REG || op_src->type == OP_TYPE_MEM){
		cpu.eip = disp - concat(decode_rm_, SUFFIX)(cpu.eip + 1);
		print_asm(str(instr)" %s", op_src->str);
	}else{
		print_asm("jmp 0x%x", cpu.eip + 1 + DATA_BYTE +disp);
		cpu.eip += disp;
	}
	
}
make_instr_helper(i)
make_instr_helper(rm)
#include "cpu/exec/template-end.h"
