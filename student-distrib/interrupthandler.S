#define ASM 1
#include "x86_desc.h"

.global keyboard_interrupt
.global rtc_interrupt
.global syscall_handler
.global pit_interrupt
#calling convetion and then call the print_keyboard
#linkage
keyboard_interrupt:
	cli
	pushfl
	call print_keyboard
	popfl
	sti
	iret
#calling convetion and then call the rtc_handle_interrupt
rtc_interrupt:
	cli
	pushfl
	call rtc_handle_interrupt
	popfl
	sti
	iret
	
pit_interrupt:
	cli
	pushal
	pushfl
	call pit_handler
	popfl
	popal
	sti
	iret


syscall_handler:


#	cmpl $1, %eax
#	jne break_point
#	pushl %eax
#	popl %eax
#break_point:


	pushfl 
	cmpl $1, %eax
	jl error
	cmpl $8, %eax
	jg error

	
	pushl %edx	#argument 3
	pushl %ecx  #argument 2
	pushl %ebx  #argument 1

	sti
	call *syscall_jump_table(, %eax,4)
	
	popl %ebx
	popl %ecx
	popl %edx
	
	popfl

	iret
	

error:
	popfl
	movl $-1, %eax
	iret
	


syscall_jump_table:
	.long 0x0, halt, execute, read, write, open, close, getargs, vidmap#, set_handler, sigreturn
