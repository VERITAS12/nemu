#include "cpu/exec/template-start.h"

#define instr lidt

void do_execute(){
	if(op_src->size == 2){
		cpu.IDTR.limit = lnaddr_read(op_src->addr, 2);
		cpu.IDTR.base = lnaddr_read(op_src->addr + 2, 3);
	}else if(op_src->size == 4){
		cpu.IDTR.limit = lnaddr_read(op_src->addr, 2);
		cpu.IDTR.base = lnaddr_read(op_src->addr + 2, 4); 
	}
	print_asm_template1();
}
make_instr_helper(rm);

#include "cpu/exec/template-end.h"
