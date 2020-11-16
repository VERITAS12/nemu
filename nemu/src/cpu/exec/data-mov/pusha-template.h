#include "cpu/exec/template-start.h"

#define instr pusha

make_helper(concat(pusha_, SUFFIX)){
	if(DATA_BYTE == 2){
		uint16_t temp = reg_l(R_ESP);
		int i;
		for(i = 0;i < 8 ;i++)
			if(i == 4)reg_w(R_ESP) -= 4, swaddr_write (reg_w(R_ESP), 4, temp, R_SS);
			else reg_w(R_ESP) -= 4, swaddr_write (reg_w(R_ESP), 4, reg_w(i), R_SS);
	}else{
		uint32_t temp = reg_l(R_ESP);
		int i;
		for(i = 0;i < 8 ;i++)
			if(i == 4)reg_l(R_ESP) -= 4, swaddr_write (reg_l(R_ESP), 4, temp, R_SS);
			else reg_l(R_ESP) -= 4, swaddr_write (reg_l(R_ESP), 4, reg_l(i), R_SS);
	}
	print_asm("pusha");
	return 1;
}


#include "cpu/exec/template-end.h"
