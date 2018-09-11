#include "filesystem.h"
#include "syscall.h"
#define block_size 4096
#define B_4      4
#define B_63   63
#define B_32   32
boot_block_t *file_sys;
uint32_t num_dir_entries;
uint32_t num_inodes;
uint32_t num_data_blocks ;
uint32_t dentry_addr;
int dir_read_loc=0;
/*Description: initial file system by setting the correct address to the struct 
 *input: addr of the boot block
 *output: none
 *side effects: none
 *
 */
void file_init(uint32_t addr){
	
	dentry_addr = addr;
	
	file_sys = (boot_block_t*)addr;
	
	return;

}

/*Description: read the directory
 *input: buffer, offset
 *output: none
 *side effects: none
 *
 */
int32_t dir_read (int32_t fd , void* buf, int32_t nbytes){
	/*
	int i;
	for(i=0; i<17; i++){
		printf("file name: %s, file type: %u, file_size: %u\n", file_sys->dentry[i].file_name,
				file_sys->dentry[i].file_type, file_size(file_sys->dentry[i].inode));

	}
	*/
	dentry_t temp_dentry;
	uint32_t file_length;
	int i;
	// if(fd>=8 || fd<0){
	// 	return -1;
	// }
	//printf("%u", dir_read_loc );
	
	if(read_dentry_by_index(dir_read_loc, &temp_dentry) !=0){
		dir_read_loc=0;
		return 0;
	}
	else{
		for(i=0; i<B_32+1;i++){
			((int8_t*)(buf))[i]='\0';
		}
		file_length=(uint32_t)strlen((int8_t*)temp_dentry.file_name);
		if(file_length>B_32){
			file_length=B_32;
		}
		strncpy((int8_t *)buf, (int8_t*)temp_dentry.file_name, file_length+1);
		dir_read_loc++;
		
		return file_length;
	}
}
	


/*Description: writes to directory
 *input: none
 *output: none
 *side effects: none
 *
 */
int32_t dir_write (int32_t fd, const void* buf, int32_t nbytes){
	return -1;
}

/*Description: open direcotry
 *input: addr of the boot block
 *output: none
 *side effects: none
 *
 */
int32_t dir_open(const uint8_t* filename){
	
	return 0;
}
/*Description: close directory
 *input: none
 *output: none
 *side effects: none
 *
 */
int32_t dir_close(int32_t fd){
	return 0;
}



/*Description: open a file
 *input: none
 *output: none
 *side effects: none
 *
 */
int32_t file_open(const uint8_t* filename){

	return 0;

}

/*Description: close a file
 *input: addr of the boot block
 *output: none
 *side effects: none
 *
 */
int32_t file_close(int32_t fd){
	return 0;
}

/*Description: write to a file
 *input: none
 *output: none
 *side effects: none
 *
 */
int32_t file_write (int32_t fd, const void* buf, int32_t nbytes){
	return -1;
}

/*Description: read a file
 *input: name of file, buf to store data, offset starts from 
 *output: -1 for invalid, 0 for sucess
 *side effects: none
 *
 */
int32_t file_read (int32_t fd , void* buf, int32_t nbytes){
	/*
	dentry_t* dentry;

	if (read_dentry_by_name(fname, dentry) == 0){
		uint32_t temp;
		temp=read_data(dentry->inode, 0 , buf , nbytes);
		return temp;
	}
	return -1;
	*/
	//printf("file_read" );
	//uint32_t read=0;
	//printf("first:%u", read );
	uint32_t offset = terminals[curr_terminal].cur_pcb->fd[fd].file_position;
	uint32_t inode = terminals[curr_terminal].cur_pcb->fd[fd].inode;

	
	//printf("%u", offset);
	//printf("%u", inode );
	uint32_t read= read_data(inode, offset, (uint8_t*)buf, nbytes);
	//printf("rrr" );
	//printf("read:%u", read );
	terminals[curr_terminal].cur_pcb->fd[fd].file_position =terminals[curr_terminal].cur_pcb->fd[fd].file_position + read;
	if(terminals[curr_terminal].cur_pcb->fd[fd].file_position > file_size(terminals[curr_terminal].cur_pcb->fd[fd].inode)){
		terminals[curr_terminal].cur_pcb->fd[fd].file_position =0;
		return 0;
	}
	// if(current_pcb->fd[fd].file_position == file_size(current_pcb->fd[fd].inode)){
	// 	current_pcb->fd[fd].file_position =0;
	// }
	return (int32_t)read;
}

/*Description: read a directory entry by name of the file
 *input: name of a file, struct of dentry
 *output: -1 for invalid, 0 for sucess
 *side effects: none
 *
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
		int i; /*for loop index*/

		/* check invalid first*/
		if(dentry==NULL || fname ==NULL){
			return -1;
		}
		/*compare the filenames to see if match up. If so, copy that to the empty dentry.*/
		for(i=0; i< B_63; i++){
			if(strncmp( (int8_t*)fname, (int8_t*)file_sys->dentry[i].file_name, B_32)==0){
				
				strncpy( (int8_t*) dentry->file_name,(int8_t*)fname, B_32);
				dentry->file_type=file_sys->dentry[i].file_type;
				dentry->inode=file_sys->dentry[i].inode;
				return 0;
			}
		}
		/*else return invalid*/
		return -1;
}

/*Description: read a direcotry entry by index 
 *input: index of the dnetry, struct of dentry
 *output: -1 for invalid, 0 for sucess
 *side effects: none
 *
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
	/*check invalid situation first*/
	if(index <0 || index >= file_sys->num_dentry){
		return -1;
	}
	//printf("%u", file_sys->dentry[index].file_name );

	/*copy the dentry information to the new empty dentry*/
	strncpy( (int8_t*) dentry->file_name,(int8_t*)file_sys->dentry[index].file_name,B_32);
	dentry->file_type=file_sys->dentry[index].file_type;
	dentry->inode=file_sys->dentry[index].inode;

	return 0;

}

/*Description: read data in a file 
 *input: inode number of the file, offset starts from, buf to store data, length to read
 *output: how many data read, -1 for invalid, 0 for sucess
 *side effects: none
 *
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	
	uint32_t inode_length =  *(uint32_t*) (dentry_addr + ((inode + 1) * block_size)); /*calculate the length of data blocks*/
	//printf("%u", inode_length);
	/*check invalid situations first*/
	/*
	if(inode > num_inodes || offset >   inode_length ){
		return -1;
	}
	if(offset == inode_length){
		return 0;
	}
	*/
	if(inode > file_sys->num_inode){
		return -1;
	}
	if(offset >= inode_length){
		return 0;
	}

	uint32_t data_read =0;  //data already read and needs to be returned. 
	uint32_t data_block_num = offset / block_size; //which data block in each inode.
	uint32_t data_block_index = *(uint32_t*) (dentry_addr + ((inode + 1) * block_size) + (data_block_num + 1) * B_4); //which data block in file system. 
	

	
	uint32_t data_block_pos = offset % block_size; //the position in the data block 
 	
	uint32_t cur_pos = dentry_addr+ ((file_sys->num_inode + 1) + data_block_index) * block_size + data_block_pos; //the address in the filesystem

	uint32_t data_to_read;	// data needs to be read
	
	/*check if the length is bigger than inode length, read the smaller one*/
	if(length <= inode_length){
		data_to_read = length;
	}
	else{
		data_to_read = inode_length;
	}
	
	
	uint32_t data_remain = (data_block_num+1) * block_size - offset;	//data_remain for the first data block 
	/*check if data_remain is bigger than data we need to read*/
	if(length < inode_length){
		if(data_remain > length){
			data_remain = length;
		}
	}
	else{
		if(data_remain > inode_length){
			data_remain = inode_length;
		}
	}


	// if(length < data_remain){
	// 	data_remain = length;
	// }
	// else if(data_remain >= inode_length){
	// 	data_remain = inode_length;
	// }
	//printf("%u|", data_remain );
	memcpy(buf, (uint8_t*)cur_pos, data_remain); // copy the first block data to buf
	
	data_read = data_remain; //update data read
	//printf("%u|", data_read);
	//printf("%u|", data_to_read-data_read );
	/* copy the middle data blocks */
	//data_to_read = data_to_read - data_read;
	while((data_to_read-data_read) >= block_size){
		data_block_num++;
		data_block_index = *(uint32_t*) (dentry_addr + ((inode + 1) * block_size) + (data_block_num + 1) * B_4 );
		cur_pos = dentry_addr+ ((file_sys->num_inode + 1) + data_block_index) * block_size;
		memcpy(&buf[data_read], (uint8_t*)cur_pos, block_size);
		data_read += block_size;	//update data read
		//printf("x" );
		//data_to_read = data_to_read - data_read;
	}
	/*copy the last data block*/
	//printf("z" );

	if(data_to_read-data_read >0){
		
		data_remain= data_to_read - data_read;
		data_block_num++;
		data_block_index = *(uint32_t*) (dentry_addr + ((inode + 1) * block_size) + (data_block_num + 1) * B_4 );
		cur_pos = dentry_addr+ ((file_sys->num_inode + 1) + data_block_index) * block_size;
		//printf("y" );
		memcpy(&buf[data_read], (uint8_t*)cur_pos, data_remain);
		data_read +=data_remain;
		//data_to_read = data_to_read-data_read;
	}
	

	
	//printf("dataread:%u", data_read);
	return data_read;
	
}

/*Description: find the size of a file 
 *input: the inode number of the file
 *output: size of the file
 *side effects: none
 *
 */
int32_t file_size(uint32_t inode){
	
	uint32_t file_size=*(uint32_t*) (dentry_addr + ((inode + 1) * block_size));
	return file_size;
}
