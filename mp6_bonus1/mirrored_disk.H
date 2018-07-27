/*
File		: mirrored_disk.H

Author		: Liuyi Jin

Date		: July, 23th, 2017
Discription	: derived from simple_disk.H/C file

*/

#ifndef _MIRRORED_DISK_H_
#define _MIRRORED_DISK_H_

#include "simple_disk.H"
#include "thread.H"


struct mirrored_node{Thread* thread; mirrored_node* next;};
class MirroredDisk : public SimpleDisk{

private:
	mirrored_node* mirrored_thread_list_head;
	mirrored_node* mirrored_last_thread_node;

public:
	 MirroredDisk(DISK_ID _disk_id, unsigned int _size);
	unsigned long  mirrored_thread_count;

	virtual	void issue_operation(DISK_OPERATION _op, unsigned long _block_no);

	virtual void read(unsigned long _block_no, unsigned char* _buf);

	virtual void write(unsigned long _block_no, unsigned char* _buf);

	virtual void block(Thread* thread);

	virtual void unblock(Thread* thread);

	virtual	unsigned long mirrored_block_count();

	virtual	void increment_thread_count();

	virtual	void decrement_thread_count();

	virtual mirrored_node* front();

	virtual	void set_front(mirrored_node* first_node);

	virtual	mirrored_node* end();

	virtual	void set_end(mirrored_node* last_node);
};

#endif
