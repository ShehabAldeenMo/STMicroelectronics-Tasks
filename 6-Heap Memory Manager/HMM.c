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
uint32 SimHeap[MAX_HEAPLENGHT];          // simulated heap
sint32 CurBreak;                         // break pointer on simulated heap
uint8* border="---------------------------------------------------------------------------------";
uint32 Head;                             // define first index of free space
uint32 Tail;                             // define last index of free spaces




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
uint32* HMM_Malloc(uint32 size);

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
void HMM_Free(uint32* ptr);

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



/*==================================  main =====================================*/
#define NUM_OF_ITER       15
uint8 main (){
    uint32* PtrArray[NUM_OF_ITER];

    HMM_Init();

    char i = 0 ;
    while (i < NUM_OF_ITER){
        PtrArray[i] = HMM_Malloc(8);
        i++;
    }

    HMM_Free ( PtrArray[1] );
    HMM_Free ( PtrArray[4] );
    HMM_Free ( PtrArray[7] );
    HMM_Free ( PtrArray[10] );
    HMM_Free ( PtrArray[13] );

    return 0 ;
}



/*============================== Functions Implementation =====================*/
uint32* HMM_Malloc(uint32 size){
#if (FIRSTFIT == ENABLE )
    uint32 index = Helper_FirstFit(size);
#else
    uint32 index = Helper_BestFit(size);
#endif
    return  (uint32*) (&(SimHeap[index])) ;
}

void HMM_Free(uint32* ptr){
    uint32 sizeOfptr = *(ptr-1); // to know size  of this pointer
    uint32 indexOfptr = 0 ;          // to know what is the index in simulated heap for ptr 

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
    CurBreak = ONE_K-1 ;
    SimHeap[Head] = ONE_K-1 ;
    SimHeap[Head+PREVIOUS_FREE_BLOCK_SHIFT] = SYMBOL_OF_HEAP_NULL ; 
    SimHeap[Head+NEXT_FREE_BLOCK_SHIFT] = SYMBOL_OF_HEAP_NULL ; 
}