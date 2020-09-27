// #include "cpu/exec/template-start.h"

// #define instr sbb

// static void do_execute(){
// 	int val1 = (int)op_src->val;
// 	int val2 = op_dest->val;
// 	val1 = val1 +cpu.CF;
// 	int result = val2 - val1;
// 	cpu.ZF = !result;
// 	cpu.SF = result < 0?1:0;
// 	DATA_TYPE tmp = op_dest->val - cpu.CF - op_src->val;
// 	int cnt = 0;
// 	while(tmp){
// 		tmp &=(tmp-1);
// 		cnt++;
// 	}
// 	cpu.PF = !(cnt%2);
// 	if((result>0&&val1>0&&val2<=0)||(result<0&&val1<0&&val2>=0))cpu.OF=1;
// 	else cpu.OF=0;
// 	cpu.CF = (unsigned int)val2 < (unsigned int)val1;
// 	OPERAND_W(op_dest, result);

// }
// #if DATA_BYTE==2||DATA_BYTE==4
// make_instr_helper(si2rm)
// #endif
// make_instr_helper(rm2r)
// make_instr_helper(i2rm)
// make_instr_helper(r2rm)
// #include "cpu/exec/template-end.h"
#include "cpu/exec/template-start.h"

#define instr sbb

static void do_execute() {
	int d1 = (int)op_src->val;
	int d2 = op_dest->val;
	d1 = d1+cpu.CF;
	int ans = d2-d1;
	cpu.ZF = !ans;
	cpu.SF = ans<0?1:0;
 	DATA_TYPE n =op_dest->val-cpu.CF-op_src->val;
	int cnt =0;
	while(n) {
		n = n&(n-1);
		cnt++;
	}	
	cpu.PF = cnt%2==0? 1:0;
	if((ans>0&&d1>0&&d2<=0)||(ans<0&&d1<0&&d2>=0)) cpu.OF = 1;
	else cpu.OF = 0;
	cpu.CF = (unsigned int)d2< (unsigned int)d1;	
	OPERAND_W(op_dest,ans);
}
#if DATA_BYTE == 2 || DATA_BYTE==4
make_instr_helper(si2rm);
#endif
make_instr_helper(rm2r);
make_instr_helper(i2rm);
make_instr_helper(r2rm);
#include "cpu/exec/template-end.h"