/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

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
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

//struct node{ContFramePool current_pool(unsigned long base, unsigned long n,
//				 unsigned long info_no, unsigned long n_info);
//				 node *next;};
node *pool_list_head;

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    //assert(false);
	base_frame_no 	= _base_frame_no;
	nframes 	= _n_frames;
	nfreeframes 	= _n_frames;
	info_frame_no   = _info_frame_no;
	ninfoframes	= _n_info_frames;

	assert(nframes <= FRAME_SIZE * ninfoframes * 4);

	//directly give address to the 1st position of bitmap
	if(info_frame_no == 0) {
		bitmap = (unsigned char*) (base_frame_no * FRAME_SIZE);	
//		isHead = (unsigned char*) (base_frame_no * FRAME_SIZE);
	}else{
		bitmap = (unsigned char*) (info_frame_no * FRAME_SIZE);
	}

	//number of frames must fill with bitmap
	assert((nframes % 4) == 0);
	//nbitmap = (unsigned int) nframes/4;	

	//initially, all frames are unallocated, not the head of sequence
	for(int i = 0; i * 4 < nframes; i++){
		bitmap[i] = 0xFF;
	}
	
	//When the info frame number is the base frame, then we mark
	//that frame as used
	if(info_frame_no == 0){
		bitmap[0] = 0x7F;
		nfreeframes--;
	}

	node* newNode;

	(*newNode).currentPool = this;	
	node* headNext = (*pool_list_head).next;
	if(headNext == NULL ){
		(*pool_list_head).next =  newNode;
	}else{
		(*newNode).next = (*pool_list_head).next;
		(*pool_list_head).next = newNode;
	}		
	
	Console::puts("Frame pool is initialized");
}


//bool ContFramePool::areFramesAvailable(unsigned long beginByte, int beginBit,
//			unsigned char beginMask, unsigned int target){
//
//	unsigned long count = 0;
//	bool flag = true;
//
//	unsigned char tempMask = beginMask;
//	for(unsigned int bit = beginBit; bit < 4; bit++){
//		if((bitmap[beginByte] & tempMask) == 0){
//			return false;
//		}else{
//			count++;
//			if(count == target) return true;
//			tempMask >> 1;
//		}
//		//if(flag == false) return false;
//	}
//
//	for(unsigned long i = beginByte+1; i < FRAME_SIZE; i++){
//		unsigned char mask = 0x80;
//		for(unsigned int bit = 0; bit < 4; bit++){
//			if((bitmap[i] & mask) == 0){
//				return false;
//			}else{
//				count++;
//				if(count == target) return true;
//				mask >> 1;
//			}
//		}
//	}
//	
//	return flag;
//}

unsigned long ContFramePool::get_frames(unsigned int _n_frames){
	assert(nfreeframes >= _n_frames)

	int begin_frame;
	int temp_frame;
	bool exist = false;
	int i;
	for(i = base_frame_no; i < base_frame_no + nframes; i++){
		
		int frame_diff = i - base_frame_no;
		unsigned int bitmap_index = frame_diff / 4;	
		unsigned char mask1 = 0x80 >> (frame_diff % 4);
		if((bitmap[bitmap_index] & mask1) != 0){

			exist = true;
			int begin_frame = i;
		}
		
		while(exist){

			int count = 0;
			int temp_frame = begin_frame;
			int temp_frame_diff = temp_frame - base_frame_no;
			

			unsigned int temp_bitmap_index = temp_frame_diff / 4;
			unsigned char temp_mask1 = 0x80 >> temp_frame_diff % 4;
			if((bitmap[temp_bitmap_index] & temp_mask1) == 0){
				
				exist = false;
				i = temp_frame;
				//break;
			}else{

				exist = true;
				count++;
				if(count == _n_frames){ 
				//unsigned int begin_bitmap_index = (begin_frame - base_frame_no)/4;
					
					unsigned char temp_mask2 = 0x08 >> temp_frame_diff % 4;
					bitmap[temp_bitmap_index] ^= temp_mask2;
					nfreeframes -= _n_frames;
					return begin_frame;
				}
			}
		}	
	}
	return 0;
}

//unsigned long ContFramePool::get_frames(unsigned int _n_frames)
//{
//    // TODO: IMPLEMENTATION NEEEDED!
//    //assert(false);
//	assert(nfreeframes >= _n_frames);
//
//	unsigned int frame_no = base_frame_no;
//
//	unsigned int i = 0;
//	if(bitmap[i] <= 0x0F && i < ninfoframes * FRAME_SIZE) i++;
//	//if we reach the end of the last info frame, then we return failure
//	if(i == ninfoframes * FRAME_SIZE) return 0;	
//
//	frame_no += i * 4;
//
//	//right now we get to the point that there are free frames available
//	//for current i
//	//unsigned int count = 0;
//
//	
//	for(; i < ninfoframes * FRAME_SIZE; i++){
//		unsigned char mask1 = 0x80;
//		unsigned char mask2 = 0x08;
//		if(bitmap[i] > 0x0F){
//			for(int j = 0; j < 4; j++){
//				if(mask1 & bitmap[i] == 0){
//					mask1 >> 1;
//					mask2 >> 1;
//					frame_no++;
//				}else if(areFramesAvailable(i,j,mask1,_n_frames)){
//					bitmap[i] = bitmap[i] ^ mask2;
//					nfreeframes -= _n_frames;
//					return frame_no;				
//				}
//			}
//		}
//		
///		if(i == ninfoframes * FRAME_SIZE) return 0;
//	}	
//}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no, unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // assert(false);
	int i;
	for(i = _base_frame_no; i < _base_frame_no + _n_frames; i++){
		assert ((i >= base_frame_no) && (i < base_frame_no + nframes))
		
		unsigned int bitmap_index = (i - base_frame_no) / 4;
		unsigned char mask = 0x80 >> ((i - base_frame_no) % 4);

		assert((bitmap[bitmap_index]) != 0);

		//update the bitmap
		bitmap[bitmap_index] ^= mask;

	}
	nfreeframes -= _n_frames;

}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
    //assert(false);
	node *currentNode = (*pool_list_head).next;
	ContFramePool cur_pool = *((*currentNode).currentPool);
	while(!(_first_frame_no >= cur_pool.base_frame_no && 
		 _first_frame_no < cur_pool.base_frame_no+cur_pool.nframes)){
		currentNode = (*currentNode).next;
		cur_pool = *((*currentNode).currentPool);
	}

	unsigned int bitmap_index = (_first_frame_no - cur_pool.base_frame_no) / 4;
	unsigned char mask1 = 0x08 >> ((_first_frame_no - cur_pool.base_frame_no) % 4);
	assert((cur_pool.bitmap[bitmap_index] & mask1) == 0);
	cur_pool.bitmap[bitmap_index] ^= mask1;
//	unsigned char mask = 0x80 >> ((_first_frame_no - cur_pool.base_frame_no) % 4);
	
	int i;
	for(i = _first_frame_no; i < cur_pool.base_frame_no + cur_pool.nframes; i++){
		int frame_diff = i - cur_pool.base_frame_no;
		bitmap_index = frame_diff / 4;
		//when reach the head of sequence
		if((0x08 >> (frame_diff % 4)) & cur_pool.bitmap[bitmap_index] == 0) break;
		//when there are free frames
		if((0x08 >> (frame_diff % 4)) & cur_pool.bitmap[bitmap_index] == 0) break;
		
		cur_pool.bitmap[bitmap_index] ^= 0x08 >> (frame_diff%4);
	}
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
//    assert(false);
	//16384 = 16 k = 4KB * 4
	return (_n_frames / 16384 + (_n_frames % 16384 > 0 ? 1 : 0));
	

}
