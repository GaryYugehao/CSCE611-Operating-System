/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2017/05/01

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
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
#include "file_system.H"
//#include "lock.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/
//char* FileSystem::block_map;
unsigned int FileSystem::free_block = 0;
file_node* FileSystem::file_list_head;
file_node* FileSystem::file_list_tail;
unsigned int FileSystem::file_count = 0;
bool FileSystem::disk_count = 0;
unsigned int FileSystem::size = 0;
Lock* FileSystem::file_system_lock;

FileSystem::FileSystem() {
    Console::puts("In file system constructor.\n");

	file_list_head = new file_node();
	file_list_tail = file_list_head;

	file_count = 0;
	disk_count = 0;

	size = 0;
	FileSystem::free_block = 2;


	return;
}

/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/

bool FileSystem::Mount(SimpleDisk * _disk) {

	if(disk_count != 0){
		return false;
	}else{
		disk = _disk;
		disk_count = 1;

		file_system_lock = new Lock();
		unsigned char* read_buf = new unsigned char[512];
		(*disk).read(0,read_buf);

		memcpy(&file_count, read_buf, 4);
		memcpy(&size, read_buf+4, 4);
	return true;
	}

}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) {

	unsigned int total_block_num = _size / 512;
	free_block = 2;	
	unsigned char* format_buf = new unsigned char[512];
	(*_disk).write(1, format_buf);
	int i = 2;
	int next_block_num = i + 1;
	for(; i < total_block_num - 1; i++){
		next_block_num = i + 1;
		memcpy(format_buf + 508, &next_block_num, 4);
		(*_disk).write(i,format_buf);
	}
	
	next_block_num = -1;
	memcpy(format_buf + 508, &next_block_num, 4);
	(*_disk).write(i, format_buf);

	unsigned int temp1 = 0;		//indicate the number of file
	memcpy(format_buf, &temp1, 4);
	unsigned int temp2 = _size;	//indicate the size of file_system
	memcpy(format_buf+4, &temp2, 4);
	//indicate which block where free block start
	memcpy(format_buf+8, &free_block, 4);
	memset(format_buf+12, 0, 500);
	(*_disk).write(0, format_buf);

    	Console::puts("formatting disk\n");

	return true;

}

File * FileSystem::LookupFile(unsigned int _file_id) {

	file_system_lock->lock();

    Console::puts("looking up file\n");
	file_node* newNode = file_list_head->next;
	if(newNode == NULL) {
		Console::putui((file_list_head->next)->file_id);
		Console::puts("\nThis is no files look up\n");
		for(;;);
	}

	for(int i = 0; i < file_count; i++){
		if(newNode->file_id == _file_id) {

			file_system_lock->unlock();
			return newNode->file;
		}
		newNode = newNode->next;
	}	
}

void FileSystem::register_file(File* file){

	file_node* newNode = new file_node();
	newNode->file_id = file->FileId();

	newNode->file = file;
	newNode->next = NULL;
	if(file_count == 0){
	
		file_list_head->next = newNode;
		file_list_tail = newNode;

	}else{
		file_list_tail->next = newNode;		
		file_list_tail = newNode;
	}	
}

bool FileSystem::CreateFile(unsigned int _file_id) {

	file_system_lock->lock();	

	unsigned char* temp_buf = new unsigned char[512];
	unsigned int info_block = free_block;
	unsigned int data_block;
	disk->read(free_block, temp_buf);
	memcpy(&data_block, temp_buf+508, 4);
	disk->read(data_block, temp_buf);
	memcpy(&free_block, temp_buf+508, 4);

 	File* newFile = new File(_file_id, &info_block, &data_block);

	register_file(newFile);
	file_count += 1;


	Console::puts("creating file\n");

	file_system_lock->unlock();

	return true;

}

void FileSystem::deregister_file(unsigned int _file_id){
	
	file_node* preNode = file_list_head;
	file_node* curNode = file_list_head->next;

	if(curNode == NULL){
		Console::puts("\nthere is no file to deregister\n");
		for(;;);
	}	
	
	int i = 0;
	for(; i < file_count; i++){
		if(curNode->file_id == _file_id){

			//once we find the file, we delete and release the occupied block
			unsigned int data_block = 0;
			unsigned int next_data_block = 0;
			unsigned char*write_buf = new unsigned char[512];

			unsigned int info_block = curNode->file->InfoBlock();
			unsigned char* read_buf = new unsigned char[512];
			disk->read(info_block, read_buf);
			memcpy(&data_block, read_buf+508, 4);
	
			memcpy(write_buf+508, &free_block, 4);
			disk->write(info_block, write_buf);
			free_block = info_block;

			for(int j = 0; j < curNode->file->BlockCount(); j++){
				disk->read(data_block, read_buf);
				memcpy(&next_data_block, read_buf+508, 4);

				memcpy(write_buf+508, &free_block, 4);
				disk->write(data_block, write_buf);
			
				free_block = data_block;
				data_block = next_data_block;	
			}

			break;
		}
		preNode = preNode->next;
		curNode = curNode->next;
	}
	
	if(curNode->next == NULL){
		preNode->next = NULL;
		file_list_tail = preNode;
	}else{

		preNode->next = curNode->next;
	}
}

bool FileSystem::DeleteFile(unsigned int _file_id) {

	file_system_lock->lock();	

	deregister_file(_file_id);
	file_count -= 1;	

	file_system_lock->unlock();
	return true;
}



