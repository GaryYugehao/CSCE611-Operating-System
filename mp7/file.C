/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2017/05/01

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file.H"
#include "file_system.H"
extern FileSystem* FILE_SYSTEM;
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(unsigned int _file_id, unsigned int* _start_block, unsigned int* _data_block) {
    /* We will need some arguments for the constructor, maybe pointer to disk
     block with file management and allocation data. */
    Console::puts("In file constructor.\n");
	file_size = 512;
	file_id = _file_id;

	cur_pos = 0;
	info_start_block = *_start_block;
	data_start_block = *_data_block;
	cur_block = data_start_block;	//first block is used to store management information
	file_end_block = data_start_block;
	data_block_count = 1;

}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char * _buf) {
    Console::puts("reading from file\n");
	unsigned char* read_buf =  new unsigned char[512];
	unsigned int read_pos = 0;
	while(read_pos < _n){
		FILE_SYSTEM->disk->read(cur_block, read_buf);
		unsigned int read_left = _n - read_pos;
		unsigned int byte_left = 508 - cur_pos;
		if(read_left <= byte_left){
			memcpy(_buf+read_pos, read_buf+cur_pos, read_left);
			read_pos += read_left;
			cur_pos += read_left;
			if(read_pos == _n){
				return _n;
			}
		}else{
			memcpy(_buf + read_pos, read_buf+cur_pos, byte_left);
			read_pos += byte_left;
			cur_pos = 0;
			memcpy(&cur_block, read_buf+508, 4);
		}
	}

	return _n;
}


void File::Write(unsigned int _n, const char * _buf) {
	unsigned char* write_buf = new unsigned char[512];
	unsigned int write_pos = 0;
	while(write_pos < _n){

	(*FILE_SYSTEM).disk->read(cur_block, write_buf);

	unsigned int write_left = _n - write_pos;
	unsigned int byte_left = 508 - cur_pos;

	if(write_left <= byte_left){

		memcpy(write_buf+cur_pos, _buf+write_pos, write_left);

		(*FILE_SYSTEM).disk->write(cur_block, write_buf);

		write_pos += write_left;
		cur_pos   += write_left;

		if(write_pos == _n){
			Console::puts("\nwe've finished writing");
		}
	}else{
		memcpy(write_buf+cur_pos, _buf + write_pos, byte_left);
		FILE_SYSTEM->disk->write(cur_block, write_buf);
	
		write_pos += byte_left;
		cur_pos = 0;

		cur_block = (*FILE_SYSTEM).free_block;
		(*FILE_SYSTEM).disk->read((*FILE_SYSTEM).free_block, write_buf);
		memcpy(&((*FILE_SYSTEM).free_block), write_buf+508, 4);
		data_block_count += 1;
	}	

	}

	if(((data_block_count-1)*512+cur_pos) > file_size){
		Console::puts("\nexceeding file size, we need bigger file size\n");
		file_size = data_block_count * 512 + cur_pos;
		file_end_block = cur_block;
	}

}

void File::Reset() {
    Console::puts("reset current position in file\n");
	cur_block = data_start_block;	//first block is used to store management information
   	cur_pos = 0;
}

void File::Rewrite() {
    Console::puts("erase content of file\n");
	unsigned char* temp_write_buf = new unsigned char[512];

	unsigned char* temp_read_buf = new unsigned char[512];
	unsigned int rw_cur_block = data_start_block;
	unsigned int rw_next_block = 0;
	int i = 0;
	for(; i < data_block_count; i++){

		if((int)rw_next_block == -1) break;
		(*FILE_SYSTEM).disk->read(rw_cur_block, temp_read_buf);
		memcpy(&rw_next_block, temp_read_buf+508, 4);

		memcpy(temp_write_buf+508, &((*FILE_SYSTEM).free_block), 4);
		(*FILE_SYSTEM).disk->write(rw_cur_block,temp_write_buf);
	
		if(data_start_block != rw_cur_block){
			(*FILE_SYSTEM).free_block = rw_cur_block;
		}
		
		rw_cur_block = rw_next_block;
	}

	file_size = 512;
	cur_pos = 0;
	cur_block = data_start_block;	//first block is used to store management information
	file_end_block = data_start_block;
	data_block_count = 1;
	
}


bool File::EoF() {
    Console::puts("testing end-of-file condition\n");
	return (data_block_count * 512 + cur_pos) == file_size;

}

unsigned int File::FileId(){
	return file_id;
}

unsigned int File::InfoBlock(){
	return info_start_block;
}

unsigned int File::BlockCount(){
	return data_block_count;
}
