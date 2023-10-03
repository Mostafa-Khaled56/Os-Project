#include <inc/lib.h>
#define frames (USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE
// malloc()
//	This function use NEXT FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
char* Last_Allocation = (char*) USER_HEAP_START;
int heap_no_of_pages[(USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE];
void* malloc(uint32 size) {

	void* ret;
	int page_count = (size / PAGE_SIZE) + (size % PAGE_SIZE != 0);
	int page_number = (Last_Allocation - (char*) USER_HEAP_START) / PAGE_SIZE;
	int i, j = 0;
	if (page_count + 1 < frames) {

		while (1 == 1) {
			if (Last_Allocation > (char*) USER_HEAP_MAX) {
				Last_Allocation = (char*) USER_HEAP_START;
				page_number = 0;
				j++;
			}
			if ((((char*) USER_HEAP_MAX - Last_Allocation) / PAGE_SIZE)
					< page_count) {
				Last_Allocation = (char*) USER_HEAP_START;
				page_number = 0;
				j++;
			}
			if (heap_no_of_pages[page_number] == 0) {
				for (i = 0; i < page_count; i++) {
					if (heap_no_of_pages[page_number + i] != 0) {
						page_number += i;
						Last_Allocation += i * PAGE_SIZE;
						j++;
						break;
					}
				}
				ret = (void*) Last_Allocation;
				if (i == page_count) {
					for (int i = 0; i < page_count; i++) {
						heap_no_of_pages[page_number + i] = page_count;
						Last_Allocation += PAGE_SIZE;
					}
					sys_allocateMem((uint32) ret, page_count);
					return (void*) ret;
					break;
				}

				i = 0;
			} else {
				page_number++;
				j++;
				Last_Allocation += PAGE_SIZE;
			}
			if (j >= frames)
				return NULL;
		}

	} else {
		return NULL;
	}
	return NULL;
}

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable) {
	panic("smalloc() is not required ..!!");
	return NULL;
}

void* sget(int32 ownerEnvID, char *sharedVarName) {
	panic("sget() is not required ..!!");
	return 0;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address) {
	int page_number = ((char*) virtual_address - (char*) USER_HEAP_START)
			/ PAGE_SIZE;
	int next = heap_no_of_pages[page_number];
	sys_freeMem((uint32) virtual_address, next);
	for (int i = 0; i < next; i++) {
		heap_no_of_pages[page_number] = 0;
		page_number++;
	}
}

void sfree(void* virtual_address) {
	panic("sfree() is not requried ..!!");
}

//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size) {
	//TODO: [PROJECT 2022 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

	return NULL;
}
