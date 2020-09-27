#include "cpu/exec/template-start.h"
#define instr movs
make_helper(concat(movs_l_, SUFFIX)){
	if(DATA_BYTE==2){
		swaddr_write(reg_w(R_DI), 2, swaddr_read(reg_w(R_SI), 2));
		if(cpu.DF==0){
			reg_w(R_DI)+=DATA_BYTE;
			reg_w(R_SI)+=DATA_BYTE;
		}else{
			reg_w(R_DI)-=DATA_BYTE;
			reg_w(R_SI)-=DATA_BYTE;
		}
	}else{
		swaddr_write(reg_w(R_EDI), 4, swaddr_read(reg_w(R_ESI), 4));
		if(cpu.DF==0){
			reg_w(R_EDI)+=DATA_BYTE;
			reg_w(R_ESI)+=DATA_BYTE;
		}else{
			reg_w(R_EDI)-=DATA_BYTE;
			reg_w(R_ESI)-=DATA_BYTE;
		}
	}
	print_asm("movs");
	return 1;
}

#include "cpu/exec/template-end.h"