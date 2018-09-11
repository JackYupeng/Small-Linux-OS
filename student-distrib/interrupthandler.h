#ifndef _INTERRUPTHANDLER_H
#define _INTERRUPTHANDLER_H

#include "idt.h"
#include "syscall.h"
#include "schedule.h"
extern void keyboard_interrupt();
extern void rtc_interrupt();
extern void syscall_handler();
extern void pit_interrupt();
#endif

