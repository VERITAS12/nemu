#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

make_helper(nop) {
	print_asm("nop");
	return 1;
}

make_helper(int3) {
	void do_int3();
	do_int3();
	print_asm("int3");

	return 1;
}

make_helper(lea) {
	ModR_M m;
	m.val = instr_fetch(eip + 1, 1);
	int len = load_addr(eip + 1, &m, op_src);
	reg_l(m.reg) = op_src->addr;

	print_asm("leal %s,%%%s", op_src->str, regsl[m.reg]);
	return 1 + len;
}
make_helper(lgdt) {
	decode_i_l(eip + 1);
	cpu.GDTR.base = op_src->val;
	decode_i_w(eip + 5);
	cpu.GDTR.limit = op_src->val;
	print_asm("lgdt 0x%x,0x%x", cpu.GDTR.base, cpu.GDTR.limit);
	return 7;
}
