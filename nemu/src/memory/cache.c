#include "common.h"
#include "burst.h"
#include "misc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
uint32_t L2_read_64(hwaddr_t, void *);
void L2_write(hwaddr_t, size_t, uint32_t);

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
		uint32_t tag : TAG_WIDTH;
		uint8_t blocks[NR_BLOCK];
	}row[NR_ROW];
}L;

L L1[NR_GROUP];
void init_cache1(){
	int i, j;
	for(i = 0; i < NR_GROUP;i++){
		for(j = 0; j < NR_ROW;j++){
			L1[i].row[j].valid = 0;	
		}
	}
}

static void cache1_read(hwaddr_t addr, void *data){
	Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!(cache1_read)", addr);
	cache_addr temp;
	temp.addr = addr & ~BURST_MASK;
	uint32_t tag = temp.tag;
	uint32_t group = temp.group;
	uint32_t off = temp.off;
	int i;
	for(i = 0;i < NR_ROW; i++){
		if(L1[group].row[i].tag != tag || L1[group].row[i].valid != 1)continue;
		memcpy(data, L1[group].row[i].blocks+off, BURST_LEN);
		return;
	}

	int a;
	srand((unsigned)time(NULL));
	a = rand() % NR_ROW;
	L2_read_64(addr, L1[group].row[a].blocks);
	L1[group].row[a].valid = 1;
	L1[group].row[a].tag = tag;
	memcpy(data, L1[group].row[a].blocks+off, BURST_LEN);

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



static void cache1_write(hwaddr_t addr, void *data, uint8_t *mask){
	Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!(cache1_read)", addr);
	cache_addr temp;
	temp.addr = addr & ~BURST_MASK;
	uint32_t tag = temp.tag;
	uint32_t group = temp.group;
	uint32_t off = temp.off;
	int i;
	for(i = 0;i < NR_ROW; i++){
		if(L1[group].row[i].tag != tag || L1[group].row[i].valid != 1)continue;
		memcpy_with_mask(L1[group].row[i].blocks+off, data, BURST_LEN, mask);
		return;
	}
	
}

void L1_write(hwaddr_t addr, size_t len, uint32_t data) {
	uint32_t offset = addr & BURST_MASK;
	uint8_t temp[2 * BURST_LEN];
	uint8_t mask[2 * BURST_LEN];
	memset(mask, 0, 2 * BURST_LEN);

	*(uint32_t *)(temp + offset) = data;
	memset(mask + offset, 1, len);

	cache1_write(addr, temp, mask);

	if(offset + len > BURST_LEN) {
		/* data cross the burst boundary */
		cache1_write(addr + BURST_LEN, temp + BURST_LEN, mask + BURST_LEN);
	}
	L2_write(addr, len, data);
}







