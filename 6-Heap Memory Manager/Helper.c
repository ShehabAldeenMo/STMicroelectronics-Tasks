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


#include "Helper.h"

/*============================  extern Global Variable ==============================*/
extern uint32 SimHeap[MAX_HEAPLENGHT];          // simulated heap
extern sint32 CurBreak;                         // break pointer on simulated heap
extern uint8* border;
extern uint32 Head;                             // define first index of free space
extern uint32 Tail;                             // define last index of free spaces


/*============================  Local Global Variable ==============================*/
static sint8 SetOfGroupMallocCalls = INVALID ; // to fix issue related to fixed tail at 0 althought we call malloc several times

/*=========================  Functions Implementation ===========================*/
sint32 Helper_sbrk (sint32 size){
    sint32 result = CurBreak+size ; // to use it in check size condition
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

sint32 Helper_FirstFit(uint32 size){
    sint32 Ret_SuitableIndex = INVALID ;  // to return which index is valid to alllocate into
    uint32 i = Head ;    // index to indicate which point that you stop on 
    while(i != (uint32) SYMBOL_OF_HEAP_NULL ){ // while its next node is not jump over break
        if (SimHeap[i] > size){
            /* handle simulated array if head is suitable to allocate this size
            *  that: 1. previous free space pointer will redefine in array and store ! in this cell
            *        2. next free space pointer will redefine in array but still point to the same cell
            *        3. metadata will redefine in array and store remaining size of free space
            */
           if (i == Head){
                uint32 NextIndex = SimHeap[i+NEXT_FREE_BLOCK_SHIFT];
                /* to shift tail with first group calls for maollac*/
                if (SetOfGroupMallocCalls == INVALID ){
                    Head = i + size + BEGIN_DATA_SHIFTER ;
                    Tail = Head ;
                    Helper_SetFreeSpaceNode(Head, SimHeap[i] - size - METADATA_CELL, SYMBOL_OF_HEAP_NULL,SYMBOL_OF_HEAP_NULL);
                }
                else {
                    Head = i + size + BEGIN_DATA_SHIFTER ;
                    Helper_SetFreeSpaceNode(Head, SimHeap[i] - size - METADATA_CELL, SYMBOL_OF_HEAP_NULL,SimHeap[i+NEXT_FREE_BLOCK_SHIFT]);
                    SimHeap[NextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = Head ;
                }
           }
            /* handle simulated array if tail is suitable to allocate this size
            *  that: 1. previous free space pointer will redefine in array but still point to the same cell
            *        2. next free space pointer will redefine in array and store ! in this cell
            *        3. metadata will redefine in array and store remaining size of free space
            */
           else if (i == Tail){
                uint32 PreIndex = SimHeap[i+PREVIOUS_FREE_BLOCK_SHIFT];
                SetOfGroupMallocCalls = VALID ; //to stop shifting tail with head because we already finish begining case
                Tail = i + size + BEGIN_DATA_SHIFTER ;
                Helper_SetFreeSpaceNode(Tail, SimHeap[i] - size - METADATA_CELL,SimHeap[i+PREVIOUS_FREE_BLOCK_SHIFT],SYMBOL_OF_HEAP_NULL); 
                SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = Tail ;
           }
            /* handle simulated array if neither head or tail is suitable to allocate this size
            *  that: 1. previous free space pointer will redefine in array but still point to the same cell
            *        2. next free space pointer will redefine in array but still point to the same cell
            *        3. metadata will redefine in array and store remaining size of free space
            *        4. update previous index ->next and Next index -> Pre
            */
           else {
                uint32 Temp = i + size + BEGIN_DATA_SHIFTER ;
                uint32 PreIndex = SimHeap[i+PREVIOUS_FREE_BLOCK_SHIFT];
                uint32 NextIndex = SimHeap[i+NEXT_FREE_BLOCK_SHIFT];
                SetOfGroupMallocCalls = VALID ;//to stop shifting tail with head because we already finish begining case
                Helper_SetFreeSpaceNode(Temp, SimHeap[i] - size - METADATA_CELL, SimHeap[i+PREVIOUS_FREE_BLOCK_SHIFT],SimHeap[i+NEXT_FREE_BLOCK_SHIFT]);  
                SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = i ;
                SimHeap[NextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = i ;
           }
            /* handle new allocation space
            *  that: 1. return value point to the beginning of data space
            *        2. edit metadata of new allocation to new allocation available space
            */
            Ret_SuitableIndex = i  + BEGIN_DATA_SHIFTER;
            SimHeap[i] = size ;
            break ;
        }
        else if (SimHeap[i] == size){
            /* handle simulated array if head is suitable to allocate this size and will take whole space
            *  that  1. shift head node to next node
            *        2. make next node -> previous free space equals !
            */
           if (i == Head){
                Head = SimHeap[i+NEXT_FREE_BLOCK_SHIFT] ;
                SimHeap[Head+PREVIOUS_FREE_BLOCK_SHIFT] = SYMBOL_OF_HEAP_NULL ; 

                /* to shift tail with first group calls for maollac*/
                if (SetOfGroupMallocCalls == INVALID ){
                    Tail = Head ;
                }
           }
            /* handle simulated array if tail is suitable to allocate this size
            *  that: 1.  define new tail index point to new free space and call Helper_sbrk to extend break index 
            *        2. new tail -> previous free space point to old tail -> previous free space
            *        3. new tail -> next free space point to !
            *        4. remove old tail node 
            */
           else if (i == Tail){
                sint32 New_Tail = Helper_sbrk(ONE_K); 
                if (New_Tail == INVALID){
                    printf("Invalid state from Helper_sbrk function\n");
                    exit(INVALID);
                }
                else {
                    Helper_SetFreeSpaceNode(New_Tail, ONE_K, SimHeap[Tail+PREVIOUS_FREE_BLOCK_SHIFT] ,SYMBOL_OF_HEAP_NULL);  
                    Tail = New_Tail ;
                }
           }
            /* handle simulated array if neither head or tail is suitable to allocate this size
            *  that: 1. make next node -> previous free space index point to temp -> previous free space
            *        2. make previous node -> next free space index point to temp -> next free space
            *        3. remove temp node 
            */
           else {
                uint32 PreTemp = SimHeap[i+PREVIOUS_FREE_BLOCK_SHIFT] ;
                uint32 NextTemp = SimHeap[i+NEXT_FREE_BLOCK_SHIFT] ;
                SimHeap[NextTemp+PREVIOUS_FREE_BLOCK_SHIFT] = SimHeap[i+PREVIOUS_FREE_BLOCK_SHIFT] ; 
                SimHeap[PreTemp+NEXT_FREE_BLOCK_SHIFT] = SimHeap[i+NEXT_FREE_BLOCK_SHIFT] ; 
           }

            SimHeap[i] = size ;
            Ret_SuitableIndex = i  + BEGIN_DATA_SHIFTER; 
            break;
        }
        else {
            /* make next index point to the next node
            */
            i = SimHeap[i+NEXT_FREE_BLOCK_SHIFT] ;
        }
    }

    /* if you exist from while because that there is no suitable free space
    * that : 1.  define new tail index point to new free space and call Helper_sbrk to extend break index 
    *        2. new tail -> previous free space point to old tail -> previous free space
    *        3. new tail -> next free space point to !
    *        4. remove old tail node 
    */
    if ( i == (uint32) SYMBOL_OF_HEAP_NULL  ){
        sint32 New_Tail = Helper_sbrk(ONE_K); 
        if (New_Tail == INVALID){
            printf("Invalid state from Helper_sbrk function\n");
            Ret_SuitableIndex = INVALID ;
            exit(INVALID);
        }
        else {
            Helper_SetFreeSpaceNode(New_Tail,ONE_K, SimHeap[Tail+NEXT_FREE_BLOCK_SHIFT],SYMBOL_OF_HEAP_NULL);
            Tail = New_Tail ;
            Ret_SuitableIndex = Tail + BEGIN_DATA_SHIFTER ;
        }
    }

    /* return index of allocation new space -> data */
    return Ret_SuitableIndex ;
}

void Helper_SetFreeSpaceNode(uint32 index, uint32 metadata, uint32 previous_content, uint32 next_content){
    SimHeap[index+PREVIOUS_FREE_BLOCK_SHIFT] = previous_content ; 
    SimHeap[index+NEXT_FREE_BLOCK_SHIFT] = next_content ;
    SimHeap[index] =  metadata;
}



void Helper_FreeOperationBeforeHead(uint32 index, uint32 size){
    SetOfGroupMallocCalls = VALID ; //to stop shifting tail with head because we already finish begining case
    uint32 PositionOfFreeBlock = index + size + 1 ; // to check that the target free block is adjecent for head or away from.
     /* if ptr is pointing to node before head node and away from it
     *  that : 1- define new node 
     *         2- make this node -> previous free space = !  
     *         3- this node -> next free space = old head
     *         4- old head -> previous free space = index
     *         5- update head to point to index
     * */
    if (PositionOfFreeBlock < Head){
         Helper_SetFreeSpaceNode(index,size,SYMBOL_OF_HEAP_NULL,Head);
         SimHeap[Head+PREVIOUS_FREE_BLOCK_SHIFT] = index ; 
         Head = index ;
    }
     /* if ptr is pointing to node just before head node and adjecent for it
     *  that : 1- define new metdata for free space to be equal the margin of new free space and head space
     *         2- make this node -> previous free space = !  
     *         3- this node -> next free space = old head -> next free space
     *         4- update head to point to index
     * */
    else if (PositionOfFreeBlock == Head){
         uint32 sizeOfhead = SimHeap[Head];
         uint32 IndexOfnextFreeSpace = SimHeap[Head+NEXT_FREE_BLOCK_SHIFT];
         Helper_SetFreeSpaceNode(index, (size+sizeOfhead+METADATA_CELL) ,SYMBOL_OF_HEAP_NULL,IndexOfnextFreeSpace);
         Head = index ;
    }
}


void Helper_FreeOperationAfterTail(uint32 index, uint32 size){
    uint32 PositionOfTailBlock = Tail + SimHeap[Tail] + 1 ; // to check that the target free block is adjecent for tail or away from.
     /* if ptr is pointing to tail before node node is away from
     *  that : 1- define new node
     *         2- this node -> previous free space = tail  
     *         3- this node -> next free space = !
     *         4- old tail -> next free space = index
     *         5- update tail to point to index
     * */
    if (PositionOfTailBlock < index){
         Helper_SetFreeSpaceNode(index,size,Tail,SYMBOL_OF_HEAP_NULL);
         SimHeap[Tail+NEXT_FREE_BLOCK_SHIFT] = index ; 
         Tail = index ;
    }
     /* if ptr is pointing to node just after tail node and adjecent for it
     *  redefine new metdata for free space to be equal the margin of new free space and tail space
     * */
    else if (PositionOfTailBlock == index){
         SimHeap[Tail] += ( size + METADATA_CELL );
    }
}

void Helper_FreeOperationMiddleNode(uint32 index,uint32 size){
    uint32 nextIndex = Head ; // to store the next free slot after index
    uint32 PreIndex = 0 ;     // to store the previous free slot before index

    // till reach to the strict next free slot after index  
    while (nextIndex < index){
        nextIndex = SimHeap[nextIndex+NEXT_FREE_BLOCK_SHIFT];
    }
    PreIndex = SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT]; // to store the previous free slot before index

    /*
    * define free block position for :
    * 1) index point to node just after free node(x) from right so we will extend x node to join two adjecent free spaces.
    * 2) index point to node just before free node(y) from left so we will extend y node to join two adjecent free spaces
    */
    uint32 PositionOfPreviousBlock = PreIndex + SimHeap[PreIndex] + 1 ; // deallcated block in right
    uint32 PositionOfNextBlock = index + size + 1 ; // deallcated block in left

    // index point to node just after free node(x) from right so we will extend x node to join two adjecent free spaces.
    if (PositionOfPreviousBlock == index && PositionOfNextBlock != nextIndex){
        uint32 NewSize = SimHeap[PreIndex]+size+METADATA_CELL ;
        /*
        * operation will be :
        * 1- redefine new size for previous node because we extend it .
        * 2- previous node previousIndex and nextIndex stay the same 
        * */
        Helper_SetFreeSpaceNode(PreIndex,NewSize,SimHeap[PreIndex+PREVIOUS_FREE_BLOCK_SHIFT],SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT]);
    }
    // index point to node just before free node(y) from left so we will extend y node to join two adjecent free spaces
    else if (PositionOfPreviousBlock != index && PositionOfNextBlock == nextIndex){
        uint32 NewSize = SimHeap[nextIndex]+size+METADATA_CELL ;
        /*
        * operation will be :
        * 1- define new node merge between deallocation and next node
        * 2- new node size will equal deallocation memory and next node metadata.
        * 3- new node previousIndex and nextIndex stay the same of old node
        * 4- update free list
        * */
        Helper_SetFreeSpaceNode(index,NewSize,SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT],SimHeap[nextIndex+NEXT_FREE_BLOCK_SHIFT]);
        SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = index ;

        /* update Tail to point on new update node */
        if (nextIndex == Tail){
            Tail = index ;
        }
        else{
            uint32 NextNextNodeIndex = SimHeap[nextIndex+NEXT_FREE_BLOCK_SHIFT];
            SimHeap[NextNextNodeIndex+PREVIOUS_FREE_BLOCK_SHIFT] = index ;
        }
    }
    // indexOfptr point to node just after free node(x) and before free node(y) from right so we will extend x,y node to join three adjecent free spaces    
    else if (PositionOfPreviousBlock == index && PositionOfNextBlock == nextIndex){
        uint32 NewSize = SimHeap[PreIndex]+size+SimHeap[nextIndex]+2*METADATA_CELL ;
        uint32 nextIndexContent = SimHeap[nextIndex+NEXT_FREE_BLOCK_SHIFT];
        Helper_SetFreeSpaceNode(PreIndex,NewSize,SimHeap[PreIndex+PREVIOUS_FREE_BLOCK_SHIFT],nextIndexContent);

        /* update Tail to point on new update node */
        if (nextIndex == Tail){
            Tail = PreIndex ;
        }
        else{
            uint32 NextNextNodeIndex = SimHeap[nextIndex+NEXT_FREE_BLOCK_SHIFT];
            SimHeap[NextNextNodeIndex+PREVIOUS_FREE_BLOCK_SHIFT] = PreIndex ;
        }

    }
    // indexOfptr point to node away not adjecent for any free node so we willn't extend and free node
    else {
        /*
        * operation will be :
        * 1- define new node 
        * 2- size for new node will be the same of deallocated memory.
        * 3- new node -> previous index will equal next node -> previous index
        * 4- new node -> next index will equal Previous node -> next index
        * 5- prevoius node -> next index = index
        * 6- next node -> pre index = index
        * */
        uint32 PreviousIndexContent = SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT] ;
        uint32 NextIndexContent = SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] ;
        Helper_SetFreeSpaceNode(index,size,PreviousIndexContent,NextIndexContent);
        SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = index ;
        SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = index ;
    }
}

sint32 Helper_BestFit(uint32 size){
    uint32 i = Head ;    // index to indicate which point that you stop on 
    sint32 Ret_SuitableIndex = INVALID ;  // to return which index is valid to alllocate into
    uint32 SuitableIndex = 0 ;
    uint8 FlagOfNoFreeSpace = INVALID ;

    /*
    * search on suitable index to allocate memory in first
    */
    while(i != (uint32) SYMBOL_OF_HEAP_NULL ){
        if (SimHeap[i] >= size){
            SuitableIndex =  i ;
            FlagOfNoFreeSpace = VALID ;
            break;
        }
        i = SimHeap[i+NEXT_FREE_BLOCK_SHIFT];
    }

    /*
    * search on suitable index to allocate memory
    */
    while(i != (uint32) SYMBOL_OF_HEAP_NULL && FlagOfNoFreeSpace == VALID){
        if (SimHeap[i] < SimHeap[SuitableIndex] && SimHeap[i] >= size){
            SuitableIndex =  i ;
            FlagOfNoFreeSpace = VALID ;
        }
        i = SimHeap[i+NEXT_FREE_BLOCK_SHIFT];
    }

    /* if you exist from while because that there is no suitable free space
    * that : 1.  define new tail index point to new free space and call Helper_sbrk to extend break index 
    *        2. new tail -> previous free space point to old tail -> previous free space
    *        3. new tail -> next free space point to !
    *        4. remove old tail node 
    */
    if ( FlagOfNoFreeSpace == (INVALID)  ){
        sint32 New_Tail = Helper_sbrk(ONE_K); 
        if (New_Tail == INVALID){
            printf("Invalid state from Helper_sbrk function\n");
            Ret_SuitableIndex = INVALID ;
            exit(INVALID);
        }
        else {
            Helper_SetFreeSpaceNode(New_Tail,ONE_K,SimHeap[Tail+NEXT_FREE_BLOCK_SHIFT],SYMBOL_OF_HEAP_NULL);
            Tail = New_Tail ;
            SuitableIndex = Tail ;
        }
    }

    if (SimHeap[SuitableIndex] > size){
        /* handle simulated array if head is suitable to allocate this size
        *  that: 1. previous free space pointer will redefine in array and store ! in this cell
        *        2. next free space pointer will redefine in array but still point to the same cell
        *        3. metadata will redefine in array and store remaining size of free space
        */
       if (SuitableIndex == Head){
            Head = SuitableIndex + size + METADATA_CELL ;
            Helper_SetFreeSpaceNode(Head,SimHeap[SuitableIndex] - size, SYMBOL_OF_HEAP_NULL,SimHeap[SuitableIndex+NEXT_FREE_BLOCK_SHIFT]);
       }
        /* handle simulated array if tail is suitable to allocate this size
        *  that: 1. previous free space pointer will redefine in array but still point to the same cell
        *        2. next free space pointer will redefine in array and store ! in this cell
        *        3. metadata will redefine in array and store remaining size of free space
        */
       else if (SuitableIndex == Tail){
            Tail = SuitableIndex + size + BEGIN_DATA_SHIFTER ;
            Helper_SetFreeSpaceNode(Tail, SimHeap[SuitableIndex] - size,SimHeap[SuitableIndex+PREVIOUS_FREE_BLOCK_SHIFT],SYMBOL_OF_HEAP_NULL); 
       }
        /* handle simulated array if neither head or tail is suitable to allocate this size
        *  that: 1. previous free space pointer will redefine in array but still point to the same cell
        *        2. next free space pointer will redefine in array but still point to the same cell
        *        3. metadata will redefine in array and store remaining size of free space
        */
       else {
            uint32 Temp = SuitableIndex + size + BEGIN_DATA_SHIFTER ;
            Helper_SetFreeSpaceNode(Temp, SimHeap[SuitableIndex] - size, SimHeap[SuitableIndex+PREVIOUS_FREE_BLOCK_SHIFT],SimHeap[SuitableIndex+NEXT_FREE_BLOCK_SHIFT]);  
       }
        /* handle new allocation space
        *  that: 1. return value point to the beginning of data space
        *        2. edit metadata of new allocation to new allocation available space
        */
        Ret_SuitableIndex = SuitableIndex  + BEGIN_DATA_SHIFTER;
        SimHeap[SuitableIndex] = size ;
    }
    else if (SimHeap[i] == size){
        /* handle simulated array if head is suitable to allocate this size and will take whole space
        *  that  1. shift head node to next node
        *        2. make next node -> previous free space equals !
        */
       if (SuitableIndex == Head){
            Head = SimHeap[SuitableIndex+NEXT_FREE_BLOCK_SHIFT] ;
            SimHeap[Head+PREVIOUS_FREE_BLOCK_SHIFT] = SYMBOL_OF_HEAP_NULL ; 
       }
        /* handle simulated array if tail is suitable to allocate this size
        *  that: 1.  define new tail index point to new free space and call Helper_sbrk to extend break index 
        *        2. new tail -> previous free space point to old tail -> previous free space
        *        3. new tail -> next free space point to !
        *        4. remove old tail node 
        */
       else if (SuitableIndex == Tail){
            sint32 New_Tail = Helper_sbrk(ONE_K); 
            if (New_Tail == INVALID){
                printf("Invalid state from Helper_sbrk function\n");
                exit(INVALID);
            }
            else {
                Helper_SetFreeSpaceNode(New_Tail, ONE_K, SimHeap[Tail+PREVIOUS_FREE_BLOCK_SHIFT] ,SYMBOL_OF_HEAP_NULL);  
                Tail = New_Tail ;
            }
       }
        /* handle simulated array if neither head or tail is suitable to allocate this size
        *  that: 1. make next node -> previous free space index point to temp -> previous free space
        *        2. make previous node -> next free space index point to temp -> next free space
        *        3. remove temp node 
        */
       else {
            uint32 PreTemp = SimHeap[SuitableIndex+PREVIOUS_FREE_BLOCK_SHIFT] ;
            uint32 NextTemp = SimHeap[SuitableIndex+NEXT_FREE_BLOCK_SHIFT] ;
            SimHeap[NextTemp+PREVIOUS_FREE_BLOCK_SHIFT] = SimHeap[SuitableIndex+PREVIOUS_FREE_BLOCK_SHIFT] ; 
            SimHeap[PreTemp+NEXT_FREE_BLOCK_SHIFT] = SimHeap[SuitableIndex+NEXT_FREE_BLOCK_SHIFT] ; 
       }
        SimHeap[SuitableIndex] = size ;
        Ret_SuitableIndex = SuitableIndex  + BEGIN_DATA_SHIFTER; 
    }

    /* return index of allocation new space -> data */
    return Ret_SuitableIndex ;   
}