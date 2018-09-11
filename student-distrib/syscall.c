#include "syscall.h"
#include "lib.h"
#include "types.h"
#include "terminal.h"


#define max_processes 6
#define first_process_index 2
#define MAX_FD 7
#define MIN_FD 0
#define file_name_char_num 32
#define file_executable_1 0x7f
#define file_executable_2 0x45
#define file_executable_3 0x4c
#define file_executable_4 0x46
#define length_to_read_four 4
#define offset_24 24
#define _8KB 0x2000 
#define _8MB 0x800000
#define MAP_1 0x8000000
#define MAP_2 0x400000
#define read_buf 0x08048000
#define asm_hex 0x83FFFFC
#define B_128 128

/*global variables*/
/* Process ID Array for starting a new process*/
uint8_t process_array [max_processes]={0,0,0,0,0,0};
pcb_t* current_pcb;	
	
//int size;
file_op_t rtc_table;
file_op_t dir_table;
file_op_t file_table;
file_op_t stdin_table;
file_op_t stdout_table;
file_op_t no_op_table;

/* 
*	init_operation_table()
*	Description: initialize all the operations
*	Input: void
*	Output: none
*	Side effect: none
*/
void init_operation_table(){
rtc_table.open = rtc_open;
rtc_table.close =rtc_close;
rtc_table.read= rtc_read;
rtc_table.write =rtc_write;

dir_table.open = dir_open;
dir_table.close =dir_close;
dir_table.read = dir_read;
dir_table.write =dir_write;

file_table.open = file_open ;
file_table.close =file_close;
file_table.read = file_read;
file_table.write =file_write;

stdin_table.open = terminal_open;
stdin_table.close = terminal_close;
stdin_table.read = terminal_read;
stdin_table.write = unvalid_operation;

stdout_table.open = terminal_open;
stdout_table.close = terminal_close;
stdout_table.read = unvalid_operation;
stdout_table.write = terminal_write;

no_op_table.open = unvalid_operation;
no_op_table.close = unvalid_operation;
no_op_table.read = unvalid_operation;
no_op_table.write = unvalid_operation;
}

/* 
*	unvalid_operation()
*	Description: return -1
*	Input: void
*	Output: return -1
*	Side effect: none
*/
int32_t unvalid_operation(){
	return -1;
}

/* 
*	open()
*	Description: open a file into the current PCB
*	Input: filename
*	Output: infex of fd array if sucess, -1 if error or failure
*	Side effect: find the file in the file system and assign an unused file descriptor
*/
int32_t open(const uint8_t* filename){
	
	uint32_t i;
	dentry_t temp_dentry;
	// if(filename == (uint8_t*)""){
	// 	return -1;
	// }
	if(strlen((int8_t*)filename) == 0)
	return -1;
	
	if(read_dentry_by_name((const uint8_t*)filename, &temp_dentry) == -1){
		return -1;
	}

	for(i=first_process_index; i<=MAX_FD; i++){
		if(terminals[curr_terminal].cur_pcb->fd[i].flags == 0) {	//open the first unused file
			terminals[curr_terminal].cur_pcb->fd[i].file_position=0;
			if(temp_dentry.file_type == 0) {
				terminals[curr_terminal].cur_pcb->fd[i].op_table=&rtc_table;
				terminals[curr_terminal].cur_pcb->fd[i].flags=1;
				return i;
			}
			else if(temp_dentry.file_type == 1){
				terminals[curr_terminal].cur_pcb->fd[i].op_table=&dir_table;
				terminals[curr_terminal].cur_pcb->fd[i].flags=1;
				return i;
			}
			else{
				terminals[curr_terminal].cur_pcb->fd[i].op_table=&file_table;
				terminals[curr_terminal].cur_pcb->fd[i].inode = temp_dentry.inode;
				terminals[curr_terminal].cur_pcb->fd[i].flags=1;
				return i;
			}
			break;
		}

	}
	//printf("opened" );
	return -1;
}


/* 
*	close()
*	Description: close a file
*	Input: fd index
*	Output: 0 if success, -1 if error
*	Side effect: close the file descriptor passed in (set it to be available)
*/ 
int32_t close(int32_t fd){
	//if fd is not valid, return -1
	if(fd < MIN_FD || fd > MAX_FD){	
		return -1;
	}

	if(terminals[curr_terminal].cur_pcb->fd[fd].flags == 0 || fd == 0 || fd ==1){
		return -1;
	}

	terminals[curr_terminal].cur_pcb->fd[fd].flags = 0;	//set current pcb flag to 0
	
	return (terminals[curr_terminal].cur_pcb->fd[fd].op_table->close(fd));
	
}


/* 
*	read()
*	Description: reads a file into the buffer
*	Input: fd idx, buffer, and number of bytes to read
*	Output: -1 if error, return the number of bytes read
*	Side effect: use the file operations jump table to call the corresponding read
*/
int32_t read(int32_t fd, void*buf, int32_t nbytes){
	//printf("read" )

	//if fd is not valid, return -1
	if(fd < MIN_FD || fd > MAX_FD) {
		return -1;
	}
	if(terminals[curr_terminal].cur_pcb->fd[fd].flags == 0) {
		return -1;
	}
	return(terminals[curr_terminal].cur_pcb->fd[fd].op_table->read(fd,buf,nbytes));

}

/* 
*	write()
*	Description: reads a file into the buffer
*	Input: fd idx, buffer, and number of bytes to write
*	Output: -1 if error, or return value from file read call
*	Side effect: use the file operations jump table to call the corresponding write
*/
int32_t write(int32_t fd, void*buf, int32_t nbytes){
	//if fd is not valid, return -1
	if(fd < MIN_FD || fd > MAX_FD) {
		return -1;
	}
	if(buf == NULL) {
		return -1;
	}
	if(terminals[get_terminal_id()-1].cur_pcb->fd[fd].flags == 0) {
		return -1;
	}
	return(terminals[get_terminal_id()-1].cur_pcb->fd[fd].op_table->write(fd,buf,nbytes));
}



/* 
*	execute()
*	Description: execute a program
*	Input: command
*	Output: the status if sucess and -1 if fail or error
*	Side effect: parse, executable check, paging, user-level program loader, create PCB, context switch
*/
int32_t execute(const uint8_t* command){
	cli();
	
	uint8_t file_name[file_name_char_num];
	
	int i;
	int j=0;
	uint8_t buf[4];	//the first four bytes of the file identifies the file as an executable
	uint8_t executable[4]={file_executable_1,file_executable_2,file_executable_3,file_executable_4};
	uint8_t process_num;
	uint32_t entry_point;
	int file_len_index;
	uint8_t arg [B_128];
	dentry_t exe_dentry;
	int size=0;
	if(command == NULL){
		return -1;
	}
	for(i=0; i<file_name_char_num; i++){
		file_name[i] = NULL;
	}

	for(i=0; command[i]!='\0'; i++){
		if(command[i]!=' '){
			file_name[i]=command[i];
		}
		else{
			file_len_index=i;
			break;
		}
	}
	
	if(command[i] == '\0'){
		arg[0]='\0';
		
	}
	else{
		for(i=file_len_index+1; command[i]!='\0';i++){
			arg[j] = command[i];
			size++;
			j++;
		}
		arg[j]= '\0';
	}

if(file_name[0]=='f' &&file_name[1]=='i' && file_name[2]== 's' && file_name[3]=='h')
{
	memcpy((uint8_t*)terminals[get_terminal_id()-1].video_ptr,(uint8_t*)screen_memeory,0x1000);
	terminals[get_terminal_id()-1].fish = 1;

}

	if(read_dentry_by_name((uint8_t*) file_name, &exe_dentry) != 0){
		return -1;
	}
	
	if(read_data(exe_dentry.inode, 0, buf, length_to_read_four) == -1){
		return -1;
	}
	//check if the buf is the same as the exexutable, if not, return -1
	if(buf[0]!=executable[0] || buf[1]!=executable[1] || buf[2]!=executable[2]
		|| buf[3]!=executable[3]) {
		return -1;
	}

	read_data(exe_dentry.inode, offset_24, buf, length_to_read_four);
	entry_point= ( buf[0]| buf[1] << 8| buf[2] <<16 | buf[3] << 24 );	//set the entry point
	
	if(terminals[get_terminal_id()-1].first_process == 0){
		process_array[get_terminal_id()-1] = 1;
		process_num = get_terminal_id()-1;
	}
	else{
		if(process_array[3] ==1 && process_array[4]==1 && process_array[5]==1 ){
			printf("Maximum Processes\n");
			return -1;
		}
		for(i=3; i<max_processes; i++){
			if(process_array[i] == 0){
				process_num=i;
				//current_pcb->pid=process_num;
				process_array[i]=1;
				break;
			}
		}
	}
	/*set up program mapping in paging.c*/
	map_to_user_phy(process_num, MAP_1 , MAP_2);

	/*load file */
	read_data(exe_dentry.inode, 0, (uint8_t *)read_buf, file_size(exe_dentry.inode));
	
	/*create pcb / open fds*/	
	current_pcb = (pcb_t *)(_8MB - _8KB * (process_num + 1));
	current_pcb->pid = process_num;
	current_pcb->arg_size = size;
	
	strcpy((int8_t*)current_pcb->pcb_arg, (int8_t*)arg);
	
	terminals[get_terminal_id()-1].cur_pcb = current_pcb;
	
	if(terminals[get_terminal_id()-1].first_process == 0){
		current_pcb->parent_pid=-1;
		terminals[get_terminal_id()-1].first_process =1;
		terminals[get_terminal_id()-1].current_pid = process_num;
		

	}
	else{
		current_pcb->parent_pid = terminals[get_terminal_id()-1].current_pid;
		terminals[get_terminal_id()-1].current_pid = process_num;
		//curr_terminal = get_terminal_id()-1;
		
	}
	curr_terminal = get_terminal_id()-1;
	//parent_id=current_pcb->pid;
	//printf("\nparentid:%d \n",current_pcb->parent_pid);

	uint32_t esp;	
	asm volatile("movl %%esp, %0;"
				:"=r"(esp)
				);
	current_pcb->old_esp=esp;
	uint32_t ebp;
	asm volatile("movl %%ebp, %0;"
				:"=r"(ebp)
				);
	current_pcb->old_ebp=ebp;
	
	
	//terminals[get_terminal_id()-1].current_esp = esp;
	//terminals[get_terminal_id()-1].current_ebp = ebp;
	
	//current_pcb->old_ebp=ebp;
	current_pcb->fd[0].op_table=&stdin_table;
	current_pcb->fd[0].inode =-1;
	current_pcb->fd[0].file_position=0;
	current_pcb->fd[0].flags=1;
	current_pcb->fd[1].op_table=&stdout_table;
	current_pcb->fd[1].inode =-1;
	current_pcb->fd[1].file_position=0;
	current_pcb->fd[1].flags=1;

	for(i=first_process_index; i<=MAX_FD; i++){
		current_pcb->fd[i].op_table=0;
		current_pcb->fd[i].inode=-1;
		current_pcb->fd[i].file_position=0;
		current_pcb->fd[i].flags=0;
	}


	/*context switch(tss privilege switch)*/
	tss.esp0 = _8MB - _8KB * (process_num + 1);
	tss.ss0  = KERNEL_DS;
	
	//asm volatile("leal execute_return_lable, %0\n\t"
	//			 :"=r"(current_pcb->eip));
	
	
asm volatile(
				
				"movw $0x2B, %%ax;"
				"movw %%ax, %%ds;"
				"movw %%ax, %%es;"
				"movw %%ax, %%fs;"
				"movw %%ax, %%gs;"

				"pushl $0x2B;"
				"pushl %0;"
				"pushfl;"
				"popl %%eax;"
				"orl $0x200, %%eax;"
				"pushl %%eax;"
				"pushl $0x23;"
				"pushl %1;"
				"iret;"
				"execute_return_lable:"
				"sti;"
				:
				:"a"(asm_hex), "b"(entry_point)
				//:"%eax"
				);
				
	return 0;

}


/* 
*	halt()
*	Description: terminate a process
*	Input: status
*	Output: status
*	Side effect: return the specified value to its parent process
*/
//int p;
int32_t halt(uint8_t status) {
	//printf("hhhh \n" );
	cli();
	int i;
	uint32_t esp;
	uint32_t ebp;
	uint32_t parent_id;
	if(terminals[curr_terminal].fish == 1)
	{
	   terminals[curr_terminal].fish = 0;
	}
	
	current_pcb = terminals[curr_terminal].cur_pcb;
	parent_id = current_pcb->parent_pid;
	
	esp = current_pcb->old_esp;
	ebp = current_pcb->old_ebp;
	
	process_array[current_pcb->pid]=0;	//shut down child process
	
	/*restore fds*/
	for(i=0; i<=MAX_FD; i++){
		current_pcb->fd[i].op_table=&no_op_table;
		current_pcb->fd[i].flags=0;
	}
	if(current_pcb->parent_pid==-1){
	
		terminals[curr_terminal].first_process=0;
	 	execute((uint8_t*) "shell\0");

	  }
	
	map_to_user_phy(current_pcb->parent_pid, MAP_1, MAP_2);
	

	tss.ss0=KERNEL_DS;
	//tss.esp0 =0x800000-((current_pcb->parent_pid+1) * 0x2000);
	tss.esp0 = _8MB-((current_pcb->parent_pid + 1) * _8KB);
	
	
	//current_pcb=(pcb_t*) (_8MB - (parent_id + 1)* _8KB);
	terminals[curr_terminal].cur_pcb = (pcb_t*) (_8MB - (parent_id + 1)* _8KB);
	terminals[curr_terminal].current_pid = parent_id;
	//terminals[curr_terminal].current_esp = esp;
	//terminals[curr_terminal].current_ebp = ebp;
	//parent_id=current_pcb->pid;
	//printf("\n halt_parentid:%d\n",parent_id);
	//sti();
	//curr_terminal = get_terminal_id()-1;
//	curr_terminal = get_terminal_id()-1;
	asm volatile (
		"movl %0, %%edx;"
		:
		:"r"((int32_t)status)
		//: "%eax"	/*clobber list*/
	);
	asm volatile (
		"movl %0, %%esp;"
		:
		:"r"(esp)
		//: "%eax"	/*clobber list*/
	);
	asm volatile (
		"movl %0, %%ebp;"
		:
		:"r"(ebp)
		//: "%eax"	/*clobber list*/
	);
	asm volatile (
		"movl %%edx, %%eax;"
		:
		:
		//: "%eax"	/*clobber list*/
	);
	//asm volatile ("leave;");
	//asm volatile ("ret;");
	return 0;
}

/* 
*	int32_t getargs(uint8_t* buf, int32_t nbytes)
*	Description: get arguement
*	Input: uint8_t* buf, int32_t nbytes
*	Output: the status if sucess and -1 if fail or error
*	Side effect:none
*/



int32_t getargs(uint8_t* buf, int32_t nbytes) {

	if(buf == NULL){
		return -1;
	}
	if(terminals[get_terminal_id()-1].cur_pcb->arg_size ==0){
		return -1;
	}
	strcpy((int8_t*)buf, (int8_t*)current_pcb->pcb_arg);
	return 0;
	
 }


/* 
*	int32_t vidmap(uint8_t ** screen_start)
*	Description: remap the page
*	Input: screen_start
*	Output: the status if sucess and -1 if fail or error
*	Side effect:
*/
 
 int32_t vidmap(uint8_t ** screen_start) {
	 //if screen_start smaller than 128 MB or greater then 132 MB out of range return -1;
	if((uint32_t)screen_start < 0x08000000 ||(uint32_t)screen_start >=(0x08000000+0x00400000))
		return -1;
	
	if(screen_start == NULL)
		return -1;
	// set screen_start to 132 MB virtual address.
	*screen_start = (uint8_t*)0x400000+128*1024*1024;
	
	//call map_to_vid with first parameter: virtual address:132MB
	// second parameter: physical address: video memeory starts
	if(get_terminal_id()== 1){
		map_to_vid(0x400000+128*1024*1024,0xB9000);// vir =132mb phy=0xb8000
		tlb_flush();
	}
	if(get_terminal_id()== 2){
		map_to_vid(0x400000+128*1024*1024,0xBa000);
		tlb_flush();
	}
	if(get_terminal_id()== 3){
		map_to_vid(0x400000+128*1024*1024,0xBb000);
	}
	//return the virtual address 132 MB/
	return 0x400000+128*1024*1024;
}


int get_process_number()
{		
	return current_pcb->pid;
}
 /*


int32_t set_handler(int32_t signum, void* handler_address) {
	return 0;
}

int32_t sigreturn(void) {
	return 0;
}

*/
