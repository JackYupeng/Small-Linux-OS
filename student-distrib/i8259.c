/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

static inline void io_wait(void);

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */


/*
*   static inline void io_wait(void)
*   Inputs: void
*   Return Value: none
*   Function: Forces the CPU to wait for an I/O operation to complete
*/
static inline void io_wait(void)
{
    /* TODO: This is probably fragile. */
    asm volatile ( "jmp 1f\n\t"
                   "1:jmp 2f\n\t"
                   "2:" );
}

/*
*   void i8259_init(void)
*   Inputs: void
*   Return Value: none
*   Function: Initialize the 8259 PIC
*/
void i8259_init(void){
	master_mask = MASTER_MASK;
	slave_mask = SLAVE_MASK;

	outb(ICW1, MASTER_8259_PORT);				//start the initialization sequence
	io_wait();
	outb(ICW1, SLAVE_8259_PORT);
	io_wait();

	outb(ICW2_MASTER, MASTER_8259_PORT_DATA);	// ICW2: Master PIC vector offset
	io_wait();
	outb(ICW2_SLAVE, SLAVE_8259_PORT_DATA);		// ICW2: Slave PIC vector offset
	io_wait();

	outb(ICW3_MASTER, MASTER_8259_PORT_DATA);	// ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(ICW3_SLAVE, SLAVE_8259_PORT_DATA);		// ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();

	outb(ICW4, MASTER_8259_PORT_DATA);
	io_wait();
	outb(ICW4, SLAVE_8259_PORT_DATA);
	io_wait();

	outb(master_mask, MASTER_8259_PORT_DATA);	// restore saved masks.
	outb(slave_mask, SLAVE_8259_PORT_DATA);		
	
}


/*
*   enable_irq(uint32_t irq_num)
*   Inputs: the IRQ number that needs to be enabled on slave or master PIC
*   Return Value: none
*   Function: Enable (unmask) the specified IRQ 
*/
void enable_irq(uint32_t irq_num){
	if(irq_num < PORT_NUM){	//IRQ in at master PIC
        master_mask &= ~(1 << irq_num);
    }
    else{	//IRQ is at slave PIC
        slave_mask &= ~(1 << (irq_num-PORT_NUM));
        master_mask &= ~(1 << irq_num);
    }
    outb(master_mask, MASTER_8259_PORT_DATA);	//output the value to the port
    outb(slave_mask, SLAVE_8259_PORT_DATA);
}


/*
*   disable_irq(uint32_t irq_num)
*   Inputs: The IRQ numbers that need to be disabled on slave or master PIC
*   Return Value: none
*   Function: Disable (mask) the specified IRQ 
*/
void disable_irq(uint32_t irq_num){
    if(irq_num < PORT_NUM){	//if irq_num < 8, disable the master port data
        master_mask = master_mask | (1 << irq_num);
    } 
    else{	//if irq_num >= 8, disable the slave port data
    	slave_mask = slave_mask | (1 << (irq_num-PORT_NUM));
    	master_mask = master_mask | (1 << irq_num);
    }
    outb(master_mask, MASTER_8259_PORT_DATA);	//output the value to the port
    outb(slave_mask, SLAVE_8259_PORT_DATA);	//output the value to the port
}


/*
*   send_eoi(uint32_t irq_num)
*   Inputs: The irq number that needs to be sent EOI
*   Return Value: none
*   Function: Send end-of-interrupt signal for the specified IRQ
*/
void send_eoi(uint32_t irq_num){
	if(irq_num >= PORT_NUM){	//if the IRQ came from the Slave PIC, it is necessary to issue the command to both PIC chips.
		outb(EOI | (irq_num - PORT_NUM), SLAVE_8259_PORT);
		outb(EOI | PIN_NUM, MASTER_8259_PORT);
	}	
	else{	//If the IRQ came from the Master PIC, it is sufficient to issue this command only to the Master PIC
		outb(EOI | irq_num, MASTER_8259_PORT);
	}
}
