#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)

//=================================================================================//
//============================ REQUIRED FUNCTION ==================================//
//=================================================================================//
struct Size_of_Address_KHeap{
	uint32 size;
	void *va;
	uint32 empty;
};
struct Size_of_Address_KHeap allocated[(KERNEL_HEAP_MAX-KERNEL_HEAP_START+1)/PAGE_SIZE];

uint32 virtualAddresses[1<<20];
uint32 count = 0;
void *firstFreeVAInKHeap  = (void*)(KERNEL_HEAP_START);
void* kmalloc(unsigned int size)   // it's void a7pipy :D
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kmalloc()
	// Write your code here, remove the panic and write your code
//	panic("kmalloc() is not implemented yet...!!");

	//NOTE: Allocation is continuous increasing virtual address
	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details


	   size=(size+PAGE_SIZE-1)/PAGE_SIZE;


		if(firstFreeVAInKHeap+size*PAGE_SIZE >= (void*)KERNEL_HEAP_MAX)
			return NULL;


		void* retVal = firstFreeVAInKHeap;

		for(int i = 0; i < size;i++ ){
			struct Frame_Info *ptr;
			if(allocate_frame(&ptr)==E_NO_MEM)
				return NULL;
			map_frame(ptr_page_directory, ptr, firstFreeVAInKHeap, PERM_PRESENT|PERM_WRITEABLE);
			virtualAddresses[to_physical_address(ptr)/PAGE_SIZE] = (uint32)firstFreeVAInKHeap;
			firstFreeVAInKHeap += PAGE_SIZE;



		}
		allocated[count].va = retVal;// first free va in kheap
		allocated[count].size = size;
		allocated[count].empty=0;
		count++;

		return retVal;


	//TODO: [PROJECT 2017 - BONUS1] Implement a Kernel allocation strategy
	// Instead of the continuous allocation/deallocation, implement both
	// FIRST FIT and NEXT FIT strategies
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy



	//change this "return" according to your answer
	//return NULL;


}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

	uint32  size=0, index;
	for(int i = 0; i < (KERNEL_HEAP_MAX-KERNEL_HEAP_START+1)/PAGE_SIZE;i++){
		if(allocated[i].va==virtual_address){
			size=allocated[i].size;
			index=i;
			break;
		}
	}
	allocated[index].empty=1;


	for(int i = 0; i < size; i++){



					virtualAddresses[(kheap_physical_address((uint32)virtual_address))/PAGE_SIZE]=0;
					unmap_frame(ptr_page_directory,virtual_address);
					virtual_address+=PAGE_SIZE;
	}



    tlbflush();
    return;


}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	return  virtualAddresses[physical_address/PAGE_SIZE];
}


unsigned int kheap_physical_address(unsigned int virtual_address)
{

	uint32* ptr_page;
		 int page_inex=PTX(virtual_address);

		get_page_table(ptr_page_directory,(void*)virtual_address,&ptr_page);
		uint32  entry_pt=ptr_page[page_inex];
		uint32 pa_page=(entry_pt>>12)*4*1024;
		return pa_page;
		/*
		uint32 *pageTable;
	 	get_page_table(ptr_page_directory,(void*)virtual_address,&pageTable);
		uint32 phyadd= (pageTable[PTX(virtual_address)]>>12);
		phyadd*=PAGE_SIZE;

		return phyadd ;
*/
	//TODO: [PROJECT 2017 - [1] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
//	panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	//return 0;
}


//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2017 - BONUS2] Kernel Heap Realloc
	// Write your code here, remove the panic and write your code

 	panic("krealloc() is not implemented yet...!!");
return NULL;
}
