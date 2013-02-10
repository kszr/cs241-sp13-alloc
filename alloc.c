/** @file alloc.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "debug.h"

/**
 * Allocate space for array in memory
 * 
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 * 
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size)
{
	/* Note: This function is complete. You do not need to modify it. */
	void *ptr = malloc(num * size);
	
	if (ptr)
		memset(ptr, 0x00, num * size);

	return ptr;
}


/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */

static char *_start = NULL;

typedef struct _metadata {
	size_t _size; //the size in bytes of the current block
	struct _metadata *_next; //a pointer to the next free block of memory
} metadata;

static metadata *_head = NULL; //the head of the free list structure

/**
 * IMPLEMENTATION PLAN:
 * The head of the free list structure, composed of links of metadata
 * whose _in_use bits are set to 0. The head is naturally NULL until
 * the first free has been made. Every time malloc() is called other
 * than the very first call, the free list is traversed to ascertain
 * whether any of the extant free blocks are usable for this purpose.
 * To this end we use a first-fit strategy. When a block is freed, a
 * link is added to the link list between links that occur before it
 * and after it respectively in memory. When a freed block is used
 * again, it is removed from the linked list.
 */ 

void *malloc(size_t size)
{
	if(!_start) //If the heap is empty
	{	
		_start = (char *) sbrk(0);
		char *return_ptr = (char *) sbrk(size+sizeof(metadata));
		metadata *data = (metadata *) return_ptr;
		data->_next = NULL;
		data->_size = size;
		return_ptr += sizeof(metadata);
printf("Statement 1\n");
		return return_ptr;
	}

	metadata *curr = _head; //We shall use ptr to iterate through the free list
	metadata *prev = NULL; 	//There is nothing before the head of a list

	while(curr)
	{	
		//if the current free block is big enough to use...
		if(curr->_size >= size)
		{
			//If curr is other than _head
			if(prev)
				prev->_next = curr->_next;	
			//If curr is head, though this probably did
			//not need to be spelt out.
			if(curr == _head)
				_head = curr->_next;
		
			curr->_next = NULL;
printf("Malloc loop\n");
			return (char *) curr + sizeof(metadata);
		}
		prev = curr;
		curr = curr->_next;
	}
	/**
 	 * Control will reach this point only if there are no free blocks
 	 * in the heap large enough for this allocation. We need to make
 	 * the heap larger now.
 	 */
    char *ptr = (char *) sbrk(size+sizeof(metadata));
	metadata *data = (metadata *) ptr;
    data->_size = size;
printf("Malloc end\n");
    return ptr + sizeof(metadata);
}


/**
 * Deallocate space in memory
 * 
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr)
{
	// "If a null pointer is passed as argument, no action occurs."
	if (!ptr)
		return;

	//Look at the metadata for this block.
	metadata *freed = (metadata *) ( (char *) ptr - sizeof(metadata));
	
	if(!_head)
	{	
		_head = freed;
printf("free head\n");
		return;
	}

	metadata *curr = _head;
	metadata *prev = NULL;

	while(curr)
	{
		if(curr == _head && curr>freed)
		{
			freed->_next = curr;
			_head = freed;
			return;
		}
		else if(prev < freed && curr > freed)
		{
			prev->_next = freed;
			freed->_next = curr;
			return;
		}

		prev = curr;
		curr = curr->_next;
printf("Free loop\n");
	}
	
	/**
 	 * If this block is located further in the heap than anything
 	 * in the free list, then it is to be inserted at the end of
 	 * the list. When control exits the while loop, curr is NULL,
 	 * and prev is the last link in the list. To this link will
 	 * be appended the newly freed block.
 	 */
printf("nearly freed\n");
	prev->_next = curr;
printf("Free end\n");
	return;
}


/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *    
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size)
{
	// "In case that ptr is NULL, the function behaves exactly as malloc()"
	if (!ptr)
		return malloc(size);

	 // "In case that the size is 0, the memory previously allocated in ptr
	 //  is deallocated as if a call to free() was made, and a NULL pointer
	 //  is returned."
	if (!size)
	{
		free(ptr);
		return NULL;
	}
	
	return NULL;

}
