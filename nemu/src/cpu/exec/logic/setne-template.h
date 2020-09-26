#include "cpu/exec/template-start.h"

#define instr setne

static void do_execute(){
	OPERAND_W(op_src,0);
	print_asm(str(instr)" %s", op_src->str);
}
make_instr_helper(rm)

#include "cpu/exec/template-end.h"
