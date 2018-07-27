#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"
//#include "cont_frame_pool.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;
//unsigned long count = 0;


void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
   //assert(false);

	kernel_mem_pool = _kernel_mem_pool;
	process_mem_pool = _process_mem_pool;
	shared_size = _shared_size;
	Console::puts("Initialized Paging System\n");

}

PageTable::PageTable()
{
  // assert(false);
	//assign 1 frame in kernel to the page directory page

//	current_page_table = this;

	unsigned long page_dir_frame = (*kernel_mem_pool).get_frames(1);
	(*this).page_directory = (unsigned long*)(page_dir_frame * PAGE_SIZE);

	((*this).page_directory)[0] = (unsigned long)(( kernel_mem_pool->get_frames(1) *PAGE_SIZE) | 3);
	//unsigned long page_table_frame = (*kernel_mem_pool).get_frames(1);
	unsigned long *page_table_page = (unsigned long*) (page_directory[0] & 0xFFFFF000);

	unsigned long address = 0;
	unsigned int i;

	for(i = 0; i < 1024; i++){
		page_table_page[i] = address | 3;
		address = address + 4096;
	}
	
	//fill the first entry of the page dir
//	((*this).page_directory)[0] = (unsigned long)page_table_page;	
//	((*this).page_directory)[0] |= 3;
	
	//Console::puts("1st page directory address: ");
	//Console::putui(((*this).page_directory)[0]);	
	//Console::puts("\n");

	for(i = 1; i < 1024; i++){
		((*this).page_directory)[i] = 0 | 2;
	}

	Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
//   assert(false);
 	//put the page directory address into cr	
	current_page_table = this;
	write_cr3((unsigned long)page_directory);
	
	//write_cr3((unsigned long)(page_directory));
	Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   //assert(false);
	paging_enabled = 1;
	//set the paging bit in cr0 to 1
	write_cr0(read_cr0() | 0x80000000);

	Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
	unsigned int exc_no = _r->int_no;
	if(exc_no == 0){
		Console::puts("Division by zero\n");
	}else if (exc_no == 14){
		unsigned long *cur_page_directory = (unsigned long*) read_cr3();
		unsigned long *page_table;
		unsigned long addr = read_cr2();
		unsigned long dir_offset = addr >> 22;
		if((cur_page_directory[dir_offset] & 1) != 1){	
		cur_page_directory[dir_offset] = (unsigned long)((kernel_mem_pool->get_frames(1)*PAGE_SIZE) | 3);
			page_table = (unsigned long*) (cur_page_directory[dir_offset] & 0xFFFFF000);
			for(int i = 0; i < 1024; i++){
				page_table[i] = 0 | 2;
			}
		}
		unsigned long table_offset = ((addr>>12) & 0x3FF);
		page_table = (unsigned long*)(cur_page_directory[dir_offset] & 0xFFFFF000);
		page_table[table_offset] = (unsigned long)(process_mem_pool->get_frames(1)*PAGE_SIZE) | 3;
	} 
	Console::puts("handled page fault\n");
}

