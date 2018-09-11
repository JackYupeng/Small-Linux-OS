#include "schedule.h"

#define MAP_1 0x8000000
#define MAP_2 0x400000

uint8_t curr_terminal ;
uint8_t next_terminal ;
int curr_pid;
int next_pid ;

/* 
*	pit initial function
*	Description: initial pit and set up the frequency
*	Input: none
*	Output: none
*	Side effect: none
*/

void pit_init(){
	
	//divisor = 1193180/ 20;
	
	outb(MODE3_WAVE, COM_REG);
	outb(HZ_20 & 0xFF, CH0_PORT);
	outb(HZ_20 >> 8, CH0_PORT);
	enable_irq(PIT_IRQ);
	curr_terminal = 0;
	next_terminal = 0;
	//terminals[1].current_esp = 0x8371996;
	//terminals[1].current_ebp = 0x8372020;
	//terminals[2].current_esp = _8MB - _8KB * (2 + 1);
	//terminals[2].current_esp = _8MB - _8KB * (2 + 1);
	//curr_terminal = current_id;
}

/* 
*	pit handler
*	Description: determine the next process to execute and handle process switch
*	Input: none
*	Output: none
*	Side effect: none
*/
void pit_handler(){
	cli();
	int i;
	curr_pid = terminals[curr_terminal].current_pid;
	/*check next avalible process*/
	for(i=1; i<3; i++){

	 	if (terminals[(curr_terminal + i) % 3].open) {
	 		next_terminal = (curr_terminal + i) % 3;
	 		break;
		 }
	} 
	
	if(next_terminal == 1)
	{
		terminal_flag =1;
	}
	
	send_eoi(PIT_IRQ);
	next_pid = terminals[next_terminal].current_pid;
	do_switch();

	sti();
	return;	
}


/* 
*	swtich process
*	Description: switch context to execute the next process
*	Input: none
*	Output: none
*	Side effect: switch esp, ebp, esp0, and paging. 
*/

void do_switch(){
	//cli();
	//printf("%d", next_terminal);
	//printf("buff:%d \n",terminals[curr_terminal].buf_index);
	
	//pcb_t* curr_pcb = (pcb_t *)(_8MB - _8KB * (curr_pid + 1) );
	//pcb_t* next_pcb = (pcb_t *)(_8MB - _8KB * (next_pid + 1) );
	
	/*save old esp and ebp*/
	uint32_t esp;
	uint32_t ebp;	
	asm volatile("movl %%esp, %0;"
				:"=r"(esp)
				);
	asm volatile("movl %%ebp, %0;"
				:"=r"(ebp)
				);
 
	terminals[curr_terminal].current_esp = esp;
	terminals[curr_terminal].current_ebp = ebp;
	
	
	
	if(curr_terminal == get_terminal_id()-1 && terminals[curr_terminal].fish == 1)
	{
	memcpy((uint8_t*)screen_memeory,(uint8_t*)terminals[get_terminal_id()-1].video_ptr,0x1000);

	}
	
	
	/*remap vitirul to physical*/
	vur_phy(0x800000 + next_pid * 0x400000, 0x8000000);
	tlb_flush();
	
	tss.esp0 = _8MB - _8KB * (next_pid + 1)-4;
	tss.ss0  = KERNEL_DS;
	

	curr_terminal = next_terminal;
	
	
	//printf("%d",curr_terminal);
	//if(curr_terminal == get_terminal_id() -1){
	//	memcpy((uint8_t*)screen_memeory,(uint8_t*)terminals[curr_terminal].video_ptr,0x1000);
	//}
	//printf("*");
	
	/*restore next esp ebp*/
	asm volatile("movl %0, %%esp;"
				:
				:"r"(terminals[next_terminal].current_esp)
				);
	asm volatile("movl %0, %%ebp;"
				:
				:"r"(terminals[next_terminal].current_ebp)
				);
	/*asm volatile("leal switch_label, %0\n\t"
				:"=r"(terminals[curr_terminal].cur_pcb->eip));
	asm volatile("jmp *%0 \n\t"
				 "switch_label:\n\t"	
				:
				:"r"(terminals[next_terminal].cur_pcb->eip));
*/
	//return;
}
