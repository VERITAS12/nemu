#include "cpu/exec/template-start.h"

#define instr iret

make_helper(concat(iret_, SUFFIX)){
	cpu.eip = swaddr_read(reg_l(R_ESP), 4, R_SS), reg_l(R_ESP) += 4;
	cpu.SR[R_CS].val = swaddr_read(reg_l(R_ESP), 4, R_SS), reg_l(R_ESP) += 4;
	load_sr_cache(R_CS);
	cpu.eflags = swaddr_read(reg_l(R_ESP), 4, R_SS), reg_l(R_ESP) += 4;	
	print_asm("iret");	
	return 1;
}

#include "cpu/exec/template-end.h"
