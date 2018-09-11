#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "initpaging.h"
#include "x86_desc.h"
#include "i8259.h"
#include "syscall.h"

//static int read_flag = 0;
static uint8_t term_buf[buf_size];
static uint32_t terminal_buffer_index;
static int line_flag = 0;
init_terminal terminals[3];
static int current_x;
static int current_y;
static int current_cursor_x;
static int current_cursor_y;
static int current_id;
static unsigned char kbbuf[buf_size];


/*
 * terminal_init
 *   DESCRIPTION: initializes terminal struct
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void terminal_init(void)
{
	int i = 0 ;
	int j = 0 ;
	
	/*initial the terminal structe*/
	for(i = 0 ; i < 3 ; i++){
	terminals[i].open = 0;
	terminals[i].tem_num = i+1;
	terminals[i].screen_x = 0;
	terminals[i].screen_y = 0;
	terminals[i].cursor_x = 0;
	terminals[i].cursor_y = 0;
	terminals[i].buf_index =0;
	terminals[i].video_ptr = (uint32_t*)((char*)screen_memeory + (i+1)*onepage);
	teminial_map_pages(i+1);
	terminals[i].fish = 0;
	for(j = 0 ;j <128 ; j++)
	{
		terminals[i].buf[j] = NULL;		
	}
	
 	}
	/*open terminal 1*/
	terminals[0].open = 1;
	current_id = 1;
	terminal_buffer_index = 0;
	//switch_to_terminal_2();
	//switch_to_terminal_1();

	//terminals[current_id - 1].cur_pcb = (pcb_t *)(0x800000 - 0x2000 * (1));

	
}


/*
 * swtich to terminal 1
 *   DESCRIPTION: switch to terminal 1 on the screen 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: save previous terminal structure and restore the next terminal 
 */



void switch_to_terminal_1()
{
	//int process_num;
	//uint32_t esp;
	//uint32_t ebp;
	
	/*check if already in terminal 1*/
	if(current_id == 1)
	{
	printf("already in terminal 1");
	return;
	}


	int i ;
	/*save previous terminal struct*/
	terminal_buffer_index = get_buf_index();
	terminals[current_id-1].buf_index = terminal_buffer_index;
	current_x = get_x();
	current_y = get_y();
	current_cursor_x = current_x;
	current_cursor_y = get_y();
	terminals[current_id-1].screen_x  = current_x;
	terminals[current_id-1].screen_y  = current_y;
	terminals[current_id-1].cursor_x = current_cursor_x;
	terminals[current_id-1].cursor_y = current_cursor_y;
	for(i = 0 ; i < 128 ; i++)
	{
	terminals[current_id-1].buf[i] = NULL;
	}
	for(i = 0 ; i < terminals[current_id-1].buf_index;i++)
	{
	terminals[current_id-1].buf[i] = kbbuf[i];
	}
	memcpy((uint8_t*)terminals[current_id-1].video_ptr,(uint8_t*)screen_memeory,0x1000);
	
	/*clear buf*/
	clearbuffuer();
	clearbufferindex();

	
	/*restore terminal 1 struct and remap video memory*/
	current_id = 1;
	//current_pcb = (pcb_t*)terminals[current_id - 1].cur_pcb;
	terminal_buffer_index = terminals[current_id-1].buf_index;
	current_x = terminals[current_id-1].screen_x;
	current_y = terminals[current_id-1].screen_y;
	current_cursor_x = terminals[current_id-1].cursor_x;
	current_cursor_y = terminals[current_id-1].cursor_y;
	memcpy((uint8_t*)screen_memeory,(uint8_t*)terminals[current_id-1].video_ptr,0x1000);
	reset_cursor(current_cursor_x,current_cursor_y);
	reset_position(current_x,current_y);
	copy_to_keyboard_buf(terminals[current_id-1].buf_index,terminals[current_id-1].buf);
	
	
	return;
	//restore_back(terminals[current_id].cur_pcb->old_esp, terminals[current_id].cur_pcb->old_ebp);
	
}

/*
 * swtich to terminal 2
 *   DESCRIPTION: switch to terminal 2 on the screen 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: save previous terminal structure and restore the next terminal 
 */



void switch_to_terminal_2()
{
	//uint32_t esp;
	//uint32_t ebp;
	
	//int process_num;
	
	/*check if already in terminal 2*/
	if(current_id == 2)
	{
	printf("already in terminal 2");
	return;
	}
	//restore the current terminal'

	int i ;

	/*save previous terminal struct*/
	terminal_buffer_index = get_buf_index();
	terminals[current_id-1].buf_index = terminal_buffer_index;
	current_x = get_x();
	current_y = get_y();
	current_cursor_x = current_x;
	current_cursor_y = get_y();
	terminals[current_id-1].screen_x  = current_x;
	terminals[current_id-1].screen_y  = current_y;
	terminals[current_id-1].cursor_x = current_cursor_x;
	terminals[current_id-1].cursor_y = current_cursor_y;
	for(i = 0 ; i < 128 ; i++)
	{
	terminals[current_id-1].buf[i] = NULL;
	}
	for(i = 0 ; i < terminals[current_id-1].buf_index;i++)
	{
	terminals[current_id-1].buf[i] = kbbuf[i];
	}

	memcpy((uint8_t*)terminals[current_id-1].video_ptr,(uint8_t*)screen_memeory,0x1000);
	

	clearbuffuer();
	clearbufferindex();

	/*If terminal 2 is not open, execute shell in terminal 2*/
	if(terminals[1].open == 0)
	{
		terminals[1].open = 1;
		clear();
		reset_position(0,0);
		reset_cursor(0,0);
		current_id = 2;
		for(i = 0 ; i < 128; i++)
		{
		term_buf[i] = NULL;
		}
		current_cursor_x = 0;
		current_cursor_y = 0;
		current_x = 0;
		current_y = 0;
		send_eoi(1);
		//curr_terminal = 1;
		sti();
		//printf("aaa");
		//terminals[current_id-1].current_pid =1;
		execute((uint8_t*) "shell\0");
		// x/16 $esp 
		
	}
	else
	{
	/*restore terminal 2 struct and remap video memory*/
	current_id = 2;
	//current_pcb = (pcb_t*)terminals[current_id - 1].cur_pcb;
	terminal_buffer_index = terminals[current_id-1].buf_index;
	copy_to_keyboard_buf(terminals[current_id-1].buf_index,terminals[current_id-1].buf);
	current_x = terminals[current_id-1].screen_x;
	current_y = terminals[current_id-1].screen_y;
	current_cursor_x = terminals[current_id-1].cursor_x;
	current_cursor_y = terminals[current_id-1].cursor_y;
	memcpy((uint8_t*)screen_memeory,(uint8_t*)terminals[current_id-1].video_ptr,0x1000);
	reset_cursor(current_cursor_x,current_cursor_y);
	reset_position(current_x,current_y);
	

	
}
}




/*
 * swtich to terminal 3
 *   DESCRIPTION: switch to terminal 3 on the screen 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: save previous terminal structure and restore the next terminal 
 */
void switch_to_terminal_3()
{
	//uint32_t esp;
	//uint32_t ebp;
	/*check if already in terminal 3*/
	if(current_id == 3)
	{
	printf("already in terminal 3");
	return;
	}
	//restore the current terminal'

	int i ;
	
	/*save previous terminal struct*/
	terminal_buffer_index = get_buf_index();
	terminals[current_id-1].buf_index = terminal_buffer_index;
	current_x = get_x();
	current_y = get_y();
	current_cursor_x = current_x;
	current_cursor_y = get_y();
	terminals[current_id-1].screen_x  = current_x;
	terminals[current_id-1].screen_y  = current_y;
	terminals[current_id-1].cursor_x = current_cursor_x;
	terminals[current_id-1].cursor_y = current_cursor_y;
	for(i = 0 ; i < 128 ; i++)
	{
	terminals[current_id-1].buf[i] = NULL;
	}
	for(i = 0 ; i < terminals[current_id-1].buf_index;i++)
	{
	terminals[current_id-1].buf[i] = kbbuf[i];
	}
	memcpy((uint8_t*)terminals[current_id-1].video_ptr,(uint8_t*)screen_memeory,0x1000);
	/*
	asm volatile("movl %%esp, %0;"
				:"=r"(esp)
				);
	
	asm volatile("movl %%ebp, %0;"
				:"=r"(ebp)
				);
	terminals[current_id-1].current_esp=esp;
	terminals[current_id-1].current_ebp=ebp;
*/
	clearbuffuer();
	clearbufferindex();


	/*If terminal 3 is not open, execute shell in terminal 3*/

	if(terminals[2].open == 0)
	{
		terminals[2].open = 1;
		clear();
		reset_position(0,0);
		reset_cursor(0,0);
		current_id = 3;
		for(i = 0 ; i < 128; i++)
		{
		term_buf[i] = NULL;
		}
		current_cursor_x = 0;
		current_cursor_y = 0;
		current_x = 0;
		current_y = 0;
		send_eoi(1);
		sti();
		execute((uint8_t*) "shell\0");

		
	}
	else
	{
	/*restore terminal 3 struct and remap video memory*/
	current_id = 3;
	//current_pcb = (pcb_t*)terminals[current_id - 1].cur_pcb;
	terminal_buffer_index = terminals[current_id-1].buf_index;
	copy_to_keyboard_buf(terminals[current_id-1].buf_index,terminals[current_id-1].buf);
	current_x = terminals[current_id-1].screen_x;
	current_y = terminals[current_id-1].screen_y;
	current_cursor_x = terminals[current_id-1].cursor_x;
	current_cursor_y = terminals[current_id-1].cursor_y;
	memcpy((uint8_t*)screen_memeory,(uint8_t*)terminals[current_id-1].video_ptr,0x1000);
	reset_cursor(current_cursor_x,current_cursor_y);
	reset_position(current_x,current_y);
	/*
	tss.ss0  = KERNEL_DS;
	//tss.esp0 = terminals[current_id-1].esp0;
	tss.esp0 =_8MB - _8KB * (terminals[current_id-1].current_pid + 1);
	vur_phy(0x800000+current_pcb->pid*0x400000, 0x8000000);
	tlb_flush();
	asm volatile (
		"movl	%0, %%esp;"
		"movl	%1, %%ebp;"
		:
		: "r" (terminals[current_id-1].current_esp), "r"(terminals[current_id-1].current_ebp)
	);
*/

	}
}

/*
 * terminal_open
 *   DESCRIPTION: initializes terminal stuff (or nothing)
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t terminal_open(const uint8_t* filename){
  return 0;
}
 

/*
 * terminal_close
 *   DESCRIPTION: clears any terminal specific variables (or do nothing)
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t terminal_close(int32_t fd){
  return 0;
}


/*
 * terminal_read
 *   DESCRIPTION: reads FROM the keyboard buffer into buf
 *   INPUTS: buffer for user input, number of bytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if succeed, -1 if fail
 *   SIDE EFFECTS: none
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
	//clear_buf(); //  need to change maybe.
	sti();
	while(terminals[current_id - 1].read_flag!=1 || curr_terminal != current_id-1){};
	cli();
	//printf("current_Id: %d\n", curr_terminal);
	
	//printf("buff2:%d \n", get_buf_index());
	int i =0; 
	int size = 0;
	while(terminals[current_id - 1].buf[i]!=NULL){
		size++;
		i++;
	}
	for(i = 0 ; i < size ; i ++){
		((int8_t*)buf)[i] = terminals[current_id - 1].buf[i];
	
	}
//printf("buff: %d \n", size);
clear_buf();
terminals[current_id - 1].read_flag = 0;
return size;
}



/*
 * terminal_write
 *   DESCRIPTION: writes TO the screen from buf
 *   INPUTS: buffer for user input, number of bytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if succeed, -1 if fail
 *   SIDE EFFECTS: none
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
	int i = 0;
	if(curr_terminal == current_id -1)
	{
	//nbytes = strlen((int8_t*)buf);

	for(i = 0 ; i < nbytes ; i++)
	{	
	
		//memcpy((uint8_t*)screen_memeory,(uint8_t*)terminals[curr_terminal].video_ptr,0x1000);
		putc(((uint8_t*)buf)[i]);
		//putm(((uint8_t*)buf)[i]);
	}

	}
	else
	{
	for(i = 0 ; i < nbytes ; i++)
	{	
		putm(((uint8_t*)buf)[i]);
	}
		
	}
	//memcpy((uint8_t*)screen_memeory,(uint8_t*)terminals[curr_terminal].video_ptr,0x1000);
	//clear_buf();
	return nbytes;
}



/*
 * copy_to_teminal
 *   DESCRIPTION: Copy from keyboard buffer to terminal
 *   INPUTS: keyboard buffer
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void copy_to_teminal(unsigned char *key_buf,int index)
{		//sti();

	int i;
	terminals[current_id - 1].read_flag = 1;
	for(i = 0 ; i < index ; i++)
	{
		terminals[current_id - 1].buf[i] = key_buf[i];
		terminals[current_id - 1].buf_index++;
	}	
	//printf("%d \n",index);
}






/*
 * terminal_test
 *   DESCRIPTION: checkpoint2 test case for terminal.
 *   INPUTS: y position
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

void terminal_test(int y)
{	
	return;
	int length = strlen((int8_t*)(term_buf));
	printf("terminal test: ");
	terminal_write(1,(int8_t*)term_buf,length);
	printf("\nThe first five character:");
	terminal_write(1,(int8_t*)term_buf,5);
	
	terminal_init();
	copy_position(0,y+2);
	reset_position(0,y+3);
	
	if(line_flag ==1)
	{
		copy_position(0,y+3);
		reset_position(0,y+4);
	}
	
}


void clear_buf()
{
	int i = 0 ;
	for(i = 0 ; i < terminals[current_id - 1].buf_index ; i++){
	terminals[current_id - 1].buf[i] = NULL;
	}
	terminals[current_id - 1].buf_index = 0;
	return;
}

void get_position(int x, int y)
{
	current_x = x;
	current_y = y;
}

void get_cursor(int x, int y)
{
	current_cursor_x = x;
	current_cursor_y = y;
}

void copy_keyboard_buf(unsigned char *kb,int index)
{
	int i = 0;
	for(i = 0 ; i <index;i++)
	{
	kbbuf[i] = kb[i];
	}
	return;
}

int get_terminal_id()
{
	return current_id;
}


