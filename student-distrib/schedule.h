#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "types.h"
#include "syscall.h"
#include "terminal.h"
#include "initpaging.h"
#include "keyboard.h"

#define CH0_PORT	0x40
#define CH1_PORT	0x41
#define CH2_PORT	0x42
#define COM_REG		0x43
#define MODE3_WAVE  0x36
#define PIT_IRQ		0
#define HZ_20  1193180/ 20

void pit_init();
void pit_handler();
void do_switch();
extern uint8_t curr_terminal ;
extern uint8_t next_terminal;
#endif 
