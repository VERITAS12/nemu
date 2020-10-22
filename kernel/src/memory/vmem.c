#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)

/* Use the function to get the start address of user page directory. */
PDE* get_updir();
static PTE my_pt2[1024] align_to_page;
void create_video_mapping() {
	/* TODO: create an identical mapping from virtual memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
	PDE* pde = (PDE*)va_to_pa(get_updir());
	pde[0].page_frame = (uint32_t)va_to_pa(my_pt2) >> 12;
	pde[0].present = 1;
	uint32_t i;
	set_bp();
	for(i = (VMEM_ADDR>>12); i < ((VMEM_ADDR + SCR_SIZE) >> 12) + 1; i += 1){
		my_pt2[i].page_frame = i;
		my_pt2[i].present = 1;
	}
	set_bp();
	//panic("please implement me");
}

void video_mapping_write_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		buf[i] = i;
	}
}

void video_mapping_read_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		assert(buf[i] == i);
	}
}

void video_mapping_clear() {
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}

