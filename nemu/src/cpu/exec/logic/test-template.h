#include "cpu/exec/template-start.h"

#define instr test

static void do_execute(){
	DATA_TYPE result = op_dest->val & op_src->val;
	cpu.CF = 0;
	cpu.ZF = !result;
	// printf("test: --------------\n");
	// printf("val:  %dval2:  %d\n", op_dest->val, op_src->val);
	// printf("ZF:      %d\n", cpu.ZF);
	cpu.SF = result >> ((DATA_BYTE << 3) - 1);
	int i;
	int cnt = 0;
	for(i = 0;i < 8;i++){
		if(((1<<i)&result)!=0)cnt++;
	}
	cpu.PF =!(cnt%2);
	print_asm(str(instr)" %s, %s", op_src->str, op_dest->str);
}
make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
#include "cpu/exec/template-end.h"
