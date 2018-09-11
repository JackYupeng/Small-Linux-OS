#include "rtc.h"


// lib commands 
// inb -- Port read functions Inb reads a byte and returns its value as a zero-extended 32-bit unsigned int static inline uint32_t inb(port)
// outb -- Writes a byte to a port outb(data, port)   
// cli -- Clear interrupt flag - disables interrupts on this processor
// sti -- Set interrupt flag - enable interrupts on this processor sti()   


// Source: http://wiki.osdev.org/RTC

// The 2 IO ports used for the RTC and CMOS are 0x70 and 0x71. Port 0x70 is used to specify an index or "register number", 
// and to disable NMI. Port 0x71 is used to read or write from/to that byte of CMOS configuration space. Only three bytes 
// of CMOS RAM are used to control the RTC periodic interrupt function. They are called RTC Status Register A, B, and C. 
// They are at offset 0xA, 0xB, and 0xC in the CMOS RAM.

// Setting the Registers code
// disable_ints();		// important that no interrupts happen (perform a CLI)
// outportb(0x70, 0x8A);	// select Status Register A, and disable NMI (by setting the 0x80 bit)
// outportb(0x71, 0x20);	// write to CMOS/RTC RAM
// enable_ints();		// (perform an STI) and reenable NMI if you wish

// Turning on the IRQ8
// disable_ints();			// disable interrupts
// outportb(0x70, 0x8B);		// select register B, and disable NMI
// char prev=inportb(0x71);	// read the current value of register B
// outportb(0x70, 0x8B);		// set the index again (a read will reset the index to register D)
// outportb(0x71, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
// enable_ints();

// Changing Interrupt Rate
// rate &= 0x0F;			// rate must be above 2 and not over 15
// disable_ints();
// outportb(0x70, 0x8A);		// set index to register A, disable NMI
// char prev=inportb(0x71);	// get initial value of register A
// outportb(0x70, 0x8A);		// reset index to A
// outportb(0x71, (prev & 0xF0) | rate); //write only our rate to A. Note, rate is the bottom 4 bits.
// enable_ints();

// end Source: http://wiki.osdev.org/RTC
#define R_A 0x8A
#define	R_B 0x8B
#define	R_C	0x8C
#define PORT_1	0x70
#define PORT_2	0x71
#define RTC_masterirq	8
#define RTC_slaveirq 	2
#define port_mask 0x40
#define inirate 0x0F
#define rate_mask 0xF0
unsigned char rate;
volatile int32_t rtc_interrupt_flag;

#define HZ_2 2
#define HZ_4 4
#define HZ_8 8
#define HZ_16 16
#define HZ_32 32
#define HZ_64 64
#define HZ_128 128
#define HZ_256 256
#define HZ_512 512
#define HZ_1024 1024

#define rate_2 0xF
#define rate_4 0xE
#define rate_8 0xD
#define rate_16 0xC
#define rate_32 0xB
#define rate_64 0xA
#define rate_128 0x9
#define rate_256 0x8
#define rate_512 0x7
#define rate_1024 0x6

#define REGISTER_A 0x8A
#define NMI 0x70
#define INIT_VAL 0x71
#define RTC_MASK 0xF0


/*Description: intialize real time clock, enable master and slave IRQs. 
 *Inputs: none
 *Outputs: none
 *Return value: none
 *Side effects: disable other interrupts while programming the RTC.
 */

void rtc_init(){
	
	cli(); /* Clear interrupt flag - disables interrupts on this processor found in lib.h*/
	disable_irq(RTC_masterirq); /*disable interrupt from master irq*/
	disable_irq(RTC_slaveirq);		/*disable slave irq*/
	outb(R_B, PORT_1);		/*select register B, and disable NMI */
	char prev = inb(PORT_2);	/*read the current value of register B*/
	outb(R_B, PORT_1);			/*set the index again (a read will reset the index to register D)*/
	outb((prev | port_mask), PORT_2); /*write the previous value ORed with 0x40. This turns on bit 6 of register B*/
	
	rate = inirate;
	outb(R_A, PORT_1);
	prev = inb(PORT_2);
	outb(R_A, PORT_1);
	outb((prev & rate_mask) | rate, PORT_2);
	enable_irq(RTC_slaveirq);		/*enable master irq*/
	enable_irq(RTC_masterirq); 		/*enable slave irq*/
	
	/*.... code to the bottom of your IRQ handler to be sure you get another interrupt.
	site from: http://wiki.osdev.org/RTC */		
	outb(R_C, PORT_1);			/*select register C*/
	inb(PORT_2);				/*just throw away contents*/
	
	sti();	/*set interrupt flag*/
}

/*Description: RTC interrupt handler
 *Inputs: none
 *Outputs: none
 *Return value: none
 *Side effects: send eoi when interrupt finishes 
 */
void rtc_handle_interrupt(){
	cli();		/*clear interrupt flag*/
	//test_interrupts();	/*call test_interrupts function in lib.c*/
	outb(R_C, PORT_1);	/*select register B, and disable NMI */
	inb(PORT_2);			/*read data*/
	
	send_eoi(RTC_masterirq); 	/*Send end-of-interrupt signal for irq8*/
	rtc_interrupt_flag=0;
	sti();	/*set interrupt flag*/
}





/*
 * rtc_open
 *   DESCRIPTION: Initialnize RTC and set to 2 HZ
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t rtc_open(const uint8_t* filename){

	rtc_init();

	return 0;
}

/*
 * rtc_read
 *   DESCRIPTION: Block until the next interrupt, not for reading RTC frequency
 *   INPUTS: buf-pointer to the buffer which contains 4 byte frequency. nbytes-nbytes read
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes){
	sti();
	rtc_interrupt_flag = 1;
	while(rtc_interrupt_flag);	//while the interrupt has not occur, wait. 
	return 0;	//if interrupt occurs, return 0
}


/*
 * rtc_write
 *   DESCRIPTION: Change RTC frequency
 *   INPUTS: buf-pointer to the buffer which contains 4 byte frequency. nbytes-nbytes read
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if succeed, -1 if fail
 *   SIDE EFFECTS: none
 */
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes){
	sti();
	uint32_t rate;
	if(  (*(int*)buf) & ((*(int*)buf)-1)  )	//if frequency is not power of 2, return -1
		return -1;

	if( (*(int*)buf) < 0 || (*(int*)buf)>1024)	//if frequency is out of range, return -1
		return -1;
	// rate must be above 2 and not over 15
	switch(*(int*)buf){
		case HZ_2:
			rate = rate_2;
			break;
		case HZ_4:
			rate = rate_4;
			break;
		case HZ_8:
			rate = rate_8;
			break;
		case HZ_16:
			rate = rate_16;
			break;
		case HZ_32:
			rate = rate_32;
			break;
		case HZ_64:
			rate = rate_64;
			break;
		case HZ_128:
			rate = rate_128;
			break;
		case HZ_256:
			rate = rate_256;
			break;
		case HZ_512:
			rate = rate_512;
			break;
		case HZ_1024:
			rate = rate_1024;
			break;
		default:
			return -1;
	}

	//critical section
	cli();	//disable interrupts
	outb(REGISTER_A, NMI);		// set index to register A, disable NMI
	char prev=inb(INIT_VAL);	// get initial value of register A
	outb(REGISTER_A, NMI);		// reset index to A
	outb((prev & RTC_MASK) | rate, INIT_VAL); //write only our rate to A. Note, rate is the bottom 4 bits.
	sti();	//enable interrupts
	return 0;
}



/*
 * rtc_close
 *   DESCRIPTION: Probably does nothing, unless virtualize RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t rtc_close (int32_t fd){
	return 0;
}

