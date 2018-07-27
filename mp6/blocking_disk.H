/*
     File        : blocking_disk.H

     Author      : 

     Date        : 
     Description : 

*/

#ifndef _BLOCKING_DISK_H_
#define _BLOCKING_DISK_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "simple_disk.H"


#include "thread.H"
//#include "scheduler.H"
/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* B l o c k i n g D i s k  */
/*--------------------------------------------------------------------------*/

struct block_node{Thread* thread; block_node* next;};
//class Scheduler;
class BlockingDisk : public SimpleDisk {
private:
	block_node* block_thread_list_head;
	block_node* block_last_thread_node;
	int block_thread_count;

public:

//	friend class Scheduler;
//	void register_disk(unsigned int index, Scheduler* scheduler);

	virtual void block(Thread* _thread);
	virtual void unblock(Thread* _thread);

	virtual int block_count();


	virtual void increment_thread_count();
	virtual void decrement_thread_count();
	
	virtual block_node* front();//{
//		return block_thread_list_head->next;
//	}

	virtual void set_front(block_node* first_node);//{
//		block_thread_list_head->next = first_node;
//	}

	virtual block_node* end();//{
//		return block_last_thread_node;
//	}

	virtual void set_end(block_node* last_node);//{
//		block_last_thread_node = last_node;
//	}


   BlockingDisk(DISK_ID _disk_id, unsigned int _size); 
   /* Creates a BlockingDisk device with the given size connected to the 
      MASTER or SLAVE slot of the primary ATA controller.
      NOTE: We are passing the _size argument out of laziness. 
      In a real system, we would infer this information from the 
      disk controller. */

   /* DISK OPERATIONS */

   virtual void read(unsigned long _block_no, unsigned char * _buf);
   /* Reads 512 Bytes from the given block of the disk and copies them 
      to the given buffer. No error check! */

   virtual void write(unsigned long _block_no, unsigned char * _buf);
   /* Writes 512 Bytes from the buffer to tihe given block on the disk. */

};

#endif
