#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute(){
	DATA_TYPE result = op_dest->val - op_src->val;

	int i=0, cnt = 0;
	for(;i<8;i++)if(((1<<i)&result)!=0)cnt++;
	cpu.PF = !(cnt%2);
	cpu.CF = op_dest->val<op_src->val;
	cpu.ZF = !result;
	printf("cmp: --------------\n");
	printf("val:  %dval2:  %d\n", op_dest->val, op_src->val);
	printf("ZF:      %d\n", cpu.ZF);
	cpu.SF = result>>((DATA_BYTE<<3)-1);
	int len = (DATA_BYTE<<3)-1;
	cpu.OF = (op_dest->val>>len)!=(op_src->val>>len)&&(op_src->val>>len)==cpu.SF;
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
