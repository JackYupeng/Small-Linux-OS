#include "keyboard.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "tests.h"
#include "terminal.h"
#include "schedule.h"
struct kboard{
	char pressed;
	char released;
	unsigned long ascii_number;
	
};

static struct kboard keyboard_sc[chara];	//36 charater to print out
static struct kboard keyboard_Cap[chara];   //36 captial charater to print out.
static int caps_lock_flags = 0;				//for checking the caps_pressed
static int shift_flags	   = 0;				//for checking shift_pressed
static int ctrl_flags	   = 0;				//for checking ctrl_pressed
static int alt_flags       = 0;

static unsigned char keyboardbuf[bufsize];  //keyboard buff for saving the character
static int buf_index = 0 ;					//buffer index
static int count = 0;						//count how many charater in buffer
static int vd_x;							//position x
static int vd_y;							//position y
static int enter_flag = 0;

int terminal_flag = 0;

//array to store the character.
static char character_pressed_array[letters]=
{char_a,char_b,char_c,char_d,char_e,char_f,
char_g,char_h,char_i,char_j,char_k,char_l,
char_m,char_n,char_o,char_p,char_q,char_r,
char_s,char_t,char_u,char_v,char_w,char_x,
char_y,char_z,};



/*
 * ini_keyboard()
 *   DESCRIPTION: initialnize the array for saving the scancode and ascillnumber for keyboard
 *   INPUTS: none(ignored)
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void ini_keyboard()
{
	int x;
	int index;
	char number_pressed = char_start;	//char start
	char number_released = char_release;//char release start
	char number_ascii    = num_ascill;// ascii number start
	
	char character_ascii = char_ascill;//character ascii number start
	
	enable_irq(1);
	clear();							// clear the screen
	reset_position(0,0);				// reset the typing position
	reset_cursor(0,0);					// reset the cursor position
	vd_x = 0;							// set the x position 0;
	vd_y = 0;							// set the y position 0;
	clearbuffuer();						// clearbuffer
	
	//save the 1 - 9 in the struct;
	for(x = 0;x<number_count;x++)
	{
		keyboard_sc[x].pressed = number_pressed;	//save the presswed scan code
		keyboard_sc[x].released = number_released;	//save the realease scan code
		keyboard_sc[x].ascii_number = number_ascii; //save the ascii scan code
		//next number;
		number_ascii++;
		number_pressed++;
		number_released++;
	}
	
	// save the 0;
	keyboard_sc[number_count].pressed = zero_pressed;
	keyboard_sc[number_count].released = zero_released;
	keyboard_sc[number_count].ascii_number = zero_ascii;
	
	
	
	// save the a - z in the struct
	for(x = number_count+1;x<chara;x++)
	{
		//save the character a - z in the struct array.
		index = x-number_count-1;
		keyboard_sc[x].ascii_number = character_ascii;
		keyboard_sc[x].pressed = character_pressed_array[index];
		keyboard_sc[x].released = character_pressed_array[index]+released_d;
		character_ascii++;
	}

	
	
	///////for the captital case:
	
	// this is the number 0 - number 9 pressed scan_code.
	keyboard_Cap[numb_0].pressed = numb_0_pressed;
	keyboard_Cap[numb_1].pressed = numb_1_pressed;
	keyboard_Cap[numb_2].pressed = numb_2_pressed;
	keyboard_Cap[numb_3].pressed = numb_3_pressed;
	keyboard_Cap[numb_4].pressed = numb_4_pressed;
	keyboard_Cap[numb_5].pressed = numb_5_pressed;
	keyboard_Cap[numb_6].pressed = numb_6_pressed;
	keyboard_Cap[numb_7].pressed = numb_7_pressed;
	keyboard_Cap[numb_8].pressed = numb_8_pressed;
	keyboard_Cap[numb_9].pressed = numb_9_pressed;
	
	// this is the number 0 - number 9 asciill number
	keyboard_Cap[numb_0].ascii_number = numb_0_ascii;
	keyboard_Cap[numb_1].ascii_number = numb_1_ascii;
	keyboard_Cap[numb_2].ascii_number = numb_2_ascii;
	keyboard_Cap[numb_3].ascii_number = numb_3_ascii;
	keyboard_Cap[numb_4].ascii_number = numb_4_ascii;
	keyboard_Cap[numb_5].ascii_number = numb_5_ascii;
	keyboard_Cap[numb_6].ascii_number = numb_6_ascii;
	keyboard_Cap[numb_7].ascii_number = numb_7_ascii;
	keyboard_Cap[numb_8].ascii_number = numb_8_ascii;
	keyboard_Cap[numb_9].ascii_number = numb_9_ascii;
	
	// this is the number 0 - number 9 released scan_code.
	keyboard_Cap[numb_0].released = numb_0_released;
	keyboard_Cap[numb_1].released = numb_1_released;
	keyboard_Cap[numb_2].released = numb_2_released;
	keyboard_Cap[numb_3].released = numb_3_released;
	keyboard_Cap[numb_4].released = numb_4_released;
	keyboard_Cap[numb_5].released = numb_5_released;
	keyboard_Cap[numb_6].released = numb_6_released;
	keyboard_Cap[numb_7].released = numb_7_released;
	keyboard_Cap[numb_8].released = numb_8_released;
	keyboard_Cap[numb_9].released = numb_9_released;
	
	character_ascii = sta_asc;			//first character ascii'
	
	// saving the cap character into the struct
	for(x = sta_ind ; x < chara; x++)
	{
		keyboard_Cap[x].ascii_number = character_ascii;
		keyboard_Cap[x].pressed = character_pressed_array[x-sta_ind];
		keyboard_Cap[x].released = character_pressed_array[x-sta_ind]+released_d;
		character_ascii++;
	}

	
}




/*
 * print_keyboard()
 *   DESCRIPTION: get the scancode and then print out the ascii number from the done array.
 *   INPUTS: none(ignored)
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void print_keyboard()
{
	unsigned char c = inb(scan_port);
	send_eoi(1);
	int x;
	char ascii_print;

	//if scan code shift is released set the flag to 0	
	if(c == leftshift_released || c== rightshift_released)
	{
		shift_flags = 0;
	}
	
	//if scan code ctrll is released set the flag to 0
	if(c == ctrll_released)
	{
		ctrl_flags = 0;
	}

	if(c == alt_released)
	{
		alt_flags = 0;
	}
	
	//if ohter released scan code just return;
	if(c > scan_released)
	{
		return;
	}
	
	//if scan code caps read ,set the caps flag.
	if(c == caps )
	{
		caps_lock_flags = 1 - caps_lock_flags;
	}

	//if the shift codd read, set the flag to 1;
	if(c == leftshift || c== rightshift)
	{
		shift_flags = 1;
	}
	
	//if control scan code read, set the flag to 1;
	if(c == ctrll )
	{
		ctrl_flags = 1;
	}		

	if(c == alt_pressed)
	{
		alt_flags = 1;
	}
	
	//if the key enter is pressed, do the following
	if(c == enter_pressed)
	{	
	

		// copy the keyboard buffer to terminal buffer;
		prout('\n');
		
		copy_to_teminal(keyboardbuf,buf_index);
		//putc('\n');
		//printf("%d \n",buf_index);
		check_terminal(vd_y);
		//clear buffer
		clearbuffuer();
		
		//this case is not needed to scroll
		if(vd_y <= screen_width+1)
		{
		vd_x = 0;
		vd_y++;
		reset_cursor(vd_x,vd_y);
		buf_index=0;
		count = 0;
		return;
		}
		//this case is needed to scroll
		else
		{
		//reset the videomeomery
		if(enter_flag == 0)
		{
		reset_video();
		//clear the last row.
		clear_row();
		}
		vd_x = 0;
		vd_y = screen_width+1;
		reset_cursor(vd_x,vd_y);
		reset_position(vd_x,vd_y);
		buf_index = 0;
		count = 0;
		enter_flag = 0;
		return;
		}
	}

	if(alt_flags ==1 )
	{
		if(c == Ftwo_pressed)
		{
		copy_keyboard_buf(keyboardbuf,buf_index);
		switch_to_terminal_2();
		return;
		}
		if(c == Fone_pressed)
		{
			copy_keyboard_buf(keyboardbuf,buf_index);
			switch_to_terminal_1();
		return;
		}
		if(c == Fthree_pressed)
		{	copy_keyboard_buf(keyboardbuf,buf_index);
			switch_to_terminal_3();
			return;
		}
			
	}

	
	//The case when ctrl_flags is 1
	if(ctrl_flags == 1)
	{		
			//case when control and L pressed together
			//clear the screen
			if(c == l_pressed )
			{
				clear();
				reset_cursor(0,0);
				reset_position(0,0);
				count=0;
				buf_index=0;
				vd_x=0;
				vd_y=0;
				return;
			}
	}
	
	//space
	if(c ==space_pressed)
	{
		ascii_print = space_ascii;
		prout(ascii_print);
	}
	
	//backspace;
	if(c == backspace_pressed )
	{
		backspace();
	}
	
	//The case shift flag is 1
	if(shift_flags ==1)
{
	for(x = 0 ; x<chara ; x++)
	{	
	//the ~ 
	if(c == top_pressed)
	{	
		ascii_print = top_ascii;
		prout(ascii_print);
		return;
	}
	//the _ 
	if(c == underline_pressed)
	{
		ascii_print = underline_ascii;
		prout(ascii_print);
		return;
	}	
	//the + 
	if(c == add_pressed)
	{
		ascii_print = add_ascii;
		prout(ascii_print);
		return;
	}
	//the { 
	if(c == le_pressed)
	{
		ascii_print = le_ascii;
		prout(ascii_print);
		return;
	}	
	//the } 
	if(c == ri_pressed)
	{
		ascii_print = ri_ascii;
		prout(ascii_print);
		return;
	}
	
	//the | 
	if(c == mid_pressed)
	{
		ascii_print = mid_ascii;
		prout(ascii_print);
		return;
	}
	//the : 
	if(c == colon_pressed)
	{
		ascii_print = colon_ascii;
		prout(ascii_print);
		return;
	}
	//the " 
	if(c == marks_pressed)
	{
		ascii_print = marks_ascii;
		prout(ascii_print);
		return;
	}

	//the < 
	if(c == less_pressed)
	{
		ascii_print = less_ascii;
		prout(ascii_print);
		return;
	}	
	
	//the > 
	if(c == larger_pressed)
	{
		ascii_print = larger_ascii;
		prout(ascii_print);
		return;
	}	
	
	//the ? 
	if(c == que_pressed)
	{
		ascii_print = que_ascii;
		prout(ascii_print);
		return;
	}
		// then check which key is pressing
		if(c == keyboard_Cap[x].pressed)
		{
			//get the ascii number and print out
			ascii_print = keyboard_Cap[x].ascii_number;
			prout(ascii_print);
			//set the flag to 1.
		
			break;
		}
		
	}	
}
	
	else
	
{
	//the case when caps flag is 1
	if(caps_lock_flags == 1)
	{
	
	//the ` 
	if(c == top_down_pressed)
	{
		ascii_print = top_down_ascii;
		prout(ascii_print);
		return;
	}
	//the - 
	if(c == min_pressed)
	{
		ascii_print = min_ascii;
		prout(ascii_print);
		return;
	}	
	//the = 
	if(c == equal_pressed)
	{
		ascii_print = equal_ascii;
		prout(ascii_print);
		return;
	}	
	
	//the [ 
	if(c == lef_pressed)
	{
		ascii_print = lef_ascii;
		prout(ascii_print);
		return;
	}	
	//the ] 
	if(c == rig_pressed)
	{
		ascii_print = rig_ascii;
		prout(ascii_print);
		return;
	}
	
	
	//the ; 
	if(c == colons_pressed)
	{
		ascii_print = colons_ascii;
		prout(ascii_print);
		return;
	}
	//the ' 
	if(c == mark_pressed)
	{
		ascii_print = mark_ascii;
		prout(ascii_print);
		return;
	}

	//the , 
	if(c == dit_pressed)
	{
		ascii_print = dit_ascii;
		prout(ascii_print);
		return;
	}	
	
	//the . 
	if(c == spot_pressed)
	{
		ascii_print = spot_ascii;
		prout(ascii_print);
		return;
	}	
	
	//the / 
	if(c == slash_pressed)
	{
		ascii_print = slash_ascii;
		prout(ascii_print);
		return;
	}	
	
	//the | lower 
	if(c == mid_pressed)
	{
		ascii_print = mid_low_ascii;
		prout(ascii_print);
		return;
	}
	
	for(x = 0 ; x < sta_ind ; x++)
	{
		if(c == keyboard_sc[x].pressed)
		{
			//get the ascii number and print out
			ascii_print = keyboard_sc[x].ascii_number;
			prout(ascii_print);
			//set the flag to 1.
			break;
		}
	}
	for(x = sta_ind ; x<chara ; x++)
	{	

		// then check which key is pressing
		if(c == keyboard_Cap[x].pressed)
		{
			//get the ascii number and print out
			ascii_print = keyboard_Cap[x].ascii_number;
			prout(ascii_print);
			//set the flag to 1.
			break;
		}
		
	}
			
	}
	else
	{
	
	//the ` 
	if(c == top_pressed)
	{
		ascii_print = top_lower_ascii;
		prout(ascii_print);
		return;
	}
	//the - 
	if(c == underline_pressed)
	{
		ascii_print = underline_lower_ascii;
		prout(ascii_print);
		return;
	}	
	//the = 
	if(c == add_pressed)
	{
		ascii_print = add_lower_ascii;
		prout(ascii_print);
		return;
	}
	
	//the [ 
	if(c == le_pressed)
	{
		ascii_print = le_lower_ascii;
		prout(ascii_print);
		return;
	}	
	//the ] 
	if(c == ri_pressed)
	{
		ascii_print = ri_lower_ascii;
		prout(ascii_print);
		return;
	}
	
	
	//the ; 
	if(c == colon_pressed)
	{
		ascii_print = colon_lower_ascii;
		prout(ascii_print);
		return;
	}
	//the ' 
	if(c == marks_pressed)
	{
		ascii_print = marks_lower_ascii;
		prout(ascii_print);
		return;
	}

	//the , 
	if(c == less_pressed)
	{
		ascii_print = less_lower_ascii;
		prout(ascii_print);
		return;
	}	
	
	//the . 
	if(c == larger_pressed)
	{
		ascii_print = larger_lower_ascii;
		prout(ascii_print);
		return;
	}	
	
	//the / 
	if(c == que_pressed)
	{
		ascii_print = que_lower_ascii;
		prout(ascii_print);
		return;
	}	
	
	//the | lower 
	if(c == 0x2B)
	{
		ascii_print = mid_low_ascii;
		prout(ascii_print);
		return;
	}
	
	
	
	for(x = 0 ; x<chara ; x++)
	{	

		// then check which key is pressing
		if(c == keyboard_sc[x].pressed)
		{
			//get the ascii number and print out
			ascii_print = keyboard_sc[x].ascii_number;
			prout(ascii_print);
			//set the flag to 1.
			break;
		}
		
	}
	}
}

}


/*
 * put_to_buf(uint8_t c )
 *   DESCRIPTION:put the character into the keyboard buffer
 *   INPUTS: unsiged character
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

void put_to_buf(uint8_t c )
{
	if(buf_index <bufsize)
	{
	keyboardbuf[buf_index] = c;
	buf_index ++;	
	}
	return ;
}


/*
 * prout(uint8_t c)
 *   DESCRIPTION: this function does the following :
	1 print out the charcter
	2 resert the cursor posion;
	3 reset the typing position;
	4 saving the character into the buffer
 *   INPUTS: unsigned character
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */


void prout(uint8_t c)
{	

	if(count == bufsize)
	{
	//reset the cursor

	reset_cursor(vd_x,vd_y);
	return;
	}
	if(buf_index<=bufsize-1)
	//save the character in buffer
	keyboardbuf[buf_index]= c;
	buf_index++;
	vd_x++;
	if(vd_x==screen_legth)
		{
		//putc out the charater and reset the position and cursor
		putc(c);
		count++;
		vd_y++ ;
		vd_x= 0;
		if(vd_y == 25)
		{
			reset_video();
			clear_row();
			reset_position(vd_x,vd_y-1);
			reset_cursor(vd_x,vd_y-1);
			return;
		}
			reset_position(vd_x,vd_y);
			reset_cursor(vd_x,vd_y);		

		}
	else
		{
		putc(c);
		count++;
		reset_cursor(vd_x,vd_y);
		}
		
	return;
	
}



/*
 * backspace()
 *   DESCRIPTION: handdle the backspace key.
 *   INPUTS: none(ignored)
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */


void backspace()
{	
	if(count==0)
		return;

	//the case that not need to do anything
	if(vd_x == 0 && vd_y == 0)
		return;
	if(vd_x == 0 && count ==0 && buf_index ==0)
		return;
	
	if(vd_x==0)
	{
	
	//
	put_upbackspace();
	vd_x = screen_legth-1;
	vd_y --;
	reset_position(vd_x,vd_y);
	reset_cursor(vd_x,vd_y);
	if(count>0 && buf_index>0)
	{count--;
	buf_index--;keyboardbuf[buf_index] = NULL;}
	}	
	else
	{
	
	put_backspace();
	vd_x --;
	reset_position(vd_x,vd_y);
	reset_cursor(vd_x,vd_y);	
	if(count>0 && buf_index>0)
	{count--;
	buf_index--;keyboardbuf[buf_index] = NULL;}
	}
	
}


/*
 * ini_clearbuffuer()
 *   DESCRIPTION: when enter is pressed this function will clear the buffer.
 *   INPUTS: none(ignored)
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */


void clearbuffuer()
{
	int x = 0;
	for(x = 0; x<bufsize; x++)
	{
		keyboardbuf[x] = NULL;
	}
}


/*
 * copy_position(int x, int y)
 *   DESCRIPTION: it will copy the most updated position from other file.
 *   INPUTS: x position and y position 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */


void copy_position(int x, int y)
{
	vd_x = x;
	vd_y = y;
}

/*keyboard_write
 * 
 *   DESCRIPTION: set enter_flag to 1
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void keyboard_write()
{
	enter_flag = 1;
}

int get_buf_index()
{
	return buf_index;
}

void 	clearbufferindex()
{
	buf_index = 0;
}

void copy_to_keyboard_buf(int index,unsigned char *key_buf)
{
	int i = 0;
	for(i = 0 ; i < index; i++)
	{
		keyboardbuf[i] = key_buf[i];
	}
	buf_index = index;
	return;
}
