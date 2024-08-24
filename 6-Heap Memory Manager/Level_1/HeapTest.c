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
extern sint32 SimHeap[MAX_HEAPLENGHT];          // simulated heap
extern sint32 CurBreak;                         // break pointer on simulated heap
extern sint32 Head;                             // define first index of free space
extern sint32 Tail;                             // define last index of free spaces


/*=========================  Functions Implementation ===========================*/
sint32 getIndex(sint32* ptr){
    sint32 indexOfptr = 0 ;          // to know what is the index in simulated heap for ptr 
    while ( &SimHeap[indexOfptr++] != (ptr-1) ); // to stop on index of ptr
    indexOfptr--;  
    return indexOfptr ;
}

void HeapTest_PrintBordersState() {
    printf("-----------------------------------------------------------------------------------------\n");
    printf("Head: %d, Tail: %d\n", Head, Tail);
    printf("SimHeap[Head]: %d, SimHeap[Head+1]: %d, SimHeap[Head+2]: %d\n",
        SimHeap[Head], SimHeap[Head+1], SimHeap[Head+2]);
    printf("SimHeap[Tail]: %d, SimHeap[Tail+1]: %d, SimHeap[Tail+2]: %d\n",
        SimHeap[Tail], SimHeap[Tail+1], SimHeap[Tail+2]);
    printf("-----------------------------------------------------------------------------------------\n");

}

void HeapTest_RandomAllocateFreeTest() {
    srand((unsigned int)time(NULL));
    
    void* pointers[NUM_ALLOCS] = {NULL};
    
    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        int index = rand() % NUM_ALLOCS;
        if (pointers[index] == NULL) {
            // Allocate memory
            sint32 size = (sint32)(rand() % MAX_SIZE) + 1;
#if DEBUGGING == ENABLE
            printf("\n\n\nBefore Malloc function\n");
            HeapTest_PrintBordersState();
#endif
            pointers[index] = HeapManager_Malloc(size);
            if (pointers[index] != NULL) {
                printf("Allocated memory of size %5d at address %p\n", size, pointers[index]);
                
            /* to allocate random data into memory that you reserved */
            for (sint32 j = 0; j < (sint32)(size / sizeof(sint32)); j++) {
                ((sint32*)pointers[index])[j] = 3;
            }

#if DEBUGGING == ENABLE
                printf("size = %5d\n",size);
                printf("After Malloc function Allcoate at : %5d\n", getIndex(pointers[index]) );
                HeapTest_PrintBordersState();
                printf("\n\n\n");
#endif
            } else {
                fprintf(stderr, "Allocation failed for size %5d\n", size);
            }
        } else {
            // Free memory
            printf("Freeing memory at address %p\n", pointers[index]);
            HeapManager_Free(pointers[index]);
#if DEBUGGING == ENABLE
            // Calculate the target index
            sint32 targetIndex = (sint32)(((sint32*)((uint8*)pointers[index] - sizeof(sint32))) - SimHeap);
    
            // Print the relevant information for the current free node
            printf("\n-------------------  Free Memory in this index only  ----------------------------\n");
            printf("| Index | Block Size | Prev Free Block | Next Free Block |\n");
            printf("----------------------------------------------------\n");
            printf("| %5d | %10d | %15d | %15d |\n", 
                   targetIndex, 
                   SimHeap[targetIndex], 
                   SimHeap[targetIndex + PREVIOUS_FREE_BLOCK_SHIFT], 
                   SimHeap[targetIndex + NEXT_FREE_BLOCK_SHIFT]);
#endif
            pointers[index] = NULL;
        }
    }

    // Free remaining allocated memory
    for (int i = 0; i < NUM_ALLOCS; ++i) {
        if (pointers[i] != NULL) {
            // Free memory
            printf("Freeing remaining memory at address %p\n", pointers[i]);
            HeapManager_Free(pointers[i]);
            
#if DEBUGGING == ENABLE
            // Calculate the target index
            sint32 targetIndex = (sint32)(((sint32*)((uint8*)pointers[i] - sizeof(sint32))) - SimHeap);

            // Print the relevant information for the current free node
            printf("\n-------------------------  Free Remaining Memory   ---------------------------\n");
            printf("| Index | Block Size | Prev Free Block | Next Free Block |\n");
            printf("----------------------------------------------------\n");
            printf("| %5d | %10d | %15d | %15d |\n", 
                   targetIndex, 
                   SimHeap[targetIndex], 
                   SimHeap[targetIndex + PREVIOUS_FREE_BLOCK_SHIFT], 
                   SimHeap[targetIndex + NEXT_FREE_BLOCK_SHIFT]);
#endif
            // Clear the pointer
            pointers[i] = NULL;
        }
    }
}

void HeapTest_PrintFreeListFromHead() {
    sint32 current = Head;

    printf("Free List From Head:\n");
    printf("----------------------------------------------------\n");
    printf("| Index | Block Size | Prev Free Block | Next Free Block |\n");
    printf("----------------------------------------------------\n");

    sint32 size = SimHeap[current];
    sint32 prev = SimHeap[current + PREVIOUS_FREE_BLOCK_SHIFT];
    sint32 next = SimHeap[current + NEXT_FREE_BLOCK_SHIFT];

    while (current != SYMBOL_OF_HEAP_NULL && current != prev) {
        size = SimHeap[current];
        prev = SimHeap[current + PREVIOUS_FREE_BLOCK_SHIFT];
        next = SimHeap[current + NEXT_FREE_BLOCK_SHIFT];

        printf("| %5d | %10d | %15d | %15d |\n", current, size, prev, next);

        current = next;
    }

    printf("----------------------------------------------------\n");
}




void HeapTest_PrintFreeListFromTail() {
    sint32 current = Tail;

    printf("Free List From Tail:\n");
    printf("----------------------------------------------------\n");
    printf("| Index | Block Size | Prev Free Block | Next Free Block |\n");
    printf("----------------------------------------------------\n");

    sint32 size = SimHeap[current];
    sint32 prev = SimHeap[current + PREVIOUS_FREE_BLOCK_SHIFT];
    sint32 next = SimHeap[current + NEXT_FREE_BLOCK_SHIFT];

    while (current != SYMBOL_OF_HEAP_NULL && current != next) {
        size = SimHeap[current];
        prev = SimHeap[current + PREVIOUS_FREE_BLOCK_SHIFT];
        next = SimHeap[current + NEXT_FREE_BLOCK_SHIFT];

        printf("| %5d | %10d | %15d | %15d |\n", current, size, prev, next);

        current = prev;
    }

    printf("----------------------------------------------------\n");
}