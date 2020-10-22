#include "common.h"
#include "../../include/cpu/reg.h"

uint32_t L1_read(hwaddr_t, size_t);
void L1_write(hwaddr_t, size_t, uint32_t);
uint32_t check_TLB(uint32_t, bool* );
void set_TLB(uint32_t , PTE);
uint32_t seg_translate(hwaddr_t addr, size_t len, uint8_t sreg){
	if(cpu.CR0.protect_enable){
		//printf("get me\n");
		assert(addr <= cpu.SR_cache[sreg].limit);
		return cpu.SR_cache[sreg].base + addr;
	}
	return addr;
}
/* Memory accessing interfaces */


uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	return L1_read(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	L1_write(addr, len, data);	
}
int aa = 0, bb = 0;
uint32_t page_translate(hwaddr_t addr){
	//printf("get me PE: 0x%x, P:0x%x \n", cpu.CR0.protect_enable, cpu.CR0.paging);
	
	if(cpu.CR0.protect_enable && cpu.CR0.paging){
		
		pageaddr_t paddr;
		paddr.val = addr;
		//printf("get me\n");
		bool flag = false;
		uint32_t temp = check_TLB(addr, &flag);
		if(flag) aa++;
		else bb++;
		printf("%d, %d\n", aa, bb);		
		if(flag) return (temp << 12) + paddr.offset;
		uint32_t base1 = (cpu.CR3.page_directory_base << 12) + (paddr.dir << 2);
		PTE pte;
		pte.val = hwaddr_read(base1, 4);
		assert(pte.present);
		uint32_t base2 = (pte.page_frame << 12) + (paddr.page << 2);
		PTE pte2;
		pte2.val = hwaddr_read(base2, 4);
		assert(pte2.present);
		set_TLB(addr, pte2);
		return (pte2.page_frame << 12) + paddr.offset;
	}
	return addr;
}


uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	// this is a special case, you can handle it later
	
	assert(len == 1 || len == 2 || len == 4);
	hwaddr_t hwaddr = page_translate(addr);
	if(cpu.CR0.protect_enable && cpu.CR0.paging)assert((hwaddr >> 20) + len < (1<<12));
	return hwaddr_read(hwaddr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	// this is a special case, you can handle it later
	assert(len == 1 || len == 2 || len == 4);
	hwaddr_t hwaddr = page_translate(addr);
	if(cpu.CR0.protect_enable && cpu.CR0.paging)assert((hwaddr >> 20) + len < (1<<12));
	hwaddr_write(hwaddr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}
/*
uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(addr, len);
}*/

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}
void load_sr_cache(uint8_t sreg){
	uint32_t addr = (cpu.SR[sreg].index << 3) + cpu.GDTR.base;
	SegDesc sd;
	sd.high = lnaddr_read(addr, 4);
	sd.low = lnaddr_read(addr + 4, 4);
	cpu.SR_cache[sreg].base = (sd.base_31_24<<24)|(sd.base_23_16<<16)|(sd.base_15_0);
	//printf("0x%x\n", addr);
	cpu.SR_cache[sreg].limit = ((sd.limit_19_16 << 16)|sd.limit_15_0);
	if(sd.granularity)cpu.SR_cache[sreg].limit <<= 12;
}
