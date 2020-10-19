#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute(){
	DATA_TYPE_S disp = op_src->val;
	if (op_src->type == OP_TYPE_REG || op_src->type == OP_TYPE_MEM){
		cpu.eip = disp - concat(decode_rm_, SUFFIX)(cpu.eip + 1)-1;
		print_asm(str(instr)" %s", op_src->str);
	}else{
		print_asm("jmp 0x%x", cpu.eip + 1 + DATA_BYTE +disp);
		cpu.eip += disp;
	}
	
}
make_instr_helper(i)
make_instr_helper(rm)
#if DATA_BYTE == 4
make_helper(ljmp){
	uint32_t opcode1 = instr_fetch(eip + 1, 4) - 7;
	uint16_t opcode2 = instr_fetch(eip + 5, 2);
	cpu.eip = opcode1;
	cpu.CS.val = opcode2;
	print_asm("ljmp %x, 0x%x", opcode2, opcode1 + 7);
	return 7;
}
#endif
#include "cpu/exec/template-end.h"
