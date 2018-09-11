#ifndef SYSCALL_H
#define SYSCALL_H

#include "rtc.h"
#include "filesystem.h"
#include "types.h"

#include "initpaging.h"
#include "interrupthandler.h"
#include "terminal.h"
#include "schedule.h"

/*PCB structure*/
typedef struct pcb_t{
	uint32_t pid;
	uint32_t parent_pid;
	file_descriptor_t fd[8];
	uint32_t old_esp;
	uint32_t old_ebp;
	uint8_t pcb_arg[128];
	int arg_size;
	uint32_t eip;
	
}pcb_t;


extern pcb_t* current_pcb;

/*functions to set up*/
void init_operation_table();
int32_t unvalid_operation();
extern int get_process_number();

/*below are system calls*/
int32_t execute(const uint8_t* command);
int32_t halt(uint8_t status);
int32_t read(int32_t fd, void*buf, int32_t nbytes);
int32_t write(int32_t fd, void*buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t vidmap(uint8_t ** screen_start);
int32_t getargs(uint8_t* buf, int32_t nbytes);
/*

int32_t set_handler(int32_t signum, void* handler_address);
int32_t sigreturn(void);
*/

void restore_back(uint32_t esp, uint32_t ebp);

#endif

