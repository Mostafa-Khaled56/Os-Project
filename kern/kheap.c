#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)

#define NUM_OF_KHEAP_PAGES ((KERNEL_HEAP_MAX - KERNEL_HEAP_START)/PAGE_SIZE)

struct NextFit {
	uint32 usedPages;
	int allocatedSize;
} nextFit[NUM_OF_KHEAP_PAGES];

struct Addresses {
	uint32 virtualAddress;
	int numOfPages;
} addresses[NUM_OF_KHEAP_PAGES];

uint32 lastAllocatedIndex = 0;
uint32 lastAddress = 0;
int index = 0;

uint32 NextFitPlacementStrategy(uint32 pageNumbers) {
	int avaliableAllocation = 0;
	uint32 startAddress = 0;
	uint32 allocationNumber = 0;
	uint32 indexOfAllocation = lastAllocatedIndex;
	uint32 vAddress = (indexOfAllocation * PAGE_SIZE) + KERNEL_HEAP_START;

	do {
		if (nextFit[indexOfAllocation].usedPages == 0) {
			allocationNumber++;
			if (startAddress == 0) {
				startAddress = (indexOfAllocation * PAGE_SIZE)
						+ KERNEL_HEAP_START;
			}

			if (allocationNumber == pageNumbers) {
				avaliableAllocation = 1;
				break;
			}
			indexOfAllocation++;
		} else {
			allocationNumber = 0;
			startAddress = 0;
			indexOfAllocation++;
		}

		if (((indexOfAllocation * PAGE_SIZE) + KERNEL_HEAP_START)
				== KERNEL_HEAP_MAX) {
			allocationNumber = 0;
			startAddress = 0;
			indexOfAllocation = 0;

		}

	} while (!avaliableAllocation
			&& ((indexOfAllocation * PAGE_SIZE) + KERNEL_HEAP_START) != vAddress);

	if (avaliableAllocation == 1) {
		lastAllocatedIndex = indexOfAllocation;
		return startAddress;
	}

	return 0;
}

void* kmalloc(unsigned int size) {
	uint32 Size = ROUNDUP(size, PAGE_SIZE);
	int numOfPages = Size / PAGE_SIZE;
	if (isKHeapPlacementStrategyNEXTFIT()) {
		uint32 firstAddress;
		uint32 address;

		firstAddress = NextFitPlacementStrategy(numOfPages);

		if (firstAddress == 0) {
			return NULL;
		}

		address = firstAddress;

		for (int j = 0; j < numOfPages; j++) {
			struct Frame_Info * ptr_free_info;
			int ret = allocate_frame(&ptr_free_info);

			if (ret == E_NO_MEM) {
				return NULL;
			}
			int r = map_frame(ptr_page_directory, ptr_free_info,
					(void *) address, PERM_WRITEABLE | PERM_PRESENT);
			if (r == E_NO_MEM) {
				free_frame(ptr_free_info);
				return NULL;
			}

			nextFit[(address - KERNEL_HEAP_START) / PAGE_SIZE].usedPages = 1;
			address += PAGE_SIZE;
		}

		nextFit[(address - KERNEL_HEAP_START) / PAGE_SIZE].allocatedSize =
				numOfPages;
		lastAddress = address;
		addresses[index].virtualAddress = firstAddress;
		addresses[index].numOfPages = numOfPages;
		index++;

		return (void*) firstAddress;
	}
	return 0;
}

void kfree(void* virtual_address) {
	void* virtualAddress = virtual_address;
	int pageNumbers;
	nextFit[((uint32) virtualAddress - KERNEL_HEAP_START) / PAGE_SIZE].allocatedSize =
			0;

	for (int j = 0; j < index; j++) {
		if ((int) virtualAddress == addresses[j].virtualAddress) {
			pageNumbers = addresses[j].numOfPages;
			break;
		}
	}

	for (int j = 0; j < pageNumbers; j++) {
		unmap_frame(ptr_page_directory, virtualAddress);
		nextFit[((uint32) virtualAddress - KERNEL_HEAP_START) / PAGE_SIZE].usedPages =
				0;
		virtualAddress += PAGE_SIZE;
	}
}

unsigned int kheap_virtual_address(unsigned int physical_address) {
	uint32* ptr_page_table = NULL;

	for (uint32 virtualAddress = KERNEL_HEAP_START;
			virtualAddress < lastAddress; virtualAddress += PAGE_SIZE)
			{
		struct Frame_Info* ptrFrameInfo = get_frame_info(ptr_page_directory,
				(void*) virtualAddress, &ptr_page_table);
		uint32 physicalAddress = to_physical_address(ptrFrameInfo);

		if (physicalAddress == physical_address) {
			return virtualAddress;
		}
	}

	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address) {
	uint32* ptr_page_table = NULL;
	struct Frame_Info* ptrFrameInfo = get_frame_info(ptr_page_directory,
			(void*) virtual_address, &ptr_page_table);

	if (ptrFrameInfo == NULL) {
		return 0;
	}
	uint32 physicalAddress = to_physical_address(ptrFrameInfo);

	return physicalAddress;
}
