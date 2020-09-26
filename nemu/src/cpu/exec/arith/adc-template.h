#include "cpu/exec/template-start.h"

#define instr adc
static void do_execute(){
	DATA_TYPE result = op_dest->val + op_src->val  + cpu.ZF;
	printf("adc:  %d\n", result);
	int len = (DATA_BYTE<<3)-1;
	cpu.CF = result < op_dest->val;
	cpu.SF = result>>len;
	cpu.OF = ((op_dest->val>>len)==(op_src->val>>len))&&((op_dest->val>>len)!=cpu.SF);
	cpu.ZF =!result;
	OPERAND_W(op_dest, result);
	int i=0,cnt=0;
	for(;i<8;i++)if(((1<<i)&result)!=0)cnt++;
	cpu.PF=!(cnt%2);
	print_asm(str(instr)" %s, %s",op_src->str,op_dest->str);
}
#if DATA_BYTE==2||DATA_BYTE==4
make_instr_helper(si2rm)
#endif
make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)
#include "cpu/exec/template-end.h"
