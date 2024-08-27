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
extern sint64 SimHeap[MAX_HEAPLENGHT];          // simulated heap
extern FreeBlock* ptrHead;
extern FreeBlock* ptrTail;
extern sint8* CurBreak;                         // break pointer on simulated heap
 
/*=========================  Functions Implementation ===========================*/
sint8* HeapUtils_AllocationCoreLoop(FreeBlock* ptrBlock,size_t ReqSize){
    sint8* RetDataPtr         = NULL ;  
    size_t sizeOfFreeSpace    = ptrBlock->BlockSize ;

    // Handle padding cases
    size_t padding            = sizeOfFreeSpace - ReqSize ;
    if ( padding < (size_t) sizeof(FreeBlock)) {
        ReqSize = sizeOfFreeSpace;
    }
    
    /* memory allocation action */
    if (sizeOfFreeSpace > ReqSize){
        RetDataPtr = HeapUtils_SplitFreeBlock (ptrBlock,ReqSize);
    }
    else if (sizeOfFreeSpace == ReqSize){
        RetDataPtr = HeapUtils_RemoveFreeBlock (ptrBlock,ReqSize);
    } // else return INVALID as the ibex is not suitable to reserve data

    return RetDataPtr ; 
}


sint8* HeapUtils_sbrk (size_t size){
    sint8* result = CurBreak + size ; // to use it in check size condition
    sint8* temp = NULL;

    /*to ensure that the new current break in heap limitions*/
    if ( result < (sint8*)SimHeap ){
        printf("Invalid passing negative size parameter\n");
        return temp ;
    }
    else if (result > ((sint8*) &SimHeap[MAX_HEAPLENGHT]) ){
        printf("Invalid passing positive size parameter\n");
        return temp ;
    } // else continue work to extend pointer of break counter

    /*update heap break*/
    temp = CurBreak ;
    CurBreak += size ;   
    return temp ;
}


sint8* HeapUtils_sbrkResize(size_t ReqSize,sint8 flag){
    /*
    * - RetDataPtr: used as the return value of target index that used to point to data 
    * - New: express what is the new created node.
    */
    sint8*     RetDataPtr = NULL ;  
    FreeBlock* New        = NULL ;

    while (RetDataPtr == NULL){
        New = (FreeBlock*)HeapUtils_sbrk(BREAK_STEP_SIZE);

        /* if there is no space */
        if (New == NULL){
            printf("Invalid state from Helper_sbrk function\n");
            while(1);///////////////////////////// for testing
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
           ptrTail->NextFreeBlock = New ;
           New->PreviousFreeBlock = ptrTail ;
           ptrTail = New ;
        }
        else if (flag == STATE2){
            /*
            * Extend current break pointer and resize heap
            * still head and tail point to the same node but with new size
            * */
            ptrTail->BlockSize += BREAK_STEP_SIZE;
        } // else continue in loop
        flag = STATE2 ;
        RetDataPtr = HeapUtils_AllocationCoreLoop(ptrTail, ReqSize);
    }

    return RetDataPtr ;
}


sint8* HeapUtils_SplitFreeBlock (FreeBlock* Node, size_t spliting_size){
    sint8* RetDataPtr = NULL; 
    size_t SizeOfFreeSpace = Node->BlockSize;

    /* head and tail point to the same node */
    sint8 HeadAndTail = INVALID ;
    if (ptrHead == ptrTail){
        HeadAndTail = VALID ;
    } 

    sint8 InFreeList = HeapUtils_SearchOnIndexInFreeList(Node);
    /* handle simulated array if head is suitable to allocate this size so you will cut section from head
    *  that: 1. previous free space pointer will redefine in array and store ! in this cell
    *        2. next free space pointer will redefine in array but still point to the same cell
    *        3. metadata will redefine in array and store remaining size of free space
    */
   if (Node == ptrHead){
#if DEBUGGING == ENABLE
        printf("Free Size from Head = %5ld\nRequired Size from user = %5ld\n",Node->BlockSize,spliting_size);
        getchar();
#endif
        /* when don't we need this local variable  `NextIndex`?
        * - in shifting head and tail to point the same index in first calling collection of calls to malloc
        * without free statments. we don't need it because we don't need to define the previous free index in 
        * next node
        * 
        * - we need this local variable if there is a free node after head to make head_next_node-> previous
        * point to the new location of head.  
        * */
        FreeBlock* NextNode = Node->NextFreeBlock ;
        
        /* to shift tail with first group calls for maollac*/
        if (HeadAndTail == INVALID){
            ptrHead = (FreeBlock*) ((sint8*)ptrHead + spliting_size + sizeof(size_t));
            HeapUtils_SetFreeNodeInfo(ptrHead, (SizeOfFreeSpace-spliting_size-sizeof(size_t)), NULL,NextNode);
            NextNode->PreviousFreeBlock = ptrHead ;
        }
        else if (HeadAndTail == VALID){
            ptrHead = (FreeBlock*) ((sint8*)ptrHead + spliting_size + sizeof(size_t)) ;
            ptrTail = ptrHead ;
            HeapUtils_SetFreeNodeInfo(ptrHead,(SizeOfFreeSpace-spliting_size-sizeof(size_t)), NULL,NULL);
        }
   }
    /* handle simulated array if tail is suitable to allocate this size
    *  that: 1. previous free space pointer will redefine in array but still point to the same cell
    *        2. next free space pointer will redefine in array and store ! in this cell
    *        3. metadata will redefine in array and store remaining size of free space
    */
   else if (Node == ptrTail){
        FreeBlock* PreNode = Node->PreviousFreeBlock;
        ptrTail = (FreeBlock*) ((sint8*)Node + spliting_size + sizeof(size_t));
        HeapUtils_SetFreeNodeInfo(ptrTail,(SizeOfFreeSpace-spliting_size-sizeof(size_t)),PreNode,NULL); 
        PreNode->NextFreeBlock = ptrTail ;
   }
    /* handle simulated array if neither head or tail is suitable to allocate this size
    *  that: 1. previous free space pointer will redefine in array but still point to the same cell
    *        2. next free space pointer will redefine in array but still point to the same cell
    *        3. metadata will redefine in array and store remaining size of free space
    *        4. update previous index ->next and Next index -> Pre
    */
    else if (InFreeList == VALID){
        FreeBlock* NewNode = (FreeBlock*) ((sint8*)Node + spliting_size + sizeof(size_t));
        FreeBlock* PreNode = Node->PreviousFreeBlock ;
        FreeBlock* NextNode = Node->NextFreeBlock;
        HeapUtils_SetFreeNodeInfo(NewNode,(SizeOfFreeSpace-spliting_size-sizeof(size_t)), PreNode,NextNode);  
        PreNode->NextFreeBlock = NewNode ;
        NextNode->PreviousFreeBlock = NewNode ;
    }
    else {
        printf("allocate size with %5ld\n",spliting_size);
        printf("Not Exist In Free List, from split function with index size\n");
        while(1);///////////////////////////// for testing
        exit(INVALID);
    }

    /* handle new allocation space
    *  that: 1. return value point to the beginning of data space
    *        2. edit metadata of new allocation to new allocation available space
    */
    RetDataPtr = (sint8*)Node+sizeof(size_t);
    Node->BlockSize = spliting_size ;

    return RetDataPtr ;
}



sint8* HeapUtils_RemoveFreeBlock (FreeBlock* Node, size_t spliting_size){
    sint8* RetDataPtr = NULL; 
    
    /* head and tail point to the same node */
    sint8 HeadAndTail = INVALID ;
    if (ptrHead == ptrTail){
        HeadAndTail = VALID ;
    } 

    sint8 InFreeList = HeapUtils_SearchOnIndexInFreeList(Node);

    /* handle simulated array if head is suitable to allocate this size and will take whole space
    *  that  1. shift head node to next node
    *        2. make next node -> previous free space equals !
    */
   if (Node == ptrHead){
#if DEBUGGING == ENABLE
        printf("Free Size from Head = %5ld\nRequired Size from user = %5ld\n",Node->BlockSize,spliting_size);
        getchar();
#endif
        /* 
        * Head and Tail point to the same index before break pointer and will store 
        * the remaining size issue #46
        * */
        /* to shift tail with first group calls for maollac*/
        if (HeadAndTail == INVALID){
            ptrHead = ptrHead->NextFreeBlock ;
            ptrHead->PreviousFreeBlock = NULL ;
        }
        else if (HeadAndTail == VALID ){
            FreeBlock* New = (FreeBlock*)HeapUtils_sbrk(BREAK_STEP_SIZE); 
            if (New == NULL){
                printf("Invalid state from Helper_sbrk function\n");
                while(1);///////////////////////////// for testing
                exit(INVALID);
            }

            HeapUtils_SetFreeNodeInfo(New,BREAK_STEP_SIZE-sizeof(size_t),NULL,NULL);
            ptrTail = New ;
            ptrHead = New ;
        }
   }
    /* handle simulated array if tail is suitable to allocate this size
    *  that: 1. backword tail index to previous free slot 
    *        2. backword tail -> next free space point to !
    * */
   else if (Node == ptrTail){
        FreeBlock* Backwork_Tail = ptrTail->PreviousFreeBlock ; 
        ptrTail = Backwork_Tail ;
        ptrTail->NextFreeBlock = NULL ;
   }
    /* handle simulated array if neither head or tail is suitable to allocate this size
    *  that: 1. make next node -> previous free space index point to temp -> previous free space
    *        2. make previous node -> next free space index point to temp -> next free space
    *        3. remove temp node 
    */
   else if (InFreeList == VALID){
        FreeBlock* PreTemp = Node->PreviousFreeBlock ;
        FreeBlock* NextTemp = Node->NextFreeBlock;
        NextTemp->PreviousFreeBlock = PreTemp ; 
        PreTemp->NextFreeBlock = NextTemp ; 
    }
    else {
        printf("Not Exist In Free List index, from remove function with index size\n");
        while(1);///////////////////////////// for testing
        exit(INVALID);
    }

    /* handle new allocation space
    *  that: 1. return value point to the beginning of data space
    *        2. edit metadata of new allocation to new allocation available space
    */
    RetDataPtr = (sint8*)Node+sizeof(size_t);
    Node->BlockSize = spliting_size ;

    return RetDataPtr ;
}



sint8 HeapUtils_SearchOnIndexInFreeList(FreeBlock* block){
    FreeBlock* ptr = ptrHead ;
    while( ptr != NULL ){ // while its next node is not jump over break
        if (block == ptr ){
            return VALID ;
        }
        else {
            ptr = ptr->NextFreeBlock ;
        }
    }
    return INVALID ;
}



void   HeapUtils_SetFreeNodeInfo(FreeBlock* Node, size_t metadata, FreeBlock* previous_content, FreeBlock* next_content){
    Node->PreviousFreeBlock = previous_content ; 
    Node->NextFreeBlock = next_content ;
    Node->BlockSize =  metadata;
}

void Shrink_Break(sint8 flag){
    if (flag == STATE2){
        size_t Tail_Size = ptrTail->BlockSize ;

        while (Tail_Size > BREAK_STEP_SIZE){
            sint8* result = CurBreak - BREAK_STEP_SIZE ; // to use it in check size condition

            /*to ensure that the new current break in heap limitions*/
            if ( result < (sint8*)SimHeap ){
                printf("Invalid passing negative size parameter\n");
                return;
            }
            else if (result > ((sint8*) &SimHeap[MAX_HEAPLENGHT]) ){
                printf("Invalid passing positive size parameter\n");
                return ;
            } // else continue work to extend pointer of break counter

            /*update heap break*/
            CurBreak -= BREAK_STEP_SIZE ; 
            ptrTail->BlockSize -= BREAK_STEP_SIZE ; 
            Tail_Size = ptrTail->BlockSize ;
        }
    }
}