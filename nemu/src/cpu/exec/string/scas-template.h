#include "cpu/exec/template-start.h"
#define instr scas

make_helper(concat(scas_l_, SUFFIX)){
	int val1, val2;
	if(DATA_BYTE == 2){
		val1 = swaddr_read(reg_w(R_DI), 2, R_ES);
		val2 = reg_w(R_AX);
		if(cpu.DF==0)reg_w(R_DI)+=DATA_BYTE;
		else reg_w(R_DI)-=DATA_BYTE;
	}else{
		val1 = swaddr_read(reg_l(R_EDI), 4, R_ES);
		val2 = reg_l(R_EAX);
		if(DATA_BYTE==1){val1&=0xff;val2&=0xff;}
		if(cpu.DF==0)reg_l(R_EDI)+=DATA_BYTE;
		else reg_l(R_EDI)-=DATA_BYTE;
	}
	print_asm_template2();
	int result = val2-val1;
	cpu.ZF = !result;
	cpu.SF=result<0;
	cpu.OF= (result>0&&val1>0&&val2<=0)||(result<0&&val1<0&&val2>=0);
	int i = 0, cnt = 0;
	for(;i<8;i++)if(((1<<i)&result)!=0)cnt++;
	cpu.PF = !(cnt%2);
	cpu.CF = (unsigned int)val2 < (unsigned int)val1;
	return 1;
}
#include "cpu/exec/template-end.h"
