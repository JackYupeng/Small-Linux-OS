#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "lib.h"
#include "types.h"

typedef struct directory_entry
{
	uint8_t file_name[32];

	uint32_t file_type;
	uint32_t inode;
	uint8_t reserved[24];
}dentry_t;


typedef struct boot_block
{
	uint32_t num_dentry;
	uint32_t num_inode;
	uint32_t data_block;
	uint8_t reserved[52];
	dentry_t dentry[64];
}boot_block_t;

typedef	int32_t (*open_t)(const uint8_t* filename);
typedef	int32_t (*close_t)(int32_t fd);
typedef	int32_t (*read_t)(int32_t fd, void* buf, int32_t nbytes);
typedef	int32_t (*write_t)(int32_t fd, const void* buf, int32_t nbytes);

typedef struct file_operation_table
{
	open_t  open; 
	close_t	close;
	read_t  read;
	write_t	write;
}file_op_t;


typedef struct file_descriptor{
	file_op_t * op_table;
	uint32_t inode;
	uint32_t file_position;
	uint32_t flags;

}file_descriptor_t;
/*
typedef struct file_descriptor_array{
	file_descriptor_t fd_array[8];
}fd_t;
*/
extern boot_block_t *file_sys;
void file_init(uint32_t addr);
int32_t file_open (const uint8_t* filename);
int32_t file_close(int32_t fd);
int32_t file_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t file_read (int32_t fd, void* buf, int32_t nbytes);
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t file_size(uint32_t inode);
int32_t dir_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_close(int32_t fd);
int32_t dir_read (int32_t fd , void* buf, int32_t nbytes);
int32_t dir_open(const uint8_t* filename);
#endif 
