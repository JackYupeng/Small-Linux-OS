#ifndef TERMINAL_H
#define TERMINAL_H

#include "types.h"
#include "syscall.h"

#define line_ 65
#define buf_size 128
#define screen_memeory 0xB8000
#define onepage 0x1000
#define _8KB 0x2000 
#define _8MB 0x800000

typedef struct{
	uint32_t open;
	uint32_t tem_num;
	uint32_t screen_x;
	uint32_t screen_y;
	uint32_t cursor_x;
	uint32_t cursor_y;
	uint8_t  buf[buf_size];
	uint32_t buf_index;
	uint32_t* video_ptr;
	struct pcb_t* cur_pcb;
	uint32_t current_ebp;
	uint32_t current_esp;
	uint32_t esp0;
	int first_process;
	uint32_t current_pid;
	uint32_t read_flag;
	int fish;
	
}init_terminal;

extern  init_terminal terminals[3];
extern void terminal_init(void);
extern void copy_to_teminal(unsigned char *key_buf,int index);
extern void terminal_test(int y);
extern void clear_buf();
extern void switch_to_terminal_1();
extern void switch_to_terminal_2();
extern void switch_to_terminal_3();
extern void get_position(int x, int y);
extern void get_cursor(int x, int y);
extern void copy_keyboard_buf(unsigned char *kb,int index);
extern int get_terminal_id();
/*terminal driver functions*/
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);



#endif

