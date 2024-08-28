/*============================================================================
 * @file name      : HeapManager.c
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This file contains the implementation of functions for managing a simulated heap memory.
 * It includes implementations for allocating and freeing memory blocks using either the
 * First Fit or Best Fit algorithm, depending on the configuration. Functions handle memory
 * allocation, deallocation, and coalescing of free blocks to reduce fragmentation.
 *
=============================================================================
 * @Notes:
 * - The allocation strategy is controlled by the macro `FIRSTFIT`. If `FIRSTFIT` is 
 *   set to `ENABLE`, the First Fit algorithm is used for allocation; otherwise, the 
 *   Best Fit algorithm is used.
 * - Ensure that the `HeapUtils.h` header file is included as it contains necessary utility 
 *   functions used by these implementations.
 *
 ******************************************************************************
 ==============================================================================
*/



/*===================================  Includes ===============================*/
#include "HeapManager.h"


/*=============================  Global Variables ==============================*/
sint64       SimHeap[MAX_HEAPLENGHT];                      // simulated heap
FreeBlock*   ptrHead     = NULL ;
FreeBlock*   ptrTail     = NULL ;
static uint8 InitFlag    = ON ;
sint8*       CurBreak    = NULL;                           // break pointer on simulated heap


void* HeapManager_Malloc(size_t size) {
    if (InitFlag == ON) {
        HeapExtras_Init();
        InitFlag = OFF;
    }
 
    sint8* ptrOfData = HeapExtras_FirstFit(size);

    // Return a pointer to the allocated memory, cast to void*
    return (void*)ptrOfData;
}


void HeapManager_Free(void* ptr){
    if (ptr == NULL) {
#if DEBUGGING == ENABLE
        printf("Passing NULL to free function\n");
        exit(INVALID);
#endif
        return ;
    }

    // Calculate the address of the FreeBlock metadata
    FreeBlock* deletedBlock = (FreeBlock*)((sint8*)ptr - sizeof(size_t));
    
    // If `deletedBlock` is pointing to a node before the head node
    if (deletedBlock < ptrHead) {
        HeapExtras_FreeOperationBeforeHead(&deletedBlock);
    }
    // If `deletedBlock` is pointing to a node after the tail node
    else if (deletedBlock > ptrTail) {
        HeapExtras_FreeOperationAfterTail(&deletedBlock);
    }
    // If `deletedBlock` is pointing to a node between head and tail nodes
    else if (deletedBlock > ptrHead && deletedBlock < ptrTail) {
        HeapExtras_FreeOperationMiddleNode(&deletedBlock);
    }
    else {
#if DEBUGGING == ENABLE
        printf("Error: deletedBlock is not within valid heap limits \n");
#endif
        while (1); // For testing purposes
        exit(INVALID);
    }
}

size_t HeapManager_GetSize(void* ptr){
    // Assuming that the metadata (FreeBlock structure) precedes the data
    FreeBlock* block = (FreeBlock*)(ptr - sizeof(size_t));
    size_t size = block->BlockSize;

    return size ;
}