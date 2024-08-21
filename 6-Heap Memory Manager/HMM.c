/*============================================================================
 * @file name      : Helper.c
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This file contains the implementation of helper functions for the Heap Memory 
 * Manager (HMM). These functions provide support for dynamic memory allocation 
 * and deallocation within a simulated heap environment, which uses a large 
 * statically allocated array to represent the heap. The HMM does not depend on 
 * the operating system's kernel for memory management, allowing for easier 
 * debugging and a controlled learning environment.
 *
 =============================================================================
 * @Notes:
 * - The HMM is implemented entirely in user space, simulating heap operations 
 *   without kernel-level dependencies.
 * - Functions like HmmAlloc and HmmFree are provided to manage memory 
 *   dynamically within the simulated heap.
 * - The implementation assumes that the memory is allocated and freed in a 
 *   controlled manner without external interference.
 * - Ensure that the simulated heap array is large enough to handle expected 
 *   allocations.
 *
 ******************************************************************************
 ==============================================================================
*/


/*===================================  Includes ===============================*/
#include "Helper.h"


/*=============================  Global Variables ==============================*/
sint32 SimHeap[MAX_HEAPLENGHT];          // simulated heap
sint32 CurBreak;                         // break pointer on simulated heap
sint32 Head;                             // define first index of free space
sint32 Tail;                             // define last index of free spaces


/*==========================  Function Prototypes ===========================*/
/*
 * Name             : HMM_Malloc
 * Description      : Allocates a block of memory from the simulated heap using the specified 
 *                    allocation strategy (First Fit or Best Fit). The allocated memory's index 
 *                    is determined based on the chosen fit algorithm.
 * Input            : size - The size of the memory block to be allocated (in bytes).
 * Output           : None
 * Return           : Returns a pointer to the allocated memory block within the simulated heap.
 * Notes            : This function abstracts the memory allocation process, depending on the 
 *                    strategy defined by the user. If FIRSTFIT is enabled, the allocation uses 
 *                    the First Fit strategy; otherwise, it uses the Best Fit strategy.
 */
sint32* HMM_Malloc(sint32 size);

/*
 * Name             : HMM_Free
 * Description      : Frees a previously allocated block of memory, identified by the pointer 
 *                    provided. The function coalesces adjacent free blocks to minimize fragmentation.
 * Input            : ptr - A pointer to the memory block that needs to be freed.
 * Output           : None
 * Return           : None
 * Notes            : This function handles different scenarios depending on the position of 
 *                    the block to be freed relative to the head and tail nodes. It ensures 
 *                    optimal memory usage by merging adjacent free blocks when possible.
 */
void HMM_Free(sint32* ptr);

/*
 * Name             : HMM_Init
 * Description      : Initializes the simulated heap by setting up the head, tail, and program 
 *                    break pointer. The heap is prepared with initial values to manage memory 
 *                    allocation and deallocation.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : This function must be called before any memory allocation or deallocation 
 *                    operations to ensure that the simulated heap is properly initialized.
 */
static void HMM_Init();



/*==================================  testing =====================================*/
#include <time.h>
#define NUM_ALLOCS 10000
#define MAX_SIZE 10240
#define MAX_ITERATIONS 1000000


void PrintFreeListFromHead();

void printHeapState() {
    printf("-----------------------------------------------------------------------------------------\n");
    printf("Head: %d, Tail: %d\n", Head, Tail);
    printf("SimHeap[Head]: %d, SimHeap[Head+1]: %d, SimHeap[Head+2]: %d\n",
        SimHeap[Head], SimHeap[Head+1], SimHeap[Head+2]);
    printf("SimHeap[Tail]: %d, SimHeap[Tail+1]: %d, SimHeap[Tail+2]: %d\n",
        SimHeap[Tail], SimHeap[Tail+1], SimHeap[Tail+2]);
    printf("-----------------------------------------------------------------------------------------\n");

}

void random_alloc_free_test() {
    srand((unsigned int)time(NULL));
    
    void* pointers[NUM_ALLOCS] = {NULL};
    
    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        int index = rand() % NUM_ALLOCS;
        if (pointers[index] == NULL) {
            // Allocate memory
            size_t size = (size_t)(rand() % MAX_SIZE) + 1;
            printf("\n\n\nBefore Malloc function\n");
            printHeapState();
            pointers[index] = HMM_Malloc(size);
            if (pointers[index] != NULL) {
                printf("Allocated memory of size %zu at address %p\n", size, pointers[index]);
                printf("size = %ld\n",size);
                printf("After Malloc function\n");
                printHeapState();
                printf("\n\n\n");
            } else {
                fprintf(stderr, "Allocation failed for size %zu\n", size);
            }
        } else {
            // Free memory
            HMM_Free(pointers[index]);
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
            pointers[index] = NULL;
        }
    }

    uint8* border = " ============================================================================" ; 
    printf("%s\n%s\n",border,border);

    // Free remaining allocated memory
    for (int i = 0; i < NUM_ALLOCS; ++i) {
        if (pointers[i] != NULL) {
            // Free memory
            HMM_Free(pointers[i]);

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

            // Clear the pointer
            pointers[i] = NULL;
        }
    }
}

void PrintFreeListFromHead() {
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




void PrintFreeListFromTail() {
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
/*==================================  main =====================================*/
uint8 main (){
    HMM_Init();
    //sint32* ptr[10] ;
    /*for (uint8 i =0 ; i < 10 ; i++){
        size_t size = (size_t)(rand() % MAX_SIZE) + 1;
        ptr[i] = HMM_Malloc(size);
    }*/

    printf("Starting random allocation and deallocation test...\n");
    random_alloc_free_test();
    printf("Test complete.\n");

    return 0 ;
}



/*============================== Functions Implementation =====================*/
sint32* HMM_Malloc(sint32 size){
#if (FIRSTFIT == ENABLE )
    sint32 indexOfData = Helper_FirstFit(size);
#else
    sint32 index = Helper_BestFit(size);
#endif
    return  (sint32*) (&(SimHeap[indexOfData])) ;
}

void HMM_Free(sint32* ptr){
    sint32 sizeOfptr = *(ptr-1); // to know size  of this pointer
    sint32 indexOfptr = 0 ;          // to know what is the index in simulated heap for ptr 

    while ( &SimHeap[indexOfptr++] != (ptr-1) ); // to stop on index of ptr
    indexOfptr--;                           // to stop on index of metadata for ptr

    /* if indexOfptr is pointing to node before head node, there are two scenarios
    * 1) indexOfptr point to node away from head
    * 2) indexOfptr point to node just before head from left so we will extend head node to join two adjecent free spaces
    * */
    if ( indexOfptr < Head ){
        Helper_FreeOperationBeforeHead(indexOfptr,sizeOfptr);
    }
    /* if indexOfptr is pointing to node after tail node, there are two scenarios
    * 1) indexOfptr point to node away from tail
    * 2) indexOfptr point to node just after tail from right so we will extend tail node to join two adjecent free spaces
    * */
    else if (indexOfptr > Tail){
        Helper_FreeOperationAfterTail(indexOfptr,sizeOfptr);
    }
    /* if indexOfptr is pointing to node after head node and before tail node, there are four scenarios
    * 1) indexOfptr point to node away not adjecent for any free node so we willn't extend and free node
    * 2) indexOfptr point to node just after free node(x) from right so we will extend x node to join two adjecent free spaces
    * 3) indexOfptr point to node just before free node(y) from left so we will extend y node to join two adjecent free spaces
    * 4) indexOfptr point to node just after free node(x) and before free node(y) from right so we will extend x,y node to join three adjecent free spaces
    * */
    else if (indexOfptr > Head && indexOfptr < Tail){
        Helper_FreeOperationMiddleNode(indexOfptr,sizeOfptr);
    }
}

void HMM_Init(){
    Head = 0 ;
    Tail = 0 ;
    CurBreak = BREAK_STEP_SIZE-1 ;
    SimHeap[Head] = BREAK_STEP_SIZE-1 ;
    SimHeap[Head+PREVIOUS_FREE_BLOCK_SHIFT] = SYMBOL_OF_HEAP_NULL ; 
    SimHeap[Head+NEXT_FREE_BLOCK_SHIFT] = SYMBOL_OF_HEAP_NULL ; 
    printHeapState();
} 