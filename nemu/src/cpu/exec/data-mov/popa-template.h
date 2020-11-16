#include "cpu/exec/template-start.h"

#define instr popa

make_helper(concat(popa_, SUFFIX)){
	if(DATA_BYTE == 2){
		int i;
		for(i = 7;i >= 0 ;i--){
			if(i == 4){reg_w(R_ESP) += 4; continue;}
			reg_w(i) = swaddr_read(reg_w(R_ESP), 4, R_SS);
			reg_l(R_ESP) += 4;
		}

	}else{
		int i;
		for(i = 7;i >= 0 ;i--){
			
			if(i == 4){reg_l(R_ESP) += 4; continue;}
			reg_l(i) = swaddr_read(reg_l(R_ESP), 4, R_SS);
			reg_l(R_ESP) += 4;
		}
	}
	print_asm("popa");
	return 1;
}


#include "cpu/exec/template-end.h"
