#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_n_, SUFFIX)){
	cpu.eip = MEM_R (reg_l(R_ESP));
	if(DATA_BYTE == 2)cpu.eip &= 0xffff;
	reg_l(R_ESP) += DATA_BYTE;
	print_asm("ret");
	return 1;
}
make_helper(concat(ret_i_, SUFFIX)){
	
	return 1;
}

#include "cpu/exec/template-end.h"
