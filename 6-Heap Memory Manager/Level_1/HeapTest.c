/*============================================================================
 * @file name      : HeapTest.c
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This file contains the implementation of testing functions for the simulated heap memory.
 * It includes functions for printing heap borders, performing random allocations and frees,
 * and printing the free list from both the head and tail. These tests help ensure the 
 * correctness and robustness of the heap manager's functionality.
 *
=============================================================================
 * @Notes:
 * - Ensure the proper inclusion of necessary headers.
 * - Modify the function implementations to match the actual heap management logic.
 *
 ******************************************************************************
 ==============================================================================
*/

/*===================================  Includes ==============================*/
#include "HeapTest.h"


/*============================  extern Global Variable ==============================*/
extern FreeBlock* ptrHead;
extern FreeBlock* ptrTail;


/*=========================  Functions Implementation ===========================*/

void HeapTest_PrintBordersState() {
    printf("-----------------------------------------------------------------------------------------\n");
    
    // Print information about ptrHead
    if (ptrHead != NULL) {
        printf("ptrHead: %p\n", (void*)ptrHead);
        printf("ptrHead BlockSize: %5ld\n", ptrHead->BlockSize);
        printf("ptrHead NextFreeBlock: %p\n", (void*)ptrHead->NextFreeBlock);
        printf("ptrHead PreviousFreeBlock: %p\n", (void*)ptrHead->PreviousFreeBlock);
    } else {
        printf("ptrHead is NULL\n");
    }
    
    // Print information about ptrTail
    if (ptrTail != NULL) {
        printf("ptrTail: %p\n", (void*)ptrTail);
        printf("ptrTail BlockSize: %5ld\n", ptrTail->BlockSize);
        printf("ptrTail NextFreeBlock: %p\n", (void*)ptrTail->NextFreeBlock);
        printf("ptrTail PreviousFreeBlock: %p\n", (void*)ptrTail->PreviousFreeBlock);
    } else {
        printf("ptrTail is NULL\n");
    }
    
    printf("-----------------------------------------------------------------------------------------\n");
}

void HeapTest_RandomAllocateFreeTest() {
    srand((unsigned int)time(NULL));
    
    void* pointers[NUM_ALLOCS] = {NULL};
    
    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        int index = rand() % NUM_ALLOCS;
        if (pointers[index] == NULL) {
            // Allocate memory
            size_t size = (sint32)(rand() % MAX_SIZE) + 1;
#if DEBUGGING == ENABLE
            printf("\n\nFree List Before\n");
            HeapTest_PrintBordersState();
#endif
            pointers[index] = HeapManager_Malloc(size);
#if DEBUGGING == ENABLE
            printf("\n\nFree List After\n");
            HeapTest_PrintBordersState();
#endif
            if (pointers[index] != NULL) {
                printf("Allocated memory of size %5ld at address %p\n", size, pointers[index]);
                
                // Fill the allocated memory with a specific value, e.g., 3
                memset(pointers[index], 3, size);
                
            } else {
                fprintf(stderr, "Allocation failed for size %5ld\n", size);
            }
        } else {
            // Free memory
            printf("Freeing memory at address %p\n", pointers[index]);
            HeapManager_Free(pointers[index]);
            pointers[index] = NULL;
        }
    }

    // Free remaining allocated memory
    for (int i = 0; i < NUM_ALLOCS; ++i) {
        if (pointers[i] != NULL) {
            // Free memory
            printf("Freeing remaining memory at address %p\n", pointers[i]);
            HeapManager_Free(pointers[i]);            
            // Clear the pointer
            pointers[i] = NULL;
        }
    }
}
