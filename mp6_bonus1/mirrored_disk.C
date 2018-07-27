/*
File		: mirrored_disk.C

Author		: Liuyi Jin

Date		: July, 23th, 2017
Discription	: implement the mirrored_disk.H
*/


#include "assert.H"
#include "utils.H"
#include "console.H"
#include "mirrored_disk.H"
//#include "machine.H"
#include "scheduler.H"
//#include "simple_disk.H"

extern Scheduler* SYSTEM_SCHEDULER;
//extern SimpleDisk* SYSTEM_DISK

/*------------------------CONSTRUCTOR----------------------------*/

MirroredDisk::MirroredDisk (DISK_ID _disk_id, unsigned int _size)
	:SimpleDisk(_disk_id, _size){
	
	mirrored_thread_list_head = new mirrored_node();	
	mirrored_last_thread_node = new mirrored_node();
	mirrored_thread_count = 0;

	SYSTEM_SCHEDULER->register_mirrored_disk(this);
//	Console::putui(SYSTEM_SCHEDULER->m_disk_counter);
//	Console::puts("\nwe have been to the constructor\n");
//	for(;;);
	return;
}


/*----------------------Mirrored Disk Functions-----------------*/

void MirroredDisk::issue_operation(DISK_OPERATION _op, unsigned long _block_no){

//	SimpleDisk::issue_operation(_op, _block_no);
	Machine::outportb(0x171, 0x00);
	Machine::outportb(0x172, 0x01);
	Machine::outportb(0x173, (unsigned char)_block_no);
	Machine::outportb(0x174, (unsigned char)(_block_no >> 8));
	Machine::outportb(0x175, (unsigned char)(_block_no >> 16));
	Machine::outportb(0x176, ((unsigned char)(_block_no >> 24) & 0x0F) | 0xE0 | (get_disk_id() << 4));

//	Console::puts("\ndisk id is\n");
//	Console::putui(get_disk_id());
//	for(;;);

	Machine::outportb(0x177, (_op == READ) ? 0x20 : 0x30);
	
//	for(int j = 0; j < 100; j++);
//	Console::puts("\nsystem ready register\n");
//	Console::putui(Machine::inportb(0x1F7));
//	Console::puts("\nMirrored ready register\n");	
//	Console::putui(Machine::inportb(0x177));
//	for(;;);

}

void MirroredDisk::block(Thread* thread){

	mirrored_node* newNode 	= new mirrored_node();
	newNode->thread 	= thread;
	newNode->next 		= NULL;

	if(mirrored_thread_count == 0){
		set_front(newNode);
		set_end(newNode);
		increment_thread_count();
	}else{
		(end())->next = newNode;
		set_end(newNode);
		increment_thread_count();
	}

}

void MirroredDisk::unblock(Thread* _thread){

	if(mirrored_thread_count == 0){
		Console::puts("There is no thread in mirrored block queue");
		abort();
	}

	mirrored_node* curNode = front();
	mirrored_node* preNode = mirrored_thread_list_head;
	for(int i = 0; i < mirrored_thread_count; i++){
		if(curNode->thread = _thread){
			break;
		}
		preNode = preNode->next;
		curNode = curNode->next;
	}	

	if(curNode->next = NULL){
		preNode->next = NULL;
		mirrored_last_thread_node = preNode;
	}else{
		preNode->next = curNode->next;
	}
	
	decrement_thread_count();

}

void MirroredDisk::increment_thread_count(){
	mirrored_thread_count += 1;
}

void MirroredDisk::decrement_thread_count(){
	mirrored_thread_count -= 1;
}

unsigned long MirroredDisk::mirrored_block_count(){
	return mirrored_thread_count;
}

mirrored_node* MirroredDisk::front(){
	return mirrored_thread_list_head->next;
}

void MirroredDisk::set_front(mirrored_node* first_node){
	mirrored_thread_list_head->next = first_node;
}

mirrored_node* MirroredDisk::end(){
	return mirrored_last_thread_node;
}

void MirroredDisk::set_end(mirrored_node* last_node){
	mirrored_last_thread_node = last_node;
}

void MirroredDisk::read(unsigned long _block_no, unsigned char* _buf){
	
	issue_operation(READ, _block_no);


	Console::puts("\nis ready\n");
//	for(int j = 0; j < 100; j++);
	Console::putui(Machine::inportb(0x177));
//	for(;;);

//	Console::puts("Here we are in the read operation");
//	for(;;);
	block(Thread::CurrentThread());	
//	Console::puts("\nHere we are in the read operation\n");
//	for(;;);
	SYSTEM_SCHEDULER->yield();


	int i;
	unsigned short tmpw;
	for(i = 0; i < 256; i++){
		tmpw = Machine::inportw(0x170);
		_buf[i*2]   = (unsigned char)(tmpw);
		_buf[i*2+1] = (unsigned char)(tmpw >> 8);
	}

}

void MirroredDisk::write(unsigned long _block_no, unsigned char* _buf){


//	Console::puts("\nThis is in the mirrored disk write1\n");
	
	issue_operation(WRITE, _block_no);

//	Console::puts("\nThis is in the mirrored disk write2\n");
	block(Thread::CurrentThread());
	SYSTEM_SCHEDULER->yield();

//	Console::puts("\nThis is in the mirrored disk write2\n");
//	for(;;);

	int i;
	unsigned short tmpw;
	for(i = 0; i < 256; i++){
		tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
	Console::puts("\nThis is before the outportw\n");
//	Console::putui(get_disk_id());
//	for(;;);
		Machine::outportw(0x170, tmpw);
	Console::puts("\nThis is after the outportw\n");
	for(;;);
	}

	Console::puts("\nfirst write completion\n");
	for(;;);

}
