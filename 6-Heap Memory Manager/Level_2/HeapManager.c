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
sint32 SimHeap[MAX_HEAPLENGHT];          // simulated heap
sint32 CurBreak;                         // break pointer on simulated heap
sint32 Head;                             // define first index of free space
sint32 Tail;                             // define last index of free spaces
static uint8 InitFlag = ON ;

sint32* HeapManager_Malloc(sint32 size){
    if (InitFlag == ON){
        HeapExtras_Init();
        InitFlag = OFF ;
    }
    
#if (FIRSTFIT == ENABLE )
    sint32 indexOfData = HeapExtras_FirstFit(size);
#else
    sint32 indexOfData = HeapExtras_BestFit(size);
#endif

    if (indexOfData == INVALID){
        return NULL;
    }

    return  (sint32*) (&(SimHeap[indexOfData])) ;
}

void HeapManager_Free(sint32* ptr){
    sint32 sizeOfptr = *(ptr-1); // to know size  of this pointer
    sint32 indexOfptr = 0 ;          // to know what is the index in simulated heap for ptr 

    while ( &SimHeap[indexOfptr++] != (ptr-1) ); // to stop on index of ptr
    indexOfptr--;                           // to stop on index of metadata for ptr

    /* if indexOfptr is pointing to node before head node, there are two scenarios
    * 1) indexOfptr point to node away from head
    * 2) indexOfptr point to node just before head from left so we will extend head node to join two adjecent free spaces
    * */
    if ( indexOfptr < Head ){
        HeapExtras_FreeOperationBeforeHead(indexOfptr,sizeOfptr);
    }
    /* if indexOfptr is pointing to node after tail node, there are two scenarios
    * 1) indexOfptr point to node away from tail
    * 2) indexOfptr point to node just after tail from right so we will extend tail node to join two adjecent free spaces
    * */
    else if (indexOfptr > Tail){
        HeapExtras_FreeOperationAfterTail(indexOfptr,sizeOfptr);
    }
    /* if indexOfptr is pointing to node after head node and before tail node, there are four scenarios
    * 1) indexOfptr point to node away not adjecent for any free node so we willn't extend and free node
    * 2) indexOfptr point to node just after free node(x) from right so we will extend x node to join two adjecent free spaces
    * 3) indexOfptr point to node just before free node(y) from left so we will extend y node to join two adjecent free spaces
    * 4) indexOfptr point to node just after free node(x) and before free node(y) from right so we will extend x,y node to join three adjecent free spaces
    * */
    else if (indexOfptr > Head && indexOfptr < Tail){
        HeapExtras_FreeOperationMiddleNode(indexOfptr,sizeOfptr);
    }
    else {
        printf("error in freeing heap that indexOfptr is not exist in limits : %5d\n",indexOfptr);
        while(1);///////////////////////////// for testing
        exit(INVALID);
    }
}