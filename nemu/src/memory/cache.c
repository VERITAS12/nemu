#include "common.h"
#include "burst.h"
#include "misc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
uint32_t dram_read_64(hwaddr_t, void *);


#define GROUP_WIDTH 7
#define BLOCK_WIDTH 6
#define TAG_WIDTH (27 - GROUP_WIDTH - BLOCK_WIDTH)
#define HW_MEM_SIZE (1 << (GROUP_WIDTH+BLOCK_WIDTH+TAG_WIDTH)) 
typedef union {
	struct {
		uint32_t off	: BLOCK_WIDTH;
		uint32_t group	: GROUP_WIDTH;
		uint32_t tag	: TAG_WIDTH;
	};
	uint32_t addr;
} cache_addr;

#define NR_GROUP (1 << GROUP_WIDTH)
#define NR_ROW (1 << 3)
#define NR_BLOCK (1 << BLOCK_WIDTH)
typedef struct cache{
	struct{
		uint32_t valid : 1;
		uint32_t tag : 14;
		uint8_t blocks[NR_BLOCK];
	}row[NR_ROW];
}L;

L L1[NR_GROUP];
void init_cache1(){
	int i, j;
	for(i = 0; i < NR_GROUP;i++){
		for(j = 0; j < NR_ROW;i++){
			L1[i].row[j].valid = 0;	
		}
	}
}

static void cache1_read(hwaddr_t addr, void *data){
	//printf("1\n");
	Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!(cache1_read)", addr);
	cache_addr temp;
	temp.addr = addr & ~BURST_MASK;
	uint32_t tag = temp.tag;
	uint32_t group = temp.group;
	uint32_t off = temp.off;
	int i;
	for(i = 0;i < NR_ROW; i++){
		if(L1[group].row[i].tag != tag || L1[group].row[i].valid != 1)continue;
		//printf("tag: 0x%x, valid: 0x%x\n", L1[group].row[i].tag, L1[group].row[i].valid);
		memcpy(data, L1[group].row[i].blocks+off, BURST_LEN);
		printf("hit off: %d %d\n",off, *(L1[group].row[i].blocks+off));
		return;
	}

	int a;
	srand((unsigned)time(NULL));
	a = 0;
	dram_read_64(addr, L1[group].row[a].blocks);
	L1[group].row[a].valid = 1;
	L1[group].row[a].tag = tag;
	if(L1[group].row[a].tag != tag || L1[group].row[a].valid != 1)return;
	memcpy(data, L1[group].row[a].blocks+off, BURST_LEN);
	printf("NO hit :%d\n", *(L1[group].row[a].blocks+off));
	//L1[group].row[a].valid = 0;

}
uint32_t L1_read(hwaddr_t addr, size_t len){
	uint32_t offset = addr & BURST_MASK;
	uint8_t temp[2 * BURST_LEN];
	cache1_read(addr, temp);
	if(offset + len > BURST_LEN){
		cache1_read(addr + BURST_LEN, temp + BURST_LEN);
	}
	return unalign_rw(temp + offset, 4);
}













