#ifndef _INITPAGING_H
#define _INITPAGING_H

#include "types.h"
#define videomemory   0xB8000
#define page_size 1024
#define nopresent_rw 0x00000002
#define bitmask 12
#define kern    0x83
#define present_rw 3
#define no_rw 2
#define page_add 0x1000
#define size_direc 4096
#define ker_add 0x400000
#define B_128 128
#define all_o 0x87
#define read_write 0x07

extern void paging_initialize();

extern void loadPageDirectory(uint32_t* page_directory);

extern void enablePaging();

extern void teminial_map_pages(uint32_t terminal_id);

extern void vur_phy(uint32_t phy, uint32_t vir);

extern void map_to_user_phy(uint32_t program_number,uint32_t start_address,uint32_t program_size);

extern void map_to_vid(uint32_t virtual_address, uint32_t phy_address);

extern void tlb_flush(void);
#endif

