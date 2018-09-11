#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"
#include "filesystem.h"
#include "terminal.h"
#include "rtc.h"

#define PASS 1
#define FAIL 0
#define ECE391 391
#define videome 0xB8000
#define kenme   0x400000
#define width_screen 80
#define size_long 10
/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}
	
	return result;
}

// add more tests here

/* page Test 
 * 
 *	check whether page has initialnize
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
void page_tests()
{
int a, b, c; 
int *pointer;    

a = ECE391; 
pointer = &a; 
b = *pointer; 
pointer = NULL;
c = *pointer; 
}
/* page Test 
 * 
 *	check whether page has initialnize and the value is contained in paging
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
void page_test_pass(){
	int a = 1;
	int *pointer = &a;
	if(a == (*pointer))			//check whether the values contain in paging structs
		printf("paging_init pass");
	
	pointer = (void*)videome;	//check whether the video memoery is correct
	if(1)
	printf("   paging_videomemoery pass!!   ");
	
	pointer = (int*)kenme;	// check whether the kenerl address is correct
	if(*pointer)
	{
		printf(" paging_kenerl pass");
	}
	
	
}
/* test divide by zero; 
 * 
 *	check whether the exception divided by zero will print out
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
void check_divide_zero()
{
	int a = 1;
	if(a ==0)
	return;
	//a = a / 0;
	return;
}
/* Checkpoint 2 tests */


/* file_test(); 
 * 
 *	test the filesystem
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */

 
void file_test(){
/*	
	//printf("%u|", file_sys->num_dentry);
	//printf("%u|", file_sys->num_inode);
	//printf("%u|", file_sys->data_block);
	
	uint8_t fname [] =" ";
	uint8_t buf[file_sys->num_dentry];
	int32_t nbytes=0;
	

	dir_read((const uint8_t*) fname , buf, nbytes);
	
	*/
	//dentry_t* new_dentry;
	//read_dentry_by_index (10,new_dentry);
	
	/*
	uint8_t buf[5000];
	//int32_t nbytes = 5;
	//file_read (file_sys->dentry[1].file_name, buf, nbytes);
	printf("%s", file_sys->dentry[10].file_name );
	read_data (file_sys->dentry[15].inode, 0, buf, 100);
	
	for(i=0;i<40;i++){
		printf("%c", buf[i] );
	}
	
	*/
	
	
}



/* test_terminal()
 * 
 *	test whether the terminal can print out the given string
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */

void test_terminal()
{
// the size of the string is 18 
int8_t buffer[18] ; 
int length;
//putting the string"test_terminal pass" into the buffer and then read and write.
// in 0 - 17
buffer[0] = 't' ; 
buffer[1] = 'e' ;
buffer[2] = 's' ;
buffer[3] = 't' ;
buffer[4] = '_' ;
buffer[5] = 't' ;
buffer[6] = 'e' ;
buffer[7] = 'r' ;
buffer[8] = 'm' ;
buffer[9] = 'i' ;
buffer[10] = 'n' ;
buffer[11] = 'a' ;
buffer[12] = 'l' ;
buffer[13] = ' ' ;
buffer[14] = 'p' ;
buffer[15] = 'a' ;
buffer[16] = 's' ;
buffer[17] = 's' ;
length = strlen(buffer);	
terminal_read(1,buffer,length);
terminal_test(0);
copy_position(0,1);	
reset_position(0,1);
reset_cursor(0,1);
}




/* rtc_test()
 * 
 * run the rtc test to check whether rtc is correct
 * Inputs: None
 * Outputs: PASS/FAIL
 */

void rtc_test(){

	uint32_t buf[4];
	buf[0]=2;
	int32_t fd=0;
	int32_t nbytes = 0;
	int i;
	int j;
	int x = 0 ;
	int y = 0;
	for(j = 0; j < size_long ; j++)
	{
	rtc_write (fd, buf, nbytes);
	printf("Rate: %d \n",buf[0]);
	y++;
	for(i=0;i<buf[0];i++){
		rtc_read(fd,buf,nbytes);
		if(x == width_screen)
		{
			x= 0 ;
			y ++;
		}
		printf("!");
		x++;
		reset_position(x,y);
		reset_cursor(x,y);
		
	}
	buf[0] = buf[0]*2;
	printf("\n");
	y=0;
	x=0;
		reset_position(0,0);
		reset_cursor(0,0);	
		clear();
	}

}




/* test divide by zero; 
 * 
 *	check whether the exception divided by zero will print out
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */

void check_terminal(int vd_y)
{	
	
	terminal_test(vd_y);
	return;
}


/* read_file_by_name(){
 * 
 *	check filesyste
 * Inputs: None
 * Outputs: PASS/FAIL
 */
void read_file_by_name(){
	
	uint8_t fname[]="frame0.txt";
	
	dentry_t new_dentry;
	read_dentry_by_name ((const uint8_t*)fname,  &new_dentry);

	uint8_t buf[173];

	read_data(new_dentry.inode, 0 , buf ,173);
	
	int i;
	for(i=0;i<173 ;i++){
	//printf("\n");
		printf("%c", buf[i] );
	}
		//putc(buf[i]);
	

	//terminal_write(0, (int8_t*) buf, file_size(new_dentry.inode));

}
	
/* Checkpoint 3 tests */

void bad_value()
{
	
}


/* Checkpoint 4 tests */
/* Checkpoint 5 tests */



/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	//test_terminal();

	//page_test_pass();
	//page_tests();
	//check_divide_zero();
	
	/*checkpoint 2 test*/
		//read_file_by_name();
	//file_test();
rtc_test();
	printf("Type whatever, I will print out the first five character and the whole string!!\n");
	copy_position(0,1);
	reset_cursor(0,1);
	reset_position(0,1);
	
	//printf("get here");
	//print_keyboard();
	//print_keyboard();
	//3.3
	bad_value();
	
	// launch your tests here
}

