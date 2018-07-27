/* 
    File: file_system.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 10/04/05

    Description: Simple File System.
    

*/

#ifndef _FILE_SYSTEM_H_                   // include file only once
#define _FILE_SYSTEM_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "file.H"
#include "simple_disk.H"
#include "lock.H"
/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARD DECLARATIONS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* F i l e S y s t e m  */
/*--------------------------------------------------------------------------*/

struct file_node{unsigned int file_id; File* file; file_node* next;};

class FileSystem {

friend class File; /* -- not sure if we need this; feel free to delete */

private:
     /* -- DEFINE YOUR FILE SYSTEM DATA STRUCTURES HERE. */
     static unsigned int size;
//	static	char* block_map;	
	static bool disk_count;
	static file_node * file_list_head;
	static file_node * file_list_tail;
	static unsigned int file_count;
	static 	unsigned int free_block;
	//responsible for recording the ids and the start block, file size
//	unsigned int* file_array;	
	static Lock* file_system_lock;
public:
//	static unsigned int free_block;
	SimpleDisk * disk;
//	bool disk_count;
//	file_node * file_list_head;
//	file_node * file_list_tail;
//	int file_count;
	void	register_file(File* file);
	void 	deregister_file(unsigned int file_id);
//	unsigned int free_block;

    FileSystem();
    /* Just initializes local data structures. Does not connect to disk yet. */
    
    bool Mount(SimpleDisk * _disk);
    /* Associates this file system with a disk. Limit to at most one file system per disk.
     Returns true if operation successful (i.e. there is indeed a file system on the disk.) */
    
    static bool Format(SimpleDisk * _disk, unsigned int _size);
    /* Wipes any file system from the disk and installs an empty file system of given size. */
    
    File * LookupFile(unsigned int _file_id);
    /* Find file with given id in file system. If found, return the initialized
     file object. Otherwise, return null. */
    
    bool CreateFile(unsigned int _file_id);
    /* Create file with given id in the file system. If file exists already,
     abort and return false. Otherwise, return true. */
    
    bool DeleteFile(unsigned int _file_id);
    /* Delete file with given id in the file system; free any disk block occupied by the file. */
   
};
#endif
