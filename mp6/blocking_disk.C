/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "scheduler.H"

extern Scheduler* SYSTEM_SCHEDULER;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
	
	block_thread_list_head = new block_node();
	block_last_thread_node = new block_node();
	block_thread_count = 0;
	SYSTEM_SCHEDULER->register_disk(this);
//	register_disk(_disk_id, SYSTEM_SCHEDULER);
	return;
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/
//add thread to the end of blocked thread list

//void BlockingDisk::register_disk(unsigned int index, Scheduler* scheduler){
//	if(index >= 2) {
//		Console::puts("There is no more than 2 disks");
//		return;
//	}else{
//		scheduler->blocking_disk[index] = this;
//	}
//}
void BlockingDisk::block(Thread* _thread){
	
	block_node *newNode = new block_node();
	newNode->thread = _thread;
	newNode->next = NULL;

	if(block_count() == 0){

		set_front(newNode);
		set_end(newNode);
		increment_thread_count();		
		
	}else{
		(end())->next = newNode;
		set_end(newNode);
		increment_thread_count();		
	}

}

void BlockingDisk::unblock(Thread* _thread){

	if(block_thread_count == 0){
		Console::puts("There is no thread in block queue");
		abort();
	}

	block_node* curNode = front();
	block_node* preNode = block_thread_list_head;
	for(int i = 0; i < block_count(); i++){
		if(curNode->thread == _thread){
			break;
		}
		preNode = preNode->next;
		curNode = curNode->next;
	}
	
	if(curNode->next == NULL){
		preNode->next = NULL;
		block_last_thread_node = preNode;
	}else{
		preNode->next = (curNode->next)->next;
	}

	decrement_thread_count();
}


void BlockingDisk::increment_thread_count(){
	block_thread_count++;
}

void BlockingDisk::decrement_thread_count(){
	block_thread_count--;	
}

int BlockingDisk::block_count(){
	return block_thread_count;
}

block_node* BlockingDisk::front(){
	return block_thread_list_head->next;
}

void BlockingDisk::set_front(block_node* first_node){
	block_thread_list_head->next = first_node;
}

block_node* BlockingDisk::end(){
	return block_last_thread_node;
}

void BlockingDisk::set_end(block_node* last_node){
	block_last_thread_node = last_node;
}

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
//	Console::puts("\nThis is before we issue operation\n");
//	for(;;);
//  SimpleDisk::read(_block_no, _buf);
	issue_operation(READ, _block_no);	
//	Console::puts("\nissued operation\n");
//	for(;;);	
//we first check if it is already ready...
//if it is 
//	if(!is_ready()){
//	Console::puts("\ncurrent thread is ");
//	Console::puti(Thread::CurrentThread()->ThreadId());	
//	Console::puts("\n");
//	for(;;);
//	if(Thread::CurrentThread()->ThreadId() == 1){
//		node* newNode = (*(SYSTEM_SCHEDULER->thread_list_head)).next;		
//		for(int i = 0; i < SYSTEM_SCHEDULER->thread_count; i++){
//			Console::puts("\nThread ");
//			Console::puti((newNode->thread)->ThreadId());
//			Console::puts(" is in the thread list\n");
//			newNode = newNode->next;
//		}
//		Console::puts("\nbefore thread 1 block\n");
//		for(;;);
//	}


	block(Thread::CurrentThread());

//	Console::puts("\ncurrent thread id ");
//	Console::puts("\nafter block thread 2, there are ");
//	Console::putui(SYSTEM_SCHEDULER->thread_count);
//	Console::puts(" threads ready, there are ");
//	Console::puti(Thread::CurrentThread()->ThreadId());
//	Console::puts(" thread blocked\n");
//	Console::puts("\n");


//	if(Thread::CurrentThread()->ThreadId() == 1){
//		node* newNode = (*(SYSTEM_SCHEDULER->thread_list_head)).next;		
//		for(int i = 0; i < SYSTEM_SCHEDULER->thread_count; i++){
//			Console::puts("\nThread ");
//			Console::puti((newNode->thread)->ThreadId());
//			Console::puts(" is in the thread list\n");
//			newNode = newNode->next;
//		}
//		Console::puts("\nafter thread 1 block\n");
//		for(;;);
//	}




//	Console::puts("\nthread 2 is yielding CPU\n");

		SYSTEM_SCHEDULER->yield();
//	}else{
//	Console::puts("\nthread 2 was yield back\n");
//	for(;;);
//	Console::puts("trying to read from port");
	int i;
	unsigned short tmpw;
	for(i = 0; i < 256; i++){
		tmpw = Machine::inportw(0x1F0);
		_buf[i*2]   = (unsigned char) tmpw;
		_buf[i*2+1] = (unsigned char) (tmpw >> 8);
	}

}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
//  SimpleDisk::read(_block_no, _buf);

	issue_operation(WRITE, _block_no);
	block(Thread::CurrentThread());
	SYSTEM_SCHEDULER->yield();

//	Console::puts("\ntrying to write to disk\n");
//	for(;;);
	int i;
	unsigned short tmpw;
	for(i = 0; i < 256; i++){
		tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
		Machine::outportw(0x1F0,tmpw);	
//		tmpw = Machine::inportw(0x1F0);
//		_buf[i*2] = (unsigned char) tmpw;
//		_buf[i*2+1] = (unsigned char) (tmpw >> 8);
	}
//	Console::puts("\nwe completed write also\n");
//	for(;;);
}
