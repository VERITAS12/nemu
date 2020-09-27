#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute(){
	DATA_TYPE result = op_dest->val - op_src->val;
	int len = (DATA_BYTE << 3) - 1;
	int i=0, cnt=0;
	for(;i<8;i++)if(((1<<i)&result)!=0)cnt++;
	cpu.CF = op_dest->val < op_src->val;
	cpu.PF = !(cnt%2);
	cpu.ZF = !result;
	cpu.SF = result >> len;
	OPERAND_W(op_dest, result);
	cpu.OF = (op_src->val>>len)!=(op_dest->val>>len)&&(cpu.SF==(op_src->val>>len));
	print_asm(str(instr)" %s, %s", op_src->str, op_dest->str);
}
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)
#include "cpu/exec/template-end.h"
