#ifndef TESTS_H
#define TESTS_H
#include "lib.h"
#include "types.h"
#include "filesystem.h"
// test launcher
void launch_tests();
void page_tests();
void page_test_pass();
void check_divide_zero();
void file_test();
void test_terminal();
void rtc_test();
void check_terminal(int vd_y);

void read_file_by_name();


extern dentry_t* new_dentry;

#endif /* TESTS_H */
