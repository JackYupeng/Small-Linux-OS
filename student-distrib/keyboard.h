#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include "lib.h"

#define alt_pressed 0x38
#define alt_released 0xB8
#define Fone_pressed 0x3b
#define Ftwo_pressed 0x3C
#define Fthree_pressed 0x3D

#define char_a   0x1E
#define char_b   0x30
#define char_c   0x2E
#define char_d   0x20
#define char_e   0x12
#define char_f   0x21
#define char_g   0x22
#define char_h   0x23
#define char_i   0x17
#define char_j   0x24
#define char_k   0x25
#define char_l   0x26
#define char_m   0x32
#define char_n   0x31
#define char_o   0x18
#define char_p   0x19
#define char_q   0x10
#define char_r   0x13
#define char_s   0x1F
#define char_t   0x14
#define char_u   0x16
#define char_v   0x2F
#define char_w   0x11
#define char_x   0x2D
#define char_y   0x15
#define char_z   0x2C

#define numb_0  0
#define numb_1  1
#define numb_2  2
#define numb_3  3
#define numb_4  4
#define numb_5  5
#define numb_6  6
#define numb_7  7
#define numb_8  8
#define numb_9  9

#define numb_0_pressed  0x0B
#define numb_1_pressed  0x02
#define numb_2_pressed  0x03
#define numb_3_pressed  0x04
#define numb_4_pressed  0x05
#define numb_5_pressed  0x06
#define numb_6_pressed  0x07
#define numb_7_pressed  0x08
#define numb_8_pressed  0x09
#define numb_9_pressed  0x0A

#define numb_0_released  0x8B
#define numb_1_released 0x82
#define numb_2_released  0x83
#define numb_3_released  0x84
#define numb_4_released  0x85
#define numb_5_released  0x86
#define numb_6_released  0x87
#define numb_7_released  0x88
#define numb_8_released  0x89
#define numb_9_released 0x8A

#define numb_0_ascii  0x29
#define numb_1_ascii 0x21
#define numb_2_ascii  0x40
#define numb_3_ascii 0x23
#define numb_4_ascii  0x24
#define numb_5_ascii  0x25
#define numb_6_ascii  0x5E
#define numb_7_ascii  0x26
#define numb_8_ascii  0x2A
#define numb_9_ascii 0x28


#define char_start 0x02
#define char_release 0x82
#define num_ascill 0x31
#define char_ascill 0x61
#define number_count 9
#define zero_pressed 0x0B
#define zero_released 0x8B
#define zero_ascii 0x30
#define chara 36
#define letters 26
#define released_d 0x80
#define scan_port 0x60
#define scan_released 0x80
#define bufsize 128
#define sta_asc 0x41
#define sta_ind 10



#define leftshift_released 0xAA
#define rightshift_released 0xB6
#define ctrll_released  0x9D
#define caps   0x3A
#define leftshift 0x2A
#define rightshift 0x36
#define ctrll 0x1D
#define enter_pressed 0x1C
#define screen_width 23
#define screen_legth 80
#define l_pressed 0x26
#define space_pressed 0x39
#define space_ascii 0x20
#define backspace_pressed 0x0E

#define top_pressed 0x29
#define top_ascii 0x7E	//~
#define top_lower_ascii 0x60

#define underline_pressed 0x0C
#define underline_ascii 0x5F
#define underline_lower_ascii 0x2D

#define add_pressed 0x0D
#define add_ascii 0x2B
#define add_lower_ascii 0x3D

#define le_pressed 0x1A
#define le_ascii 0x7B
#define le_lower_ascii 0x5B

#define ri_pressed 0x1B
#define ri_ascii  0x7D
#define ri_lower_ascii 0x5D

#define mid_pressed 0x2B
#define mid_ascii  0x7C
#define mid_low_ascii 0x5C

#define colon_pressed 0x27
#define colon_ascii 0x3A
#define colon_lower_ascii 0x3B

#define marks_pressed 0x28
#define marks_ascii 0x22
#define marks_lower_ascii 0x27

#define less_pressed 0x33
#define less_ascii  0x3C
#define less_lower_ascii 0x2C

#define larger_pressed 0x34
#define larger_ascii 0x3E
#define larger_lower_ascii 0x2E


#define que_pressed 0x35
#define que_ascii 0x3F
#define que_lower_ascii 0x2F

#define top_down_pressed 0x29
#define top_down_ascii 0x60
 
#define min_pressed 0x0C
#define min_ascii 0x2D

#define equal_pressed  0x0D
#define equal_ascii   0x3D

#define lef_pressed 0x1A
#define lef_ascii 0x5B

#define rig_pressed 0x1B
#define rig_ascii 0x5D

#define colons_pressed 0x27
#define colons_ascii 0x3B

#define mark_pressed 0x28
#define mark_ascii 0x27

#define dit_pressed 0x33
#define dit_ascii  0x2C

#define spot_pressed 0x34
#define spot_ascii 0x2E
 
#define slash_pressed 0x35
#define slash_ascii 0x2F

extern void ini_keyboard();

extern int terminal_flag;

extern int get_buf_index();

extern void clearbufferindex();

extern void copy_to_keyboard_buf(int index,unsigned char *key_buf);

extern void print_keyboard();

extern void put_to_buf(uint8_t c );

extern void prout(uint8_t c);

extern void backspace();

extern  void clearbuffuer();

extern  void copy_position(int x, int y);

extern void keyboard_write();
#endif
