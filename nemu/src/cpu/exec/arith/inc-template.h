#include "cpu/exec/template-start.h"

#define instr inc

static void do_execute () {
	DATA_TYPE result = op_src->val + 1;
	OPERAND_W(op_src, result);

	/* TODO: Update EFLAGS. */
	// panic("please implement me");
	int len = (DATA_BYTE << 3) - 1;
	cpu.OF = (result>op_dest->val);
	cpu.SF = result>>len;
	cpu.ZF = !result;
	int i=0, cnt =0;
	for(;i<8;i++)if(((1<<i)&result)!=0)cnt++;
	cpu.PF = !(cnt%2);
	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
// #include "cpu/exec/template-start.h"

// #define instr inc

// static void do_execute () {
// 	DATA_TYPE result = op_src->val + 1;
// 	OPERAND_W(op_src, result);

// 	/* TODO: Update EFLAGS. */
// 	int d1 = 1;
// 	int d2 = op_src->val;
// 	int ans = (int)result;
// 	cpu.ZF = !ans;
// 	cpu.SF = ans<0?1:0;
// 	int cnt =0;
// 	while(result) {
// 		result = result&(result-1);
// 		cnt++;
// 	}	
// 	cpu.PF = cnt%2==0? 1:0;
// 	if((d1>0&&d2>0&&ans<=0)||(d2<0&&d1<0&&ans>=0)) cpu.OF = 1;
// 	else cpu.OF = 0;

// 	print_asm_template1();
// }

// make_instr_helper(rm)
// #if DATA_BYTE == 2 || DATA_BYTE == 4
// make_instr_helper(r)
// #endif

// #include "cpu/exec/template-end.h"