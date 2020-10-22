#include "common.h"
#include "burst.h"
#include "misc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
typedef union PageTableEntry {
	struct {
		uint32_t present             : 1;
		uint32_t read_write          : 1;
		uint32_t user_supervisor     : 1;
		uint32_t page_write_through  : 1;
		uint32_t page_cache_disable  : 1;
		uint32_t accessed            : 1;
		uint32_t dirty               : 1;
		uint32_t pad0                : 1;
		uint32_t global              : 1;
		uint32_t pad1                : 3;
		uint32_t page_frame          : 20;
	};
	uint32_t val;
} PTE;
typedef struct TLB{
	uint32_t tag : 20;
	PTE pte;
	uint32_t valid : 1;
}TLB;
TLB tlb[64];
uint32_t check_TLB(uint32_t addr, bool* success){
	int i;
	uint32_t index = addr >> 12;
	for(i = 0;i < 64;i++){
		if(tlb[i].tag == index && tlb[i].valid && tlb[i].pte.present){
			return tlb[i].pte.page_frame;
			*success = true;
		}
	}
	return 0;
}
void init_TLB(){
	memset(tlb, 0, sizeof(tlb));
}
void set_TLB(uint32_t addr, PTE pte){
	int i;
	for(i = 0;i < 64;i++){
		if(tlb[i].valid == 0){
			printf("set\n");
			tlb[i].valid = 1;
			tlb[i].tag = addr>>12;
			tlb[i].pte=pte;
			return;
		}
	}
}
