#include "cpu/exec/template-start.h"

#define instr dec

static void do_execute () {
	DATA_TYPE result = op_src->val - 1;
	OPERAND_W(op_src, result);

	/* TODO: Update EFLAGS. */
	//panic("please implement me");
	int val1=1, val2 = op_src->val;
	int ans = (int)result;
	cpu.ZF = !result;
	cpu.SF = result>>((DATA_BYTE<<3)-1);
	int i=0,cnt=0;
	for(;i<8;i++)if(((1<<i)&result))cnt++;
	cpu.PF = !(cnt%2);
	cpu.OF = (ans>0&&val1>0&&val2<=0)||(ans<0&&val1<0&&val2>=0);
	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
