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
#include <unistd.h>


/*============================  extern Global Variable ==============================*/
extern FreeBlock* ptrHead;
extern FreeBlock* ptrTail;
extern sint8*     CurBreak;                         // break pointer on simulated heap

static sint8      TailBrkState = STATE1 ; 

/*=========================  Functions Implementation ===========================*/
sint8* HeapExtras_FirstFit(size_t size){
    static sint8 sleep_flag = 0 ;
    if (sleep_flag == 0){
      	sleep(20);
       sleep_flag = 1 ;
    }
    /*
    * TailBrkState: State variable to track the relation between the tail and the program break pointer.
    * RetDataPtr: Pointer to the location where memory will be allocated.
    * CurBlock: Pointer to the current block in the free list being examined.
    * size: Requested size for memory allocation.
    */
    sint8*            RetDataPtr   = NULL ;  
    FreeBlock*        CurBlock     = ptrHead ; 

    // to ensure when free this pointer the new node will not overwrite on the next node
    if (size < sizeof(FreeBlock) ){
        size = sizeof(FreeBlock);
    }

    // to align data on 8
    size = ((size + 7 ) / 8) * 8;

    TailBreakStatus();

    /*
    * Iterate through the free list to find a suitable block for the required size.
    * */
    while( CurBlock != NULL){ // while its next node is not jump over break
        RetDataPtr = HeapUtils_AllocationCoreLoop(CurBlock,size);

        if (RetDataPtr == NULL){
            /* make next index point to the next node
            */
            CurBlock = CurBlock->NextFreeBlock ;
        }
        else {
            break;
        }
    }

    /* If no suitable free space was found, adjust the program break pointer and update the free list.
    * */
    if ( CurBlock == NULL){
        RetDataPtr = HeapUtils_sbrkResize(size, TailBrkState);
    } // rather that will be continue on state 1

    Shrink_Break(TailBrkState);

    /* return index of allocation new space -> data */
    return RetDataPtr ;
}


void HeapExtras_Init() {
    // Use sbrk to allocate the initial block of memory from the system heap
    void* heap_start = sbrk(BREAK_STEP_SIZE);
    
    if (heap_start == (void*)-1) {
        // Handle sbrk failure
        perror("sbrk failed");
        exit(EXIT_FAILURE);
    }

    // Create the initial free block in the simulated heap
    FreeBlock* initialBlock = (FreeBlock*)heap_start;
    
    // Set the block size to the total size of the simulated heap
    initialBlock->BlockSize = ( BREAK_STEP_SIZE - sizeof(size_t) ); // - sizeof(size_t) because the first size_t use for metadata representation

    // No other free blocks, so Next and Previous pointers are NULL
    initialBlock->NextFreeBlock = NULL;
    initialBlock->PreviousFreeBlock = NULL;

    // Set the head and tail pointers to the initial block
    ptrHead = initialBlock;
    ptrTail = initialBlock;

    // Set the current break pointer to the end of the allocated block
    CurBreak = (sint8*)sbrk(0);  // Get the current program break
}


void   HeapExtras_FreeOperationBeforeHead(FreeBlock* Node){
#if DEBUGGING == ENABLE
    printf("Free before head and node Size = %5ld\n",Node->BlockSize);
    getchar();
#endif

    sint8* PositionOfFreeBlock = (sint8*)Node + sizeof(size_t) + Node->BlockSize ; // to check that the target free block is adjecent for head or away from.

    /* head and tail point to the same node */
    sint8 HeadAndTail = INVALID ;
    if (ptrHead == ptrTail){
        HeadAndTail = VALID ;
    } 

    /*
     * Case 1: The block is before the head and not adjacent.
     *         - Define the new node.
     *         - Set the new node's previous free space to NULL.
     *         - Set the new node's next free space to the old head.
     *         - Update the old head's previous free space to the new node.
     *         - Update the head to point to the new node.
     */
    if (PositionOfFreeBlock < (sint8*)ptrHead ){
        HeapUtils_SetFreeNodeInfo(Node,Node->BlockSize,NULL,ptrHead);
        ptrHead->PreviousFreeBlock = Node ;
        ptrHead = Node ;
    }
    /*
     * Case 2: The block is just before the head and adjacent to it.
     *         - Merge the new block with the head.
     *         - Define new metadata for the merged free space.
     *         - Set the new head's previous free space to NULL.
     *         - Set the new head's next free space to the old head's next free space.
     */
    else if (PositionOfFreeBlock == (sint8*)ptrHead ){
        size_t sizeOfhead = ptrHead->BlockSize;
        if (HeadAndTail == INVALID){
            FreeBlock* nextNode = ptrHead->NextFreeBlock;
            ptrHead = Node ;
            HeapUtils_SetFreeNodeInfo(ptrHead, Node->BlockSize+sizeOfhead+(size_t)sizeof(size_t),NULL,nextNode);
            nextNode->PreviousFreeBlock = ptrHead ;
        }
        else {
            ptrHead = Node ;
            ptrTail = ptrHead ;
            HeapUtils_SetFreeNodeInfo(ptrHead, Node->BlockSize+sizeOfhead+(size_t)sizeof(size_t),NULL,NULL);
        }
    }
    /*
     * Error Handling: If the block is not before the head or adjacent to it, 
     *                 print an error message and halt execution.
     */
    else {
#if DEBUGGING == ENABLE
        printf("error in freeing before head, with PositionOfFreeBlock = %p\n",PositionOfFreeBlock);
#endif
        while(1);///////////////////////////// for testing
        exit(INVALID);
    }
}



void   HeapExtras_FreeOperationAfterTail(FreeBlock* Node){
#if DEBUGGING == ENABLE
    printf("Free after tail and node Size = %5ld\n",Node->BlockSize);
    getchar();
#endif
    sint8* PositionOfTailBlock = (sint8*)ptrTail + ptrTail->BlockSize + sizeof(size_t) ; // to check that the target free block is adjecent for tail or away from.

    /* if ptr is pointing to tail before node node is away from
    *  that : 1- define new node
    *         2- this node -> previous free space = tail  
    *         3- this node -> next free space = !
    *         4- old tail -> next free space = index
    *         5- update tail to point to index
    * */
    if (PositionOfTailBlock < (sint8*)Node ){
        HeapUtils_SetFreeNodeInfo(Node,Node->BlockSize,ptrTail,NULL);
        ptrTail->NextFreeBlock = Node;
        ptrTail = Node ;
    }
     /* if ptr is pointing to node just after tail node and adjecent for it
     *  redefine new metdata for free space to be equal the margin of new free space and tail space
     * */
    else if (PositionOfTailBlock == (sint8*)Node){
        size_t sizeOftail = ptrTail->BlockSize;
        ptrTail->BlockSize = sizeOftail+(size_t)sizeof(size_t)+Node->BlockSize;
    }
    else {
#if DEBUGGING == ENABLE
        printf("error in freeing after tail, with PositionOfTailBlock = %p\n",PositionOfTailBlock);
#endif
        while(1);///////////////////////////// for testing
        exit(INVALID);
    }

    TailBreakStatus();
    Shrink_Break(TailBrkState);
}


void   HeapExtras_FreeOperationMiddleNode(FreeBlock* Node){
#if DEBUGGING == ENABLE
    printf("Free in middle from free list and node Size = %5ld\n",Node->BlockSize);
    getchar();
#endif
    FreeBlock* nextNode = ptrHead ; // to store the next free slot after index
    FreeBlock* PreNode = NULL ;     // to store the previous free slot before index
    size_t size = Node->BlockSize ;

    // till reach to the strict next free slot after index  
    while (nextNode < Node){
        nextNode = nextNode->NextFreeBlock;
    }
    PreNode = nextNode->PreviousFreeBlock; // to store the previous free slot before index
    
    FreeBlock* PrevPrevNode = PreNode->PreviousFreeBlock;
    FreeBlock* NextNextNode = nextNode->NextFreeBlock;

    size_t SizeOfPrev = PreNode->BlockSize ;
    size_t SizeOfNext = nextNode->BlockSize ;

    /*
    * define free block position for :
    * 1) index point to node just after free node(x) from right so we will extend x node to join two adjecent free spaces.
    * 2) index point to node just before free node(y) from left so we will extend y node to join two adjecent free spaces
    */
    sint8* PositionOfPreviousBlock = (sint8*)PreNode + SizeOfPrev + sizeof(size_t) ; // deallcated block in right
    sint8* PositionOfNextBlock = (sint8*)Node + Node->BlockSize + sizeof(size_t) ; // deallcated block in left

    // index point to node just after free node(x) from right so we will extend x node to join two adjecent free spaces.
    if (PositionOfPreviousBlock == (sint8*)Node && PositionOfNextBlock < (sint8*)nextNode){
#if DEBUGGING == ENABLE
        printf("\n Free from right \n");
#endif
        sint32 NewSize = SizeOfPrev+size+sizeof(size_t) ;
        /*
        * operation will be :
        * 1- redefine new size for previous node because we extend it .
        * 2- previous node previousIndex and nextIndex stay the same 
        * */
       HeapUtils_SetFreeNodeInfo(PreNode,NewSize,PrevPrevNode,nextNode);
       HeapUtils_SetFreeNodeInfo(nextNode,SizeOfNext,PreNode,NextNextNode);
    }
    // index point to node just before free node(y) from left so we will extend y node to join two adjecent free spaces
    else if (PositionOfPreviousBlock < (sint8*)Node && PositionOfNextBlock == (sint8*)nextNode){
#if DEBUGGING == ENABLE
        printf("\n Free from left \n");
#endif
        sint32 NewSize = SizeOfNext+size+sizeof(size_t) ;
        /*
        * operation will be :
        * 1- define new node merge between deallocation and next node
        * 2- new node size will equal deallocation memory and next node metadata.
        * 3- new node previousIndex and nextIndex stay the same of old node
        * 4- update previous index -> next free node
        * 5- update next index -> previous free node  
        * */
        HeapUtils_SetFreeNodeInfo(Node,NewSize,PreNode,NextNextNode);
        HeapUtils_SetFreeNodeInfo(PreNode,SizeOfPrev,PrevPrevNode,Node);

        /* update Tail to point on new update node */
        if (nextNode == ptrTail){
#if DEBUGGING == ENABLE
            printf("\n Free from left with Tail \n");
#endif
            ptrTail = Node ;
        }
        else{
#if DEBUGGING == ENABLE
            printf("\n Free from left without Tail \n");
#endif
            HeapUtils_SetFreeNodeInfo(NextNextNode,NextNextNode->BlockSize,Node,NextNextNode->NextFreeBlock);
        }
    }
    // indexOfptr point to node just after free node(x) and before free node(y) from right so we will extend x,y node to join three adjecent free spaces    
    else if (PositionOfPreviousBlock == (sint8*)Node && PositionOfNextBlock == (sint8*)nextNode){
#if DEBUGGING == ENABLE
        printf("\n Free between two free slots \n");
#endif
        sint32 NewSize = SizeOfPrev+size+SizeOfNext+2*sizeof(size_t) ;
        HeapUtils_SetFreeNodeInfo(PreNode,NewSize,PrevPrevNode,NextNextNode);

        /* update Tail to point on new update node */
        if (nextNode == ptrTail){
#if DEBUGGING == ENABLE
            printf("\n Free between two free slots with Tail \n");
#endif
            ptrTail = PreNode ;
        }
        else{
#if DEBUGGING == ENABLE
            printf("\n Free between two free slots without Tail \n");
#endif
            HeapUtils_SetFreeNodeInfo(NextNextNode,NextNextNode->BlockSize,PreNode,NextNextNode->NextFreeBlock);
        }

    }
    // indexOfptr point to node away not adjecent for any free node so we willn't extend and free node
    else if (PositionOfPreviousBlock < (sint8*)Node && PositionOfNextBlock < (sint8*)nextNode){
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
       HeapUtils_SetFreeNodeInfo(Node,size,PreNode,nextNode);
       HeapUtils_SetFreeNodeInfo(PreNode,SizeOfPrev,PrevPrevNode,Node);
       HeapUtils_SetFreeNodeInfo(nextNode,SizeOfNext,Node,NextNextNode);
    }
    else {
#if DEBUGGING == ENABLE
        printf("error in freeing middle node, with PositionOfPreviousBlock = %p and PositionOfNextBlock = %p\n",PositionOfPreviousBlock,PositionOfNextBlock);
#endif
        while(1);///////////////////////////// for testing
        exit(INVALID);
    }
}

void TailBreakStatus(){
    /* Check if the head and tail are pointing to the same index before the break pointer in the heap.
    *  This ensures that there is no allocated memory between them (related to issue #46).
    * - STATE1: Tail points to a node but not directly before the break pointer.
    * - STATE2: Tail points to a node and is directly before the break pointer.
    */
    sint8* AdjecentNode = (sint8*)ptrTail + sizeof(size_t) + ptrTail->BlockSize ;
    if (AdjecentNode == CurBreak){
        TailBrkState = STATE2 ; // used to call Helper_sbrk if it is needed.
    } // rather that will be continue on state 1
    else {
#if DEBUGGING == ENABLE
        printf("AdjecentNode = %p and CurBreak = %p",AdjecentNode,CurBreak);
        getchar();
#endif
        TailBrkState = STATE1 ;
    }
}
