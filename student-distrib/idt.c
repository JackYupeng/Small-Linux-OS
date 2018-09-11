#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "keyboard.h"
#include "interrupthandler.h"

/* An interrupt descriptor entry (goes into the IDT) 
typedef union idt_desc_t {
    uint32_t val[2];
    struct {
        uint16_t offset_15_00;
        uint16_t seg_selector;
        uint8_t  reserved4;
        uint32_t reserved3 : 1;
        uint32_t reserved2 : 1;
        uint32_t reserved1 : 1;
        uint32_t size      : 1;
        uint32_t reserved0 : 1;
        uint32_t dpl       : 2;
        uint32_t present   : 1;
        uint16_t offset_31_16;
    } __attribute__ ((packed));
} idt_desc_t;

*/

/*
Number	Description	Type
0	Divide-by-zero	fault
1	Debug exception	trap or fault
2	Non-Maskable Interrupt (NMI)	trap
3	Breakpoint (INT 3)	trap
4	Overflow (INTO with EFlags[OF] set)	trap
5	Bound exception (BOUND on out-of-bounds access)	trap
6	Invalid Opcode	trap
7	FPU not available	trap
8*	Double Fault	abort
9	Coprocessor Segment Overrun	abort
10*	Invalid TSS	fault
11*	Segment not present	fault
12*	Stack exception	fault
13*	General Protection	fault or trap
14*	Page fault	fault
15	Reserved
16	Floating-point error	fault
17	Alignment Check	fault
18	Machine Check	abort
19  folating point exception 
20-31	Reserved By Intel Do not use
32-255	user defined interrupts

cited from intel menu
*/


#define exception_size 20
#define interrupt_32   32
#define keyb_interrupt 0x21

#define interrupt_rtc  0x28
#define interrupt_pit	0x20
/*Description: 20 exception functions and that handle fault, diable interrupts and print exception message.
 *Inputs: none
 *Outputs: none
 *Return value: none
 *Side effects: none		
 */

void EXCEPTION_DE(){
	cli();
	test_exceptions();
	printf("%s", "Divide Error");
	while(1);
	sti();
}

void EXCEPTION_DB(){
	cli();
	test_exceptions();
	printf("%s", "Debug exception");
	while(1);
	sti();
}
void EXCEPTION_NMI(){
	cli();
	test_exceptions();
	printf("%s", "Non-Maskable Interrupt");
	while(1);
	sti();
}
void EXCEPTION_BP(){
	cli();
	test_exceptions();
	printf("%s", "Breakpoint");
	while(1);
	sti();
}
void EXCEPTION_OF(){
	cli();
	test_exceptions();
	printf("%s", "Overflow");
	while(1);
	sti();
}
void EXCEPTION_BR(){
	cli();
	test_exceptions();
	printf("%s", "Bound exception");
	while(1);
	sti();
}
void EXCEPTION_UD(){
	cli();
	test_exceptions();
	printf("%s", "Invalid Opcode");
	while(1);
	sti();
}
void EXCEPTION_NM(){
	cli();
	test_exceptions();
	printf("%s", "Device Not Available");
	while(1);
	sti();
}
void EXCEPTION_DF(){
	cli();
	test_exceptions();
	printf("%s", "Double Fault");
	while(1);
	sti();
}
void EXCEPTION_CS(){
	cli();
	test_exceptions();
	printf("%s", "Coprocessor Segment Overrun");
	while(1);
	sti();
}
void EXCEPTION_TS(){
	cli();
	test_exceptions();
	printf("%s", "Invalid TSS");
	while(1);
	sti();
}
void EXCEPTION_NP(){
	cli();
	test_exceptions();
	printf("%s", "Segment Not Present");
	while(1);
	sti();
}
void EXCEPTION_SS(){
	cli();
	test_exceptions();
	printf("%s", "	Stack Exception");
	while(1);
	sti();
}
void EXCEPTION_GP(){
	cli();
	test_exceptions();
	printf("%s", "General Protection");
	while(1);
	sti();
}
void EXCEPTION_PF(){
	cli();
	//test_exceptions();
	printf("%s", "Page Fault");
	while(1);
	sti();
}
void EXCEPTION_MF(){
	cli();
	test_exceptions();
	printf("%s", "Floating-point error");
	while(1);
	sti();
}
void EXCEPTION_AC(){
	cli();
	test_exceptions();
	printf("%s", "Alignment Check");
	while(1);
	sti();
}
void EXCEPTION_MC(){
	cli();
	test_exceptions();
	printf("%s", "Machine Check");
	while(1);
	sti();
}
void EXCEPTION_XF(){
	cli();
	test_exceptions();
	printf("%s", "SIMD Floating-Point Exception");
	while(1);
	sti();
}

/*Description: undefined interruptions that print undefined interrupts message.
 *Inputs: none
 *Outputs: none
 *Return value: none
 *Side effects: none		
 */


void general_interrupt(){
	cli();
	printf("Undefined");
	sti();
}
/*Description: intialize idt table with 256 exception or interrupt vectos, each is an eight-byte entry. 
 *Inputs: none
 *Outputs: none
 *Return value: none
 *Side effects: initial 256 entries and set the first 20 entires for exceptions and 0x21 and 0x28 for keyboard
 *  			and RTC		
 */
void idt_init(){
	lidt (idt_desc_ptr);
	int i; /*for loop index*/
	/* initialize entries for exceptions*/
	for(i=0; i< interrupt_32; i++){
		idt[i].present=0x1;   /* 1 for using interrupts, 0 for unused interrupts*/
		idt[i].dpl=0x0;     /*0 for exceptions and hardware interrupts, 3 for software interrupts*/
		idt[i].size=0x1;  /* 1 for 32 bits, 0 for 16 bits*/
		/* reserved bits for exceptions are 01110.*/
		idt[i].reserved0=0x0;
		idt[i].reserved1=0x1;
		idt[i].reserved2=0x1;
		idt[i].reserved3=0x1;
		idt[i].reserved4=0x0;
		idt[i].seg_selector=KERNEL_CS; /*selector of the interrupt function*/

	}
	/* initialize entries for interrupts*/
	for(i=interrupt_32; i<NUM_VEC; i++){
		idt[i].present=0x1;   /* 1 for using interrupts, 0 for unused interrupts*/
		idt[i].dpl=0x0;     /*0 for exceptions and hardware interrupts, 3 for software interrupts*/
		idt[i].size=0x1;  /* 1 for 32 bits,0 for 16 bits*/
		/* reserved bits for exceptions are 01100.*/
		idt[i].reserved0=0x0;
		idt[i].reserved1=0x1;
		idt[i].reserved2=0x1;
		idt[i].reserved3=0x0;
		idt[i].reserved4=0x0;
		idt[i].seg_selector=KERNEL_CS; /*selector of the interrupt function*/
		SET_IDT_ENTRY(idt[i], general_interrupt); /*set entries for undefined interrupts.*/
	}
	
	/* set idt entries for first 20 exceptions */
	SET_IDT_ENTRY(idt[0], &EXCEPTION_DE);
	SET_IDT_ENTRY(idt[1], &EXCEPTION_DB);
	SET_IDT_ENTRY(idt[2], &EXCEPTION_NMI);
	SET_IDT_ENTRY(idt[3], &EXCEPTION_BP);
	SET_IDT_ENTRY(idt[4], &EXCEPTION_OF);
	SET_IDT_ENTRY(idt[5], &EXCEPTION_BR);
	SET_IDT_ENTRY(idt[6], &EXCEPTION_UD);
	SET_IDT_ENTRY(idt[7], &EXCEPTION_NM);
	SET_IDT_ENTRY(idt[8], &EXCEPTION_DF);
	SET_IDT_ENTRY(idt[9], &EXCEPTION_CS);
	SET_IDT_ENTRY(idt[10], &EXCEPTION_TS);
	SET_IDT_ENTRY(idt[11], &EXCEPTION_NP);
	SET_IDT_ENTRY(idt[12], &EXCEPTION_SS);
	SET_IDT_ENTRY(idt[13], &EXCEPTION_GP);
	SET_IDT_ENTRY(idt[14], &EXCEPTION_PF);
	SET_IDT_ENTRY(idt[16], &EXCEPTION_MF);
	SET_IDT_ENTRY(idt[17], &EXCEPTION_AC);
	SET_IDT_ENTRY(idt[18], &EXCEPTION_MC);
	SET_IDT_ENTRY(idt[19], &EXCEPTION_XF);

	/*set idt entries for keyboard and rtc interrupts*/
	SET_IDT_ENTRY(idt[keyb_interrupt],keyboard_interrupt);
	SET_IDT_ENTRY(idt[interrupt_rtc], rtc_interrupt);
	SET_IDT_ENTRY(idt[0x80],syscall_handler);
	SET_IDT_ENTRY(idt[interrupt_pit], pit_interrupt);
	idt[0x80].reserved3 = 0;
	idt[0x80].dpl       = 3;
	idt[0x80].present   = 1;
}




