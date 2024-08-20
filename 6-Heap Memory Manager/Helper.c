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
extern sint32 SimHeap[MAX_HEAPLENGHT];          // simulated heap
extern sint32 CurBreak;                         // break pointer on simulated heap
extern sint32 Head;                             // define first index of free space
extern sint32 Tail;                             // define last index of free spaces


/*=========================  Functions Implementation ===========================*/
sint32 Helper_sbrk (sint32 size){
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

sint32 Helper_FirstFit(sint32 size){ // size is sint32 but we will sure the this value will be positive but we define it signed to compare it with SimHeap[i]
    /*
    * - TailSbrkState: express about `if head and tail point to the same index and just before break pointer in your heap`
    * - Ret_SuitableIndex: used as the return value of target index that used to point to data 
    * - i: iterate in while loop
    * - size: resize the block with the new size. Because we are use three blocks for represent free node.
    */
    sint8 TailSbrkState = STATE1 ; 
    sint32 Ret_SuitableIndex = INVALID ;  
    sint32 i = Head ;  

    /* if head and tail are point to the same index before break index in heap
    *  should be ensured that there is no allocation memory between them (illustrated in issue #46)
    * - state 1 : tail point to node but not just before break pointer
    * - state 2 : tail point to node and just before break pointer
    */
    sint32 AdjecentNode = Tail + SimHeap[Tail]  ;
    if (AdjecentNode == CurBreak){
        TailSbrkState = STATE2 ; // used to call Helper_sbrk if it is needed.
    }

    /*
    * iterate to reserve some memory for required pointer
    * */
    while( i != (sint32) SYMBOL_OF_HEAP_NULL){ // while its next node is not jump over break
        Ret_SuitableIndex = Helper_AllocateMemory(i,size,TailSbrkState);

        if (Ret_SuitableIndex != INVALID){
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
        Ret_SuitableIndex = Helper_sbrk_Resize(size, TailSbrkState);
    }

    /* return index of allocation new space -> data */
    return Ret_SuitableIndex ;
}

sint32 Helper_sbrk_Resize(sint32 size, sint8 flag){
    /*
    * - Ret_SuitableIndex: used as the return value of target index that used to point to data 
    * - New_Tail: express what is the new created node.
    */
    sint32 Ret_SuitableIndex = INVALID ;  
    sint32 New = INVALID ;

    while (Ret_SuitableIndex == INVALID){
        New = Helper_sbrk(BREAK_STEP_SIZE);

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
            Helper_SetFreeSpaceNode(New,BREAK_STEP_SIZE, Tail,SYMBOL_OF_HEAP_NULL);
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
        Ret_SuitableIndex = Helper_AllocateMemory(Tail, size, flag);
    }

    Ret_SuitableIndex +=  BEGIN_DATA_SHIFTER ;
    return Ret_SuitableIndex ;
}

sint32 Helper_InsertPartFromNode (sint32 index,sint32 size, sint8 flag){
    sint32 Ret_SuitableIndex ; 

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
            Helper_SetFreeSpaceNode(Head, SimHeap[index] - size - METADATA_CELL, SYMBOL_OF_HEAP_NULL,NextIndex);
            SimHeap[NextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = Head ;
        }
        else if (HeadAndTail == VALID){
            Head = index + size + BEGIN_DATA_SHIFTER ; 
            Tail = Head ;
            Helper_SetFreeSpaceNode(Head, SimHeap[index] - size - METADATA_CELL, SYMBOL_OF_HEAP_NULL,SYMBOL_OF_HEAP_NULL);
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
        Helper_SetFreeSpaceNode(Tail, SimHeap[index] - size - METADATA_CELL,PreIndex,SYMBOL_OF_HEAP_NULL); 
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
        Helper_SetFreeSpaceNode(New, SimHeap[index] - size - METADATA_CELL, PreIndex,NextIndex);  
        SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = New ;
        SimHeap[NextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = New ;
   }
    /* handle new allocation space
    *  that: 1. return value point to the beginning of data space
    *        2. edit metadata of new allocation to new allocation available space
    */
    Ret_SuitableIndex = index  + BEGIN_DATA_SHIFTER;
    SimHeap[index] = size ;

    return Ret_SuitableIndex ;
}

sint32 Helper_RemoveCompleteNode (sint32 index, sint32 size, sint8 flag){
    sint32 Ret_SuitableIndex ; 
    
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
            sint32 New = Helper_sbrk(BREAK_STEP_SIZE); 
            if (New == INVALID){
                printf("Invalid state from Helper_sbrk function\n");
                exit(INVALID);
            }

            Helper_SetFreeSpaceNode(New,BREAK_STEP_SIZE,SYMBOL_OF_HEAP_NULL,SYMBOL_OF_HEAP_NULL);
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
    Ret_SuitableIndex = index  + BEGIN_DATA_SHIFTER;

    return Ret_SuitableIndex ;
}

// define new node with new info or update on some node
void Helper_SetFreeSpaceNode(sint32 index, sint32 metadata, sint32 previous_content, sint32 next_content){
    SimHeap[index+PREVIOUS_FREE_BLOCK_SHIFT] = previous_content ; 
    SimHeap[index+NEXT_FREE_BLOCK_SHIFT] = next_content ;
    SimHeap[index] =  metadata;
}



void Helper_FreeOperationBeforeHead(sint32 index, sint32 size){
    sint32 PositionOfFreeBlock = index + size + METADATA_CELL ; // to check that the target free block is adjecent for head or away from.
     /* if ptr is pointing to node before head node and away from it
     *  that : 1- define new node 
     *         2- make this node -> previous free space = !  
     *         3- this node -> next free space = old head
     *         4- old head -> previous free space = index
     *         5- update head to point to index
     * */
    if (PositionOfFreeBlock < Head ){
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
    else if (PositionOfFreeBlock == Head ){
        sint32 sizeOfhead = SimHeap[Head];
        sint32 nextIndex = SimHeap[Head+NEXT_FREE_BLOCK_SHIFT];
        Helper_SetFreeSpaceNode(index, size+sizeOfhead+METADATA_CELL ,SYMBOL_OF_HEAP_NULL,nextIndex);
        SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = index ;
        Head = index ;
    }
}


void Helper_FreeOperationAfterTail(sint32 index, sint32 size){
    sint32 PositionOfTailBlock = Tail + SimHeap[Tail] + METADATA_CELL ; // to check that the target free block is adjecent for tail or away from.
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

void Helper_FreeOperationMiddleNode(sint32 index,sint32 size){
    sint32 nextIndex = Head ; // to store the next free slot after index
    sint32 PreIndex = 0 ;     // to store the previous free slot before index

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
    if (PositionOfPreviousBlock == index && PositionOfNextBlock != nextIndex){
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
    else if (PositionOfPreviousBlock != index && PositionOfNextBlock == nextIndex){
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
        Helper_SetFreeSpaceNode(index,NewSize,PreIndex,NextNextNodeIndex);
        SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = index ;

        /* update Tail to point on new update node */
        if (nextIndex == Tail){
            Tail = index ;
        }
        else{
            SimHeap[NextNextNodeIndex+PREVIOUS_FREE_BLOCK_SHIFT] = index ;
        }
    }
    // indexOfptr point to node just after free node(x) and before free node(y) from right so we will extend x,y node to join three adjecent free spaces    
    else if (PositionOfPreviousBlock == index && PositionOfNextBlock == nextIndex){
        sint32 NewSize = SizeOfPrev+size+SizeOfNext+2*METADATA_CELL ;
        sint32 NextNextIndex = SimHeap[nextIndex+NEXT_FREE_BLOCK_SHIFT];

        /* update Tail to point on new update node */
        if (nextIndex == Tail){
            Helper_SetFreeSpaceNode(PreIndex,NewSize,SimHeap[PreIndex+PREVIOUS_FREE_BLOCK_SHIFT],SYMBOL_OF_HEAP_NULL);
            Tail = PreIndex ;
        }
        else{
            Helper_SetFreeSpaceNode(PreIndex,NewSize,SimHeap[PreIndex+PREVIOUS_FREE_BLOCK_SHIFT],NextNextIndex);
            SimHeap[NextNextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = PreIndex ;
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
        sint32 PreviousIndexContent = SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT] ;
        sint32 NextIndexContent = SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] ;
        Helper_SetFreeSpaceNode(index,size,PreviousIndexContent,NextIndexContent);
        SimHeap[PreIndex+NEXT_FREE_BLOCK_SHIFT] = index ;
        SimHeap[nextIndex+PREVIOUS_FREE_BLOCK_SHIFT] = index ;
    }
}

sint32 Helper_BestFit(sint32 size){
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
    sint32 SuitableIndex = 0 ;
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
        }
        i = SimHeap[i+NEXT_FREE_BLOCK_SHIFT];
    }

    /*
    * search on suitable index to allocate memory
    */
    while(i != (sint32) SYMBOL_OF_HEAP_NULL && FlagOfNoFreeSpace == VALID){
        if (SimHeap[i] < SimHeap[SuitableIndex] && SimHeap[i] >= size){
            SuitableIndex =  i ;
        }
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
    }

    /* if you exist from while because that there is no suitable free space 
    * so we will move sbrk pointer and update our free list
    */
    if ( FlagOfNoFreeSpace == INVALID ){
        Ret_SuitableIndex = Helper_sbrk_Resize(size, TailSbrkState);
    }
    else {
        Ret_SuitableIndex = Helper_AllocateMemory(SuitableIndex,size,TailSbrkState);
        Ret_SuitableIndex += BEGIN_DATA_SHIFTER;
    }
    
    SimHeap[SuitableIndex] = size ;

    /* return index of allocation new space -> data */
    return Ret_SuitableIndex ;   
}

sint32 Helper_AllocateMemory(sint32 index, sint32 size, sint8 flag){
    sint32 Ret_SuitableIndex = INVALID ;  
    /* memory allocation action */
    if (SimHeap[index] >= size + NUMBER_OF_FREE_NODE_ELEMENTS ){
        Ret_SuitableIndex = Helper_InsertPartFromNode (index,size,flag);
    }
    else if (SimHeap[index] == size || SimHeap[index] == size+1 || SimHeap[index] == size+2){
        Ret_SuitableIndex = Helper_RemoveCompleteNode (index,size,flag);
    } 
    return Ret_SuitableIndex ; 
}