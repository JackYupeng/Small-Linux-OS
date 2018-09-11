#include "initpaging.h"
#include "x86_desc.h"
#include "lib.h"

static unsigned long kernel = ker_add;// the kernel address
static uint32_t page_directory[page_size] __attribute__((aligned(size_direc)));

static uint32_t first_page_table[page_size] __attribute__((aligned(size_direc)));


/*
 * paging_initialize()
 *   DESCRIPTION: initialize the paging ,setting the page_directory and page_table
 *                find the location of video meomery and then save it.
 *   INPUTS: none(ignored)
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
 //cited from http://wiki.osdev.org/Setting_Up_Paging 
void paging_initialize()
{
	//set each entry to not present
	int i;
	for(i = 0; i < page_size; i++)
	{
	//first set all the entry to 0x00000002 mean that 
	//it is not present but can write and read
    page_directory[i] = nopresent_rw;
	}

	unsigned int vm_index; //where the videomemory should be save in page_table;
	vm_index = videomemory >> bitmask ;
	
//we will fill all 1024 entries in the table, mapping 4 megabytes
	for(i = 0; i < page_size; i++)
	{
	//get the index of videomemory and locate the address of videomemory.
	//search every page_table_index until the vm_index
	if(i == vm_index)
	{
	//save the vm_index and set the u/s r/w and p to 1.
	first_page_table[i] = videomemory | present_rw;
	} 
	else
	{
    first_page_table[i] = (i * page_add) | no_rw; 
	}
	}
	
	// attributes: supervisor level, read/write, present
	//reference from: Osdev/setting_paging.
	page_directory[0] = ((unsigned int)first_page_table) | present_rw;
	page_directory[1] =  kernel | kern;
	
	//call the the function in initpaging.S
	//to store the cr3 and cr0;
	loadPageDirectory(page_directory);
	enablePaging();
	
	return;
}



/*
 *map_to_user_phy(uint32_t program_number,uint32_t start_address,uint32_t program_size)
 *   DESCRIPTION: map to paging depending ong the program_number
 *   INPUTS: uint32_t program_number,uint32_t start_address,uint32_t program_size
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

void map_to_user_phy(uint32_t program_number,uint32_t start_address,uint32_t program_size)
{
	uint32_t cur_address;
	uint32_t pde_index;

	
	pde_index = B_128 / 4 ;
	cur_address = (program_number+2) * program_size;
	page_directory[pde_index] =  cur_address | all_o ;
	
	tlb_flush();
	return;
}


/*
 * map_to_vid(uint32_t virtual_address, uint32_t phy_address)
 *   DESCRIPTION: map to video meomry
 *   INPUTS: uint32_t virtual_address, uint32_t phy_address
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

void map_to_vid(uint32_t virtual_address, uint32_t phy_address)
{
	uint32_t pde_index = (B_128+4) / 4 ;
	
	page_directory[pde_index]=(uint32_t)first_page_table |read_write;
	
	first_page_table[0] = phy_address|read_write;	
	
}



void vur_phy(uint32_t phy, uint32_t vir)
{
	uint32_t pde_index = vir / 0x400000;
	
	page_directory[pde_index] = phy | all_o;
	
}

void teminial_map_pages(uint32_t terminal_id)
{
	unsigned int vm_index;
	unsigned int vm;
	vm = videomemory + terminal_id*0x1000;
	vm_index = vm >>bitmask;
	first_page_table[vm_index] = vm | present_rw;
}

/*
 * tlb_flush(void)
 *   DESCRIPTION: flush tlb
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

//referenced from osdev_flush.
void tlb_flush(void)
{
	asm volatile(
		"movl	%%cr3,%%eax;"
		"movl	%%eax,%%cr3;"
		:
		:
		:"%eax"
	);
}


	
