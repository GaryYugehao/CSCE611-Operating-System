/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "blocking_disk.H"
#include "machine.H"


extern Scheduler* SYSTEM_SCHEDULER;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

//Thread* Scheduler::current_running_thread = NULL;
node* Scheduler::last_thread_node = 0;
node* Scheduler::thread_list_head = 0;
unsigned long Scheduler::thread_count;

//this is for mp6
//BlockingDisk* Scheduler::disk;
//unsigned long Scheduler::disk_counter = 0;

Scheduler::Scheduler() {
//  assert(false);
	thread_list_head->thread = NULL;
	thread_list_head->next = NULL;

	thread_count = 0;

	last_thread_node->thread = NULL;
	last_thread_node->next = NULL;
//	current_running_thread = Thread::CurrentThread();
	//this is for mp6
//	disk = new BlockingDisk()[2];
//	disk[1] = new Blockingdisk();
	disk_counter = 0;

  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  //assert(false);
	//grab the next thread
	//current_running_thread = Thread::CurrentThread();

//	if(Machine::interrupts_enabled()) {
//		Console::puts("\ninterrupts enabled\n");
//		Machine::disable_interrupts();
//		Console::puts("interrupts disabled\n");
//		for(;;);
//	}
//	if(Thread::CurrentThread()->ThreadId() == 0){
//		node* newNode = (*thread_list_head).next;	
//		for(int i = 0; i < thread_count; i++){
//			Console::puts("\nThread ");
//			Console::puti((newNode->thread)->ThreadId());
//			Console::puts(" is in the thread list\n");
//			newNode = newNode->next;
//		}
//		Console::puts("\nbefore thread 0 yield\n");
//		for(;;);
//	}

	//Console::puts("\nHere is the start of yield\n ");
	node* coming_thread_node = (*thread_list_head).next;
//	thread_count++;
	(*thread_list_head).next = (*coming_thread_node).next;
	(*coming_thread_node).next = NULL;
	thread_count--;

	


	//give up CPU
	Thread* temp_thread = (*coming_thread_node).thread;
//	Console::puts("\nHere is the start of dispatch\n ");

//	if(Thread::CurrentThread()->ThreadId() == 1){
//		Console::puts("\nThread <2> is yielding CPU to thread ");
//		Console::puti(temp_thread->ThreadId());
//		Console::puts(" \n");
//		for(;;);
//	}

//	if(Thread::CurrentThread()->ThreadId() == 1){
//		node* newNode = (*thread_list_head).next;	
//		for(int i = 0; i < thread_count; i++){
//			Console::puts("\nThread ");
//			Console::puti((newNode->thread)->ThreadId());
//			Console::puts(" is in the thread list\n");
//			newNode = newNode->next;
//		}
//		Console::puts("\nthread <1> is yielding cpu to thread");
//		Console::puti(temp_thread->ThreadId());
//		Console::puts(" \n");
//		for(;;);
//	}


	Thread::dispatch_to(temp_thread);	


	return;
}

void Scheduler::resume(Thread * _thread) {
//  assert(false);
//	if(!Machine::interrupts_enabled()) {
//		Console::puts("\ninterrupts disabled\n");
//		Machine::enable_interrupts();  
//		Console::puts("interrupts enabled");
//	}

	//check if any thread is ready to be unblocked
//	if(Thread::CurrentThread()->ThreadId() == 0){
//		node* newNode = (*thread_list_head).next;	
//		for(int i = 0; i < thread_count; i++){
//			Console::puts("\nThread ");
//			Console::puti((newNode->thread)->ThreadId());
//			Console::puts(" is in the thread list\n");
//			newNode = newNode->next;
//		}
//	}
//	for(;;);

	add(_thread);

//-------------------------------this is the begin og blocking disk------------------------
/*
	if(disk_counter != 0){


		BlockingDisk* blocking_disk = disk[0];
		if(blocking_disk->block_count() > 0){

			Console::puts("\nThis should not be seen\n");

			block_node* temp_node = blocking_disk->front();
		//if there is no blocked thread at all
			for(int i  = 0; i < blocking_disk->block_count(); i++){
			
				if(temp_node == NULL){
					return;
				}
		
				if(Machine::inportb(0x1F7) & 0x08){
					blocking_disk->unblock(temp_node->thread);
					add(temp_node->thread);	
//					if(temp_node = NULL)	return;

//				node* newNode = (*thread_list_head).next;	

//		for(int i = 0; i < thread_count; i++){
//			Console::puts("\nThread ");
//			Console::puti((newNode->thread)->ThreadId());
//			Console::puts(" is in the thread list\n");
//			newNode = newNode->next;
//		}
//		Console::puts("\nthread ");
//		Console::puti((temp_node->thread)->ThreadId());
//		Console::puts(" was added\n");
//		for(;;);
//		Console::puts("\n there are ");
//		Console::puti(blocking_disk->block_count());
//		Console::puts("threads left in the block list");

//					for(;;);
					delete temp_node;
					return;
				}			
				temp_node = temp_node->next;
			}
		}
	}
*/
//---------------------------------this is the end of blocking disk---------------------------

	return;
}

void Scheduler::add(Thread * _thread) {
//	Console::puts("\nThread ");
//	Console::putui((_thread)->ThreadId());
//	Console::puts(" have been added\n");

	
	node* newNode = new node();
	newNode->thread = _thread;
	newNode->next = NULL;
	if(thread_count == 0){

		(*thread_list_head).next = newNode;
		
		last_thread_node = newNode;			

	}else{
		(*last_thread_node).next = newNode;
		last_thread_node = newNode;
	}
	thread_count++;
//	Console::puts("\nhead next node thread id = ");
//	Console::putui(((*(thread_list_head->next)).thread)->ThreadId());
//	Console::puts(" have been added once\n");

  //assert(false);
	//for(;;);
	return;
}

void Scheduler::terminate(Thread * _thread) {
 // assert(false);

//	if(Machine::interrupts_enabled()) Machine::disable_interrupts();  
	
	if(thread_count == 0) return;
//	Console::puts("\nHere is the begin of terminate\n");
//	for(;;);
//	Thread* target_thread= ;
	node* newNode = thread_list_head->next;
	node* preNode = thread_list_head;
	int i = 0;
	for(i = 0; i < thread_count; i++){
//		Console::puts("\ni = ");
//		Console::puti(i);
//		for(;;);
		if(newNode->thread == _thread){
		//	for(;;);
			 break; 
		}
		preNode = preNode->next;	
		newNode = newNode->next;
	}
	

	if(newNode->next == NULL){	//we reach the end of the queue
		preNode->next = NULL;
		last_thread_node = preNode;
	}else{
		preNode->next = (newNode->next)->next;
	}
	thread_count--;

	if(newNode == NULL) return;
	else delete newNode;

//	Machine::enable_interrupts();
	return;
}


//This is for mp6
void Scheduler::register_disk(BlockingDisk* _disk){

	if(disk_counter == 0){
		disk[0] = _disk;
		disk_counter += 1;
	}else{
		disk[1] = _disk;
		disk_counter += 1;
	}	

}

