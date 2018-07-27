/* 
    File: kernel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2017/05/02


    This file has the main entry point to the operating system.

    MAIN FILE FOR MACHINE PROBLEM "KERNEL-LEVEL DEVICE MANAGEMENT"

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- COMMENT/UNCOMMENT THE FOLLOWING LINE TO EXCLUDE/INCLUDE SCHEDULER CODE */

#define _USES_SCHEDULER_
/* This macro is defined when we want to force the code below to use 
   a scheduler.
   Otherwise, no scheduler is used, and the threads pass control to each 
   other in a co-routine fashion.
*/

#define MB * (0x1 << 20)
#define KB * (0x1 << 10)

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "machine.H"         /* LOW-LEVEL STUFF   */
#include "console.H"
#include "gdt.H"
#include "idt.H"             /* EXCEPTION MGMT.   */
#include "irq.H"
#include "exceptions.H"     
#include "interrupts.H"

#include "simple_timer.H"    /* TIMER MANAGEMENT  */

#include "frame_pool.H"      /* MEMORY MANAGEMENT */
#include "mem_pool.H"

#include "thread.H"         /* THREAD MANAGEMENT */

#ifdef _USES_SCHEDULER_
#include "scheduler.H"      /* WE WILL NEED A SCHEDULER WITH BlockingDisk */
#endif

#include "simple_disk.H"    /* DISK DEVICE */
#include "blocking_disk.H"  /* YOU MAY NEED TO INCLUDE blocking_disk.H*/
#include "mirrored_disk.H"

/*--------------------------------------------------------------------------*/
/* MEMORY MANAGEMENT */
/*--------------------------------------------------------------------------*/

/* -- A POOL OF FRAMES FOR THE SYSTEM TO USE */
FramePool * SYSTEM_FRAME_POOL;

/* -- A POOL OF CONTIGUOUS MEMORY FOR THE SYSTEM TO USE */
MemPool * MEMORY_POOL;

typedef unsigned int size_t;

//replace the operator "new"
void * operator new (size_t size) {
    unsigned long a = MEMORY_POOL->allocate((unsigned long)size);
    return (void *)a;
}

//replace the operator "new[]"
void * operator new[] (size_t size) {
    unsigned long a = MEMORY_POOL->allocate((unsigned long)size);
    return (void *)a;
}

//replace the operator "delete"
void operator delete (void * p) {
    MEMORY_POOL->release((unsigned long)p);
}

//replace the operator "delete[]"
void operator delete[] (void * p) {
    MEMORY_POOL->release((unsigned long)p);
}

/*--------------------------------------------------------------------------*/
/* SCHEDULER */
/*--------------------------------------------------------------------------*/

#ifdef _USES_SCHEDULER_

/* -- A POINTER TO THE SYSTEM SCHEDULER */
Scheduler * SYSTEM_SCHEDULER;

#endif

/*--------------------------------------------------------------------------*/
/* DISK */
/*--------------------------------------------------------------------------*/

/* -- A POINTER TO THE SYSTEM DISK */
SimpleDisk * SYSTEM_DISK;

/*---------------bonus 1--------------------*/
SimpleDisk * SUB_DISK;

#define SYSTEM_DISK_SIZE (10 MB)
#define SUB_DISK_SIZE (10 MB)
/*--------------------------------------------------------------------------*/
/* JUST AN AUXILIARY FUNCTION */
/*--------------------------------------------------------------------------*/

void pass_on_CPU(Thread * _to_thread) {

#ifndef _USES_SCHEDULER_

        /* We don't use a scheduler. Explicitely pass control to the next
           thread in a co-routine fashion. */
	Thread::dispatch_to(_to_thread); 

#else

        /* We use a scheduler. Instead of dispatching to the next thread,
           we pre-empt the current thread by putting it onto the ready
           queue and yielding the CPU. */
//	Console::puts("\nhere is before thread 1 resume\n");
//	Console::putui((SYSTEM_SCHEDULER->m_disk)->mirrored_thread_count);
//	for(;;);
        SYSTEM_SCHEDULER->resume(Thread::CurrentThread()); 
        SYSTEM_SCHEDULER->yield();
#endif
}

/*--------------------------------------------------------------------------*/
/* A FEW THREADS (pointer to TCB's and thread functions) */
/*--------------------------------------------------------------------------*/

Thread * thread1;
Thread * thread2;
Thread * thread3;
Thread * thread4;

void fun1() {
    Console::puts("THREAD: "); Console::puti(Thread::CurrentThread()->ThreadId()); Console::puts("\n");

    Console::puts("FUN 1 INVOKED!\n");

    for(int j = 0;; j++) {
//	for(int j = 0; j < 10; j++) {

       Console::puts("FUN 1 IN ITERATION["); Console::puti(j); Console::puts("]\n");

       for (int i = 0; i < 10; i++) {
	  Console::puts("FUN 1: TICK ["); Console::puti(i); Console::puts("]\n");
       }
       pass_on_CPU(thread2);
    }
}



void fun2() {
    Console::puts("THREAD: "); Console::puti(Thread::CurrentThread()->ThreadId()); Console::puts("\n");

    Console::puts("FUN 2 INVOKED!\n");
//	for(;;);
    unsigned char buf[512];
    int  read_block  = 1;
    int  write_block = 0;

    for(int j = 0;; j++) {

//    for(int j = 0; j < 10; j++) {

       Console::puts("FUN 2 IN ITERATION["); Console::puti(j); Console::puts("]\n");

       /* -- Read */
       Console::puts("Reading a block from disk...\n");
//      	if(j == 0) {
//		Console::puts("\nThis is the 1st for loop\n");
//		for(;;);
//	}
//	 SYSTEM_DISK->read(read_block, buf);
//	SUB_DISK->write(read_block, buf);
	SUB_DISK->read(read_block, buf);
//	if(j == 0) {
//		Console::puts("\nThis is the 2nd for loop\n");
//		for(;;);
//	}

       /* -- Display */
       int i;
       for (i = 0; i < 512; i++) {
	  Console::putui(buf[i]);
	  Console::puts("\n");
       }
//	for(;;);
       Console::puts("\nWriting a block to disk...\n");
//	for(;;);
//	SYSTEM_DISK->write(write_block, buf); 
//	SUB_DISK->read(write_block, buf);
	SUB_DISK->write(write_block,buf);	
       /* -- Move to next block */
       write_block = read_block;
       read_block  = (read_block + 1) % 10;

//	if(j == 20) {
//		Console::puts("\nThis is 2nd the for loop\n");
//		for(;;);
//	}

//	Console::puts("\nThis in fun1 normal pass by thread 1\n");
//	for(;;);
       /* -- Give up the CPU */
       pass_on_CPU(thread3);
    }
}

void fun3() {
    Console::puts("THREAD: "); Console::puti(Thread::CurrentThread()->ThreadId()); Console::puts("\n");

    Console::puts("FUN 3 INVOKED!\n");

     for(int j = 0;; j++) {

//	for(int j = 0; j < 10; j++) {

       Console::puts("FUN 3 IN BURST["); Console::puti(j); Console::puts("]\n");

       for (int i = 0; i < 10; i++) {
	  Console::puts("FUN 3: TICK ["); Console::puti(i); Console::puts("]\n");
       }
    
       pass_on_CPU(thread4);
    }
}

void fun4() {
    Console::puts("THREAD: "); Console::puti(Thread::CurrentThread()->ThreadId()); Console::puts("\n");

    for(int j = 0;; j++) {

//	for(int j = 0; j < 10; j++) {
       Console::puts("FUN 4 IN BURST["); Console::puti(j); Console::puts("]\n");

       for (int i = 0; i < 10; i++) {
	  Console::puts("FUN 4: TICK ["); Console::puti(i); Console::puts("]\n");
       }

//	if(j == 2){
//		Console::puts("\nHere is where we panic\n");
//		Console::puts("current thread is ");
//		Console::puti(Thread::CurrentThread()->ThreadId());
//		Console::puts("\n");
//		node* newNode = (*(SYSTEM_SCHEDULER->thread_list_head)).next;
//		for(int i = 0; i < SYSTEM_SCHEDULER->thread_count; i++){
//			Console::puts("\nThread ");
//			Console::puti((newNode->thread)->ThreadId());
//			Console::puts( "is in the thread list\n");
//			newNode = newNode->next;
//		}
//		for(;;);
//	}

       pass_on_CPU(thread1);
    }
}

/*--------------------------------------------------------------------------*/
/* MAIN ENTRY INTO THE OS */
/*--------------------------------------------------------------------------*/

int main() {

    GDT::init();
    Console::init();
    IDT::init();
    ExceptionHandler::init_dispatcher();
    IRQ::init();
    InterruptHandler::init_dispatcher();

    /* -- EXAMPLE OF AN EXCEPTION HANDLER -- */

    class DBZ_Handler : public ExceptionHandler {
      public:
      virtual void handle_exception(REGS * _regs) {
        Console::puts("DIVISION BY ZERO!\n");
        for(;;);
      }
    } dbz_handler;

    ExceptionHandler::register_handler(0, &dbz_handler);

    /* -- INITIALIZE MEMORY -- */
    /*    NOTE: We don't have paging enabled in this MP. */
    /*    NOTE2: This is not an exercise in memory management. The implementation
                of the memory management is accordingly *very* primitive! */

    /* ---- Initialize a frame pool; details are in its implementation */
    FramePool system_frame_pool;
    SYSTEM_FRAME_POOL = &system_frame_pool;
   
    /* ---- Create a memory pool of 256 frames. */
    MemPool memory_pool(SYSTEM_FRAME_POOL, 256);
    MEMORY_POOL = &memory_pool;

    /* -- MEMORY ALLOCATOR SET UP. WE CAN NOW USE NEW/DELETE! -- */

    /* -- INITIALIZE THE TIMER (we use a very simple timer).-- */

    /* Question: Why do we want a timer? We have it to make sure that 
                 we enable interrupts correctly. If we forget to do it,
                 the timer "dies". */

    SimpleTimer timer(100); /* timer ticks every 10ms. */
    InterruptHandler::register_handler(0, &timer);
    /* The Timer is implemented as an interrupt handler. */

#ifdef _USES_SCHEDULER_

    /* -- SCHEDULER -- IF YOU HAVE ONE -- */
  
    SYSTEM_SCHEDULER = new Scheduler();

#endif

/*	Console::putui((SYSTEM_SCHEDULER->m_disk[0])->mirrored_thread_count);
	Console::puti((SYSTEM_SCHEDULER->disk[0])->block_thread_count);
	for(;;);
*/
    /* -- DISK DEVICE -- */

//    SYSTEM_DISK = new SimpleDisk(MASTER, SYSTEM_DISK_SIZE);
  	 
    SYSTEM_DISK = new BlockingDisk(MASTER, SYSTEM_DISK_SIZE);

	SUB_DISK = new MirroredDisk(SLAVE, SUB_DISK_SIZE);
//	Console::puts("\nhere is before thread 1 resume\n");
//	Console::putui((SYSTEM_SCHEDULER->m_disk[0])->mirrored_thread_count);
//	Console::puti((SYSTEM_SCHEDULER->disk[0])->block_thread_count);
//	for(;;);

    /* NOTE: The timer chip starts periodically firing as 
             soon as we enable interrupts.
             It is important to install a timer handler, as we 
             would get a lot of uncaptured interrupts otherwise. */  

    /* -- ENABLE INTERRUPTS -- */
//	Console::puts("\nhere is before thread 1 resume\n");
//	Console::putui((SYSTEM_SCHEDULER->m_disk)->mirrored_thread_count);
//	for(;;);

     Machine::enable_interrupts();

    /* -- MOST OF WHAT WE NEED IS SETUP. THE KERNEL CAN START. */

    Console::puts("Hello World!\n");

    /* -- LET'S CREATE SOME THREADS... */

    Console::puts("CREATING THREAD 1...\n");
    char * stack1 = new char[1024];
    thread1 = new Thread(fun1, stack1, 1024);
    Console::puts("DONE\n");

    Console::puts("CREATING THREAD 2...");
    char * stack2 = new char[1024];
    thread2 = new Thread(fun2, stack2, 1024);
    Console::puts("DONE\n");

    Console::puts("CREATING THREAD 3...");
    char * stack3 = new char[1024];
    thread3 = new Thread(fun3, stack3, 1024);
    Console::puts("DONE\n");

    Console::puts("CREATING THREAD 4...");
    char * stack4 = new char[1024];
    thread4 = new Thread(fun4, stack4, 1024);
    Console::puts("DONE\n");

#ifdef _USES_SCHEDULER_

    /* WE ADD thread2 - thread4 TO THE READY QUEUE OF THE SCHEDULER. */

    SYSTEM_SCHEDULER->add(thread2);
    SYSTEM_SCHEDULER->add(thread3);
    SYSTEM_SCHEDULER->add(thread4);

	



#endif

    /* -- KICK-OFF THREAD1 ... */

    Console::puts("STARTING THREAD 1 ...\n");
    Thread::dispatch_to(thread1);

    /* -- AND ALL THE REST SHOULD FOLLOW ... */
 
    assert(false); /* WE SHOULD NEVER REACH THIS POINT. */

    /* -- WE DO THE FOLLOWING TO KEEP THE COMPILER HAPPY. */
    return 1;
}
