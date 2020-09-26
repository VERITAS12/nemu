#include "cpu/exec/template-start.h"

#define instr xor

static void do_execute () {
	DATA_TYPE result = op_dest->val ^ op_src->val;
	OPERAND_W(op_dest, result);

	/* TODO: Update EFLAGS. */
	//panic("please implement me");
	cpu.ZF = !result;
	cpu.OF=0;
	cpu.SF = result>>((DATA_BYTE<<3)-1);
	cpu.CF = 0;
	int i=0,cnt=0;
	for(;i<8;i++)if(((1<<i)&result)!=0)cnt++;
	cpu.PF = !(cnt%2);
	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
