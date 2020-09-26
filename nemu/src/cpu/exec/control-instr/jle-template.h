#include "cpu/exec/template-start.h"
#define instr jle
static void do_execute(){
	DATA_TYPE_S disp = op_src->val;
	print_asm("jle %x", cpu.eip + 1+DATA_BYTE+disp);
	if(cpu.ZF==1||cpu.SF!=cpu.OF)cpu.eip+=disp;
}	
make_instr_helper(i)
#include "cpu/exec/template-end.h"
