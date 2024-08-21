/*============================================================================
 * @file name      : HeapUtils.c
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This file contains the implementation of utility functions for managing memory 
 * within the simulated heap environment. These functions support various operations 
 * such as memory allocation, resizing, splitting free blocks, removing free blocks, 
 * and updating metadata in the heap. They work together with the heap manager to 
 * provide dynamic memory management using a statically allocated array to represent 
 * the heap.
 *
 =============================================================================
 * @Notes:
 * - The functions assume that the heap is managed in a simulated environment, where 
 *   operations are performed on a large statically allocated array.
 * - Functions handle edge cases such as padding, resizing, and updating pointers 
 *   to maintain the integrity of the heap.
 * - Proper handling of memory and pointer updates is crucial for correct operation 
 *   of the heap manager.
 *
 ******************************************************************************
 ==============================================================================
*/


/*===================================  Includes ==============================*/
#include "HeapUtils.h"



/*============================  extern Global Variable ==============================*/
extern sint32 SimHeap[MAX_HEAPLENGHT];          // simulated heap
extern sint32 CurBreak;                         // break pointer on simulated heap
extern sint32 Head;                             // define first index of free space
extern sint32 Tail;                             // define last index of free spaces




/*=========================  Functions Implementation ===========================*/
sint32 HeapUtils_AllocationCoreLoop(sint32 index, sint32 size){
    sint32 Ret_DataIndex = INVALID ;  
    sint32 padding =  SimHeap[index] - size ;

    // Handle padding cases
    if (padding >= 0 && padding < NUMBER_OF_FREE_NODE_ELEMENTS) {
        // If padding is 0, 1, or 2, we adjust the requested size to include the padding
        size += padding;
    }
    
    /* memory allocation action */
    if (SimHeap[index] > size){
        Ret_DataIndex = HeapUtils_SplitFreeBlock (index,size);
    }
    else if (SimHeap[index] == size){
        Ret_DataIndex = HeapUtils_RemoveFreeBlock (index,size);
    } 
    return Ret_DataIndex ; 
}



sint32 HeapUtils_sbrk (sint32 size){
    sint32 result = CurBreak + size ; // to use it in check size condition
    sint32 temp = 0 ; // to point of new free space allocation

    /*to ensure that the new current break in heap limitions*/
    if ( result < 0 ){
        printf("Invalid passing negative size parameter\n");
        return INVALID ;
    }
    else if (result > (MAX_HEAPLENGHT-1 ) ){
        printf("Invalid passing positive size parameter\n");
        return INVALID ;
    }

    /*update heap break*/
    temp = CurBreak ;
    CurBreak += size;   
    return temp ;
}




sint32 HeapUtils_sbrkResize(sint32 size,sint8 flag){
    /*
    * - Ret_DataIndex: used as the return value of target index that used to point to data 
    * - New_Tail: express what is the new created node.
    */
    sint32 Ret_DataIndex = INVALID ;  
    sint32 New = INVALID ;

    while (Ret_DataIndex == INVALID){
        New = HeapUtils_sbrk(BREAK_STEP_SIZE);

        /* if there is no space */
        if (New == INVALID){
            printf("Invalid state from Helper_sbrk function\n");
            exit(INVALID);
        }

        /* 
        * Head and Tail point to the same index before break pointer and will store 
        * the remaining size issue #46
        * */
        if (flag == STATE1){   
            /* if you exist from while because that there is no suitable free space
            * that : 1.  define new tail index point to new free space and call Helper_sbrk to extend break index 
            *        2. new tail -> previous free space point to old tail -> previous free space
            *        3. new tail -> next free space point to !
            *        4. remove old tail node 
            */
            SimHeap[Tail+NEXT_FREE_BLOCK_SHIFT]= New;
            HeapUtils_SetFreeNodeInfo(New,BREAK_STEP_SIZE, Tail,SYMBOL_OF_HEAP_NULL);
            Tail = New ;
        }
        else if (flag == STATE2){
            /*
            * Extend current break pointer and resize heap
            * still head and tail point to the same node but with new size
            * */
            SimHeap[Tail] += BREAK_STEP_SIZE;
        }
        flag = STATE2 ;
        Ret_DataIndex = HeapUtils_AllocationCoreLoop(Tail, size);
    }

    return Ret_DataIndex ;
}




sint32 HeapUtils_SplitFreeBlock (sint32 index, sint32 size){
    sint32 Ret_DataIndex ; 

    /* head and tail point to the same node */
    sint8 HeadAndTail = INVALID ;
    if (Head == Tail){
        HeadAndTail = VALID ;
    } 

    /* handle simulated array if head is suitable to allocate this size so you will cut section from head
    *  that: 1. previous free space pointer will redefine in array and store ! in this cell
    *        2. next free space pointer will redefine in array but still point to the same cell
    *        3. metadata will redefine in array and store remaining size of free space
    */
   if (index == Head){
        /* when don't we need this local variable  `NextIndex`?
        * - in shifting head and tail to point the same index in first calling collection of calls to malloc
        * without free statments. we don't need it because we don't need to define the previous free index in 
        * next node
        * 
        * - we need this local variable if there is a free node after head to make head_next_node-> previous
        * point to the new location of head.  
        * */
        sint32 NextIndex = SimHeap[Head+NEXT_FREE_BLOCK_SHIFT];
        
        /* to shift tail with first group calls for maollac*/
        if (HeadAndTail == INVALID){
            Head = index + size + BEGIN_DATA_SHIFTER ;
            HeapUtils_SetFreeNodeInfo(Head, SimHeap[index] - size - METADATA_CELL, SYMBOL_OF_HEAP_NULL,NextIndex);
            SimHeap[NextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = Head ;
        }
        else if (HeadAndTail == VALID){
            Head = index + size + BEGIN_DATA_SHIFTER ; 
            Tail = Head ;
            HeapUtils_SetFreeNodeInfo(Head, SimHeap[index] - size - METADATA_CELL, SYMBOL_OF_HEAP_NULL,SYMBOL_OF_HEAP_NULL);
        }
   }
    /* handle simulated array if tail is suitable to allocate this size
    *  that: 1. previous free space pointer will redefine in array but still point to the same cell
    *        2. next free space pointer will redefine in array and store ! in this cell
    *        3. metadata will redefine in array and store remaining size of free space
    */
   else if (index == Tail){
        sint32 PreIndex = SimHeap[index+PREVIOUS_FREE_BLOCK_SHIFT];
        Tail = index + size + BEGIN_DATA_SHIFTER ;
        HeapUtils_SetFreeNodeInfo(Tail, SimHeap[index] - size - METADATA_CELL,PreIndex,SYMBOL_OF_HEAP_NULL); 
        SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = Tail ;
   }
    /* handle simulated array if neither head or tail is suitable to allocate this size
    *  that: 1. previous free space pointer will redefine in array but still point to the same cell
    *        2. next free space pointer will redefine in array but still point to the same cell
    *        3. metadata will redefine in array and store remaining size of free space
    *        4. update previous index ->next and Next index -> Pre
    */
   else {
        sint32 New = index + size + BEGIN_DATA_SHIFTER ;
        sint32 PreIndex = SimHeap[index+PREVIOUS_FREE_BLOCK_SHIFT];
        sint32 NextIndex = SimHeap[index+NEXT_FREE_BLOCK_SHIFT];
        HeapUtils_SetFreeNodeInfo(New, SimHeap[index] - size - METADATA_CELL, PreIndex,NextIndex);  
        SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = New ;
        SimHeap[NextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = New ;
   }
    /* handle new allocation space
    *  that: 1. return value point to the beginning of data space
    *        2. edit metadata of new allocation to new allocation available space
    */
    Ret_DataIndex = index  + BEGIN_DATA_SHIFTER;
    SimHeap[index] = size ;

    return Ret_DataIndex ;
}



sint32 HeapUtils_RemoveFreeBlock (sint32 index, sint32 size){
    sint32 Ret_DataIndex ; 
    
    /* head and tail point to the same node */
    sint8 HeadAndTail = INVALID ;
    if (Head == Tail){
        HeadAndTail = VALID ;
    } 

    /* handle simulated array if head is suitable to allocate this size and will take whole space
    *  that  1. shift head node to next node
    *        2. make next node -> previous free space equals !
    */
   if (index == Head){
        /* 
        * Head and Tail point to the same index before break pointer and will store 
        * the remaining size issue #46
        * */
        /* to shift tail with first group calls for maollac*/
        if (HeadAndTail == INVALID){
            Head = SimHeap[index+NEXT_FREE_BLOCK_SHIFT] ;
            SimHeap[Head+PREVIOUS_FREE_BLOCK_SHIFT] = SYMBOL_OF_HEAP_NULL ;
        }
        else if (HeadAndTail == VALID ){
            sint32 New = HeapUtils_sbrk(BREAK_STEP_SIZE); 
            if (New == INVALID){
                printf("Invalid state from Helper_sbrk function\n");
                exit(INVALID);
            }

            HeapUtils_SetFreeNodeInfo(New,BREAK_STEP_SIZE,SYMBOL_OF_HEAP_NULL,SYMBOL_OF_HEAP_NULL);
            Tail = New ;
            Head = New ;
        }
   }
    /* handle simulated array if tail is suitable to allocate this size
    *  that: 1. backword tail index to previous free slot 
    *        2. backword tail -> next free space point to !
    * */
   else if (index == Tail){
        sint32 Backwork_Tail = SimHeap[index+PREVIOUS_FREE_BLOCK_SHIFT] ; 
        Tail = Backwork_Tail ;
        SimHeap[Tail+NEXT_FREE_BLOCK_SHIFT] = SYMBOL_OF_HEAP_NULL ;
   }
    /* handle simulated array if neither head or tail is suitable to allocate this size
    *  that: 1. make next node -> previous free space index point to temp -> previous free space
    *        2. make previous node -> next free space index point to temp -> next free space
    *        3. remove temp node 
    */
   else {
        sint32 PreTemp = SimHeap[index+PREVIOUS_FREE_BLOCK_SHIFT] ;
        sint32 NextTemp = SimHeap[index+NEXT_FREE_BLOCK_SHIFT] ;
        SimHeap[NextTemp+PREVIOUS_FREE_BLOCK_SHIFT] = PreTemp ; 
        SimHeap[PreTemp+NEXT_FREE_BLOCK_SHIFT] = NextTemp ; 
    }

    SimHeap[index] = size;
    Ret_DataIndex = index  + BEGIN_DATA_SHIFTER;

    return Ret_DataIndex ;
}



void   HeapUtils_SetFreeNodeInfo(sint32 index, sint32 metadata, sint32 previous_content, sint32 next_content){
    SimHeap[index+PREVIOUS_FREE_BLOCK_SHIFT] = previous_content ; 
    SimHeap[index+NEXT_FREE_BLOCK_SHIFT] = next_content ;
    SimHeap[index] =  metadata;
}
