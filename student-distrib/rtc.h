#ifndef RTC_H
#define RTC_H

#include "lib.h"
#include "i8259.h"
#include "x86_desc.h"


/*intialize real time clock, enable master and slave IRQs.*/
void rtc_init(); 

/*RTC interrupt handler*/
void rtc_handle_interrupt(); 

/*test_interrupt function from lib.c*/
extern void test_interrupts();	



/*open the RTC device driver*/
int32_t rtc_open(const uint8_t* filename);

/*wait for intturupt to occur*/
int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes);

/*Change the clock frequency*/
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes);

/*close the RTC driver and disable interrupts*/
int32_t rtc_close (int32_t fd);


#endif
