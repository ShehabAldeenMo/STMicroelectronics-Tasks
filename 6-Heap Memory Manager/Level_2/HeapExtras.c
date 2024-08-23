/*============================================================================
 * @file name      : HeapExtras.c
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This file contains functions for managing heap memory, including allocating
 * memory using first-fit and best-fit strategies, and managing free memory blocks.
 * It provides operations for handling free memory blocks before and after the
 * head and tail of the free list, as well as merging adjacent free blocks.
 *
=============================================================================
 * @Notes:
 * - Functions in this file assume a specific heap layout where free blocks are
 *   managed using a free list with head and tail pointers.
 * - The functions handle scenarios such as adjacent blocks and resizing the heap
 *   when necessary.
 * - Functions are designed to work with a simulated heap array where metadata and
 *   pointers are embedded in the heap array itself.
 *
 ******************************************************************************
 ==============================================================================
*/


/*===================================  Includes ==============================*/
#include "HeapExtras.h"



/*============================  extern Global Variable ==============================*/
extern sint32 SimHeap[MAX_HEAPLENGHT];          // simulated heap
extern sint32 CurBreak;                         // break pointer on simulated heap
extern sint32 Head;                             // define first index of free space
extern sint32 Tail;                             // define last index of free spaces



/*=========================  Functions Implementation ===========================*/
sint32 HeapExtras_FirstFit(sint32 size){
    /*
    * - TailSbrkState: express about `if head and tail point to the same index and just before break pointer in your heap`
    * - Ret_DataIndex: used as the return value of target index that used to point to data 
    * - i: iterate in while loop
    * - size: resize the block with the new size. Because we are use three blocks for represent free node.
    */
    sint8 TailSbrkState = STATE1 ; 
    sint32 Ret_DataIndex = INVALID ;  
    sint32 i = Head ;  

    /* if head and tail are point to the same index before break index in heap
    *  should be ensured that there is no allocation memory between them (illustrated in issue #46)
    * - state 1 : tail point to node but not just before break pointer
    * - state 2 : tail point to node and just before break pointer
    */
    sint32 AdjecentNode = Tail + SimHeap[Tail]  ;
    if (AdjecentNode == CurBreak){
        TailSbrkState = STATE2 ; // used to call Helper_sbrk if it is needed.
    } // rather that will be continue on state 1

    /*
    * iterate to reserve some memory for required pointer
    * */
    while( i != (sint32) SYMBOL_OF_HEAP_NULL){ // while its next node is not jump over break
        Ret_DataIndex = HeapUtils_AllocationCoreLoop(i,size);

        if (Ret_DataIndex != INVALID){
            break ;
        }
        else {
            /* make next index point to the next node
            */
            i = SimHeap[i+NEXT_FREE_BLOCK_SHIFT] ;
        }
    }

    /* if you exist from while because that there is no suitable free space 
    * so we will move sbrk pointer and update our free list
    */
    if ( i == (sint32) SYMBOL_OF_HEAP_NULL ){
        Ret_DataIndex = HeapUtils_sbrkResize(size, TailSbrkState);
    } // rather that will be continue on state 1


    /* return index of allocation new space -> data */
    return Ret_DataIndex ;
}


void   HeapExtras_FreeOperationBeforeHead(sint32 index, sint32 size){
#if DEBUGGING == ENABLE
    printf("Free Before Head\n");
    printf("| Index | Block Size |\n");
    printf("----------------------------------------------------\n");
    printf("| %5d | %10d |\n", 
            index, SimHeap[index]);
#endif
    sint32 PositionOfFreeBlock = index + size + METADATA_CELL ; // to check that the target free block is adjecent for head or away from.
     /* if ptr is pointing to node before head node and away from it
     *  that : 1- define new node 
     *         2- make this node -> previous free space = !  
     *         3- this node -> next free space = old head
     *         4- old head -> previous free space = index
     *         5- update head to point to index
     * */
    if (PositionOfFreeBlock < Head ){
        HeapUtils_SetFreeNodeInfo(index,size,SYMBOL_OF_HEAP_NULL,Head);
        SimHeap[Head+PREVIOUS_FREE_BLOCK_SHIFT] = index ; 
        Head = index ;
    }
     /* if ptr is pointing to node just before head node and adjecent for it
     *  that : 1- define new metdata for free space to be equal the margin of new free space and head space
     *         2- make this node -> previous free space = !  
     *         3- this node -> next free space = old head -> next free space
     *         4- update head to point to index
     * */
    else if (PositionOfFreeBlock == Head ){
        sint32 sizeOfhead = SimHeap[Head];
        sint32 nextIndex = SimHeap[Head+NEXT_FREE_BLOCK_SHIFT];
        HeapUtils_SetFreeNodeInfo(index, size+sizeOfhead+METADATA_CELL ,SYMBOL_OF_HEAP_NULL,nextIndex);
        SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = index ;
        Head = index ;
    }
    else {
        printf("error in freeing before head %5d with PositionOfFreeBlock = %5d\n",size,PositionOfFreeBlock);
        while(1);///////////////////////////// for testing
        exit(INVALID);
    }
}



void   HeapExtras_FreeOperationAfterTail(sint32 index,sint32 size){
#if DEBUGGING == ENABLE
    printf("Free After Tail\n");
    printf("| Index | Block Size |\n");
    printf("----------------------------------------------------\n");
    printf("| %5d | %10d |\n", 
            index, SimHeap[index]);
#endif
    sint32 PositionOfTailBlock = Tail + SimHeap[Tail] + METADATA_CELL ; // to check that the target free block is adjecent for tail or away from.
     /* if ptr is pointing to tail before node node is away from
     *  that : 1- define new node
     *         2- this node -> previous free space = tail  
     *         3- this node -> next free space = !
     *         4- old tail -> next free space = index
     *         5- update tail to point to index
     * */
    if (PositionOfTailBlock < index){
        HeapUtils_SetFreeNodeInfo(index,size,Tail,SYMBOL_OF_HEAP_NULL);
        SimHeap[Tail+NEXT_FREE_BLOCK_SHIFT] = index ; 
        Tail = index ;
    }
     /* if ptr is pointing to node just after tail node and adjecent for it
     *  redefine new metdata for free space to be equal the margin of new free space and tail space
     * */
    else if (PositionOfTailBlock == index){
         SimHeap[Tail] += ( size + METADATA_CELL );
    }
    else {
        printf("error in freeing after tail %5d with PositionOfTailBlock = %5d\n",size,PositionOfTailBlock);
        while(1);///////////////////////////// for testing
        exit(INVALID);
    }
}


void   HeapExtras_FreeOperationMiddleNode(sint32 index,sint32 size){
#if DEBUGGING == ENABLE
    printf("Free Middle Node\n");
    printf("| Index | Block Size |\n");
    printf("----------------------------------------------------\n");
    printf("| %5d | %10d |\n", 
            index, SimHeap[index]);
#endif
    sint32 nextIndex = Head ; // to store the next free slot after index
    sint32 PreIndex = INVALID ;     // to store the previous free slot before index

    // till reach to the strict next free slot after index  
    while (nextIndex < index){
        nextIndex = SimHeap[nextIndex+NEXT_FREE_BLOCK_SHIFT];
    }
    PreIndex = SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT]; // to store the previous free slot before index
    sint32 SizeOfPrev = SimHeap[PreIndex] ;
    sint32 SizeOfNext = SimHeap[nextIndex] ;

    /*
    * define free block position for :
    * 1) index point to node just after free node(x) from right so we will extend x node to join two adjecent free spaces.
    * 2) index point to node just before free node(y) from left so we will extend y node to join two adjecent free spaces
    */
    sint32 PositionOfPreviousBlock = PreIndex + SizeOfPrev + METADATA_CELL ; // deallcated block in right
    sint32 PositionOfNextBlock = index + size + METADATA_CELL ; // deallcated block in left

    // index point to node just after free node(x) from right so we will extend x node to join two adjecent free spaces.
    if (PositionOfPreviousBlock == index && PositionOfNextBlock < nextIndex){
#if DEBUGGING == ENABLE
        printf("\n Free from right \n");
#endif
        sint32 NewSize = SizeOfPrev+size+METADATA_CELL ;
        /*
        * operation will be :
        * 1- redefine new size for previous node because we extend it .
        * 2- previous node previousIndex and nextIndex stay the same 
        * */
        SimHeap[PreIndex] = NewSize ;
        SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = nextIndex;
        SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = PreIndex ;
    }
    // index point to node just before free node(y) from left so we will extend y node to join two adjecent free spaces
    else if (PositionOfPreviousBlock < index && PositionOfNextBlock == nextIndex){
#if DEBUGGING == ENABLE
        printf("\n Free from left \n");
#endif
        sint32 NewSize = SizeOfNext+size+METADATA_CELL ;
        sint32 NextNextNodeIndex = SimHeap[nextIndex+NEXT_FREE_BLOCK_SHIFT];
        /*
        * operation will be :
        * 1- define new node merge between deallocation and next node
        * 2- new node size will equal deallocation memory and next node metadata.
        * 3- new node previousIndex and nextIndex stay the same of old node
        * 4- update previous index -> next free node
        * 5- update next index -> previous free node  
        * */
        HeapUtils_SetFreeNodeInfo(index,NewSize,PreIndex,NextNextNodeIndex);
        SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = index ;

        /* update Tail to point on new update node */
        if (nextIndex == Tail){
#if DEBUGGING == ENABLE
            printf("\n Free from 2 Tail \n");
#endif
            Tail = index ;
        }
        else{
#if DEBUGGING == ENABLE
            printf("\n Free from 1 no Tail \n");
#endif
            SimHeap[NextNextNodeIndex+PREVIOUS_FREE_BLOCK_SHIFT] = index ;
        }
    }
    // indexOfptr point to node just after free node(x) and before free node(y) from right so we will extend x,y node to join three adjecent free spaces    
    else if (PositionOfPreviousBlock == index && PositionOfNextBlock == nextIndex){
#if DEBUGGING == ENABLE
        printf("\n Free from between two free slots \n");
#endif
        sint32 NewSize = SizeOfPrev+size+SizeOfNext+2*METADATA_CELL ;
        sint32 NextNextIndex = SimHeap[nextIndex+NEXT_FREE_BLOCK_SHIFT];

        /* update Tail to point on new update node */
        if (nextIndex == Tail){
#if DEBUGGING == ENABLE
            printf("\n Free from 3 Tail \n");
#endif
            HeapUtils_SetFreeNodeInfo(PreIndex,NewSize,SimHeap[PreIndex+PREVIOUS_FREE_BLOCK_SHIFT],SYMBOL_OF_HEAP_NULL);
            Tail = PreIndex ;
        }
        else{
#if DEBUGGING == ENABLE
            printf("\n Free from 3 no Tail \n");
#endif
            HeapUtils_SetFreeNodeInfo(PreIndex,NewSize,SimHeap[PreIndex+PREVIOUS_FREE_BLOCK_SHIFT],NextNextIndex);
            SimHeap[NextNextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = PreIndex ;
        }

    }
    // indexOfptr point to node away not adjecent for any free node so we willn't extend and free node
    else if (PositionOfPreviousBlock < index && PositionOfNextBlock < nextIndex){
        /*
        * operation will be :
        * 1- define new node 
        * 2- size for new node will be the same of deallocated memory.
        * 3- new node -> previous index will equal next node -> previous index
        * 4- new node -> next index will equal Previous node -> next index
        * 5- prevoius node -> next index = index
        * 6- next node -> pre index = index
        * */
#if DEBUGGING == ENABLE
       printf("\n Free between two reserved slots \n");
#endif
       sint32 PreviousIndexContent = SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT] ;
       sint32 NextIndexContent = SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] ;
       HeapUtils_SetFreeNodeInfo(index,size,PreviousIndexContent,NextIndexContent);
       SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = index ;
       SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = index ;
    }
    else {
        printf("error in freeing middle node %5d with PositionOfPreviousBlock = %5d and PositionOfNextBlock = %5d\n",size,PositionOfPreviousBlock,PositionOfNextBlock);
        while(1);///////////////////////////// for testing
        exit(INVALID);
    }
}


sint32 HeapExtras_BestFit(sint32 size){
    /*
    * - TailSbrkState: express about `if head and tail point to the same index and just before break pointer in your heap`
    * - Ret_SuitableIndex: used as the return value of target index that used to point to data 
    * - i: iterate in while loop
    * - size: resize the block with the new size. Because we are use three blocks for represent free node.
    * - FlagOfNoFreeSpace : there is no free space suitable to allocate this data
    */
    sint8 TailSbrkState = STATE1 ; 
    sint32 Ret_SuitableIndex = INVALID;  
    sint32 i = Head ;    // index to indicate which point that you stop on 
    sint32 SuitableIndex = INVALID ;
    sint8  FlagOfNoFreeSpace = INVALID ;

    /*
    * search on suitable index to allocate memory in first
    */
    while(i != (sint32) SYMBOL_OF_HEAP_NULL ){
        if (SimHeap[i] >= size){
            SuitableIndex =  i ;
            FlagOfNoFreeSpace = VALID ;
            i = SimHeap[i+NEXT_FREE_BLOCK_SHIFT];
            break;
        } // else while be contiune
        i = SimHeap[i+NEXT_FREE_BLOCK_SHIFT];
    }

    /*
    * search on suitable index to allocate memory
    */
    while(i != (sint32) SYMBOL_OF_HEAP_NULL && FlagOfNoFreeSpace == VALID){
        if (SimHeap[i] < SimHeap[SuitableIndex] && SimHeap[i] >= size){
            SuitableIndex =  i ;
        } // else while be contiune
        i = SimHeap[i+NEXT_FREE_BLOCK_SHIFT];
    }

    /* if head and tail are point to the same index before break index in heap
    *  should be ensured that there is no allocation memory between them (illustrated in issue #46)
    * - state 1 : tail point to node but not just before break pointer
    * - state 2 : tail point to node and just before break pointer
    */
    sint32 AdjecentNode = Tail + SimHeap[Tail]  ;
    if (AdjecentNode == CurBreak){
        TailSbrkState = STATE2 ; // used to call Helper_sbrk if it is needed.
    } // else while be contiune on state 1

    /* if you exist from while because that there is no suitable free space 
    * so we will move sbrk pointer and update our free list
    */
    if ( FlagOfNoFreeSpace == INVALID ){
        Ret_SuitableIndex = HeapUtils_sbrkResize(size, TailSbrkState);
    }
    else {
        Ret_SuitableIndex = HeapUtils_AllocationCoreLoop(SuitableIndex,size);
        Ret_SuitableIndex += BEGIN_DATA_SHIFTER;
    }
    
    SimHeap[SuitableIndex] = size ;

    /* return index of allocation new space -> data */
    return Ret_SuitableIndex ;   
}

void HeapExtras_Init(){
    Head = 0 ;
    Tail = 0 ;
    CurBreak = BREAK_STEP_SIZE ;
    SimHeap[Head] = BREAK_STEP_SIZE-1 ;
    SimHeap[Head+PREVIOUS_FREE_BLOCK_SHIFT] = SYMBOL_OF_HEAP_NULL ; 
    SimHeap[Head+NEXT_FREE_BLOCK_SHIFT] = SYMBOL_OF_HEAP_NULL ; 
} 

