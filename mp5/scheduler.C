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

Scheduler::Scheduler() {
//  assert(false);
	thread_list_head->thread = NULL;
	thread_list_head->next = NULL;

	thread_count = 0;

	last_thread_node->thread = NULL;
	last_thread_node->next = NULL;
//	current_running_thread = Thread::CurrentThread();
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  //assert(false);
	//grab the next thread
	//current_running_thread = Thread::CurrentThread();

	if(Machine::interrupts_enabled()) {
//		Console::puts("\ninterrupts enabled\n");
		Machine::disable_interrupts();
//		Console::puts("interrupts disabled\n");
//		for(;;);
	}
	//Console::puts("\nHere is the start of yield\n ");
	node* coming_thread_node = (*thread_list_head).next;
//	thread_count++;
	(*thread_list_head).next = (*coming_thread_node).next;
	(*coming_thread_node).next = NULL;
	thread_count--;

	//give up CPU
	Thread* temp_thread = (*coming_thread_node).thread;
//	Console::puts("\nHere is the start of dispatch\n ");


	Thread::dispatch_to(temp_thread);	


	return;
}

void Scheduler::resume(Thread * _thread) {
//  assert(false);
	if(!Machine::interrupts_enabled()) {
//		Console::puts("\ninterrupts disabled\n");
		Machine::enable_interrupts();  
//		Console::puts("interrupts enabled");
		
	}
//	node* newNode;
//	newNode->thread = _thread;
//	newNode->next   = NULL;

	add(_thread);
//	Console::puts("\nCurrent Running thread ID: ");
//	Console::puti(_thread->ThreadId());
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
