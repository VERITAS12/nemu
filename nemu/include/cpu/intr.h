#ifndef INTR_H
#define INTR_H
#include <setjmp.h>
#include "common.h"
extern jmp_buf jbuf;

void raise_intr(uint8_t NO){
	/* save EFLAGS CS EIP */
	reg_l(R_ESP) -= 4, swaddr_write (reg_l(R_ESP), 4, cpu.eflags, R_SS);
	reg_l(R_ESP) -= 4, swaddr_write (reg_l(R_ESP), 4, cpu.SR[R_CS].val, R_SS);
	reg_l(R_ESP) -= 4, swaddr_write (reg_l(R_ESP), 4, cpu.eip, R_SS);
	/* get selector and offset */
	uint32_t addr = cpu.IDTR.base + NO * 8;
	GateDesc gd;
	gd.high = lnaddr_read(addr, 4);
	gd.low = lnaddr_read(addr + 4, 4);
	uint16_t Selector = gd.segment;
	uint32_t off_set = (gd.offset_31_16 << 16) | (gd.offset_15_0);
	cpu.SR[R_CS].val = Selector;
	load_sr_cache(R_CS);
	cpu.eip = off_set;
	longjmp(jbuf, 1);
}
#endif
