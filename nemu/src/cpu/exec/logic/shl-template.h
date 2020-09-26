#include "cpu/exec/template-start.h"

#define instr shl

static void do_execute () {
	DATA_TYPE src = op_src->val;
	DATA_TYPE dest = op_dest->val;
	DATA_TYPE tmp = op_dest->val;
	uint8_t count = src & 0x1f;
	dest <<= count;
	OPERAND_W(op_dest, dest);

	/* TODO: Update EFLAGS. */
//	panic("please implement me");
	cpu.OF = (op_src->val>>((DATA_BYTE <<3 ) - 1))?1:0;
	cpu.CF = 0;
	cpu.ZF = !tmp;
	cpu.SF = tmp>>((DATA_BYTE <<3 ) - 1);
	int i=0,cnt=0;
	for(;i<8;i++)if(((1<<i)&tmp)!=0)cnt++;
	cpu.PF = !(cnt%2);
	
	print_asm_template2();
}

make_instr_helper(rm_1)
make_instr_helper(rm_cl)
make_instr_helper(rm_imm)

#include "cpu/exec/template-end.h"
