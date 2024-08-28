/*============================================================================
 * @file name      : HeapUtils.h
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This header file declares utility functions used by the heap manager for 
 * managing memory within the simulated heap. Functions include operations 
 * for allocation, resizing, splitting, and removing free blocks, as well as 
 * updating metadata and pointers in the heap.
 *
 =============================================================================
 * @Notes:
 * - The header file provides function prototypes for the implementations in 
 *   `HeapUtils.c`.
 * - It is important to include this header in files that use these utility 
 *   functions.
 * - Ensure that the heap is correctly initialized and managed when using these 
 *   functions.
 *
 ******************************************************************************
 ==============================================================================
*/
#ifndef HEAP_UTILS_H_
#define HEAP_UTILS_H_

/*===================================  Includes ==============================*/
#include <stdio.h>          // Standard I/O functions
#include <stdlib.h>         // Standard library functions: memory management, program utilities, etc.
#include <string.h>         // String manipulation functions


/*==================================  Definitions ===========================*/
#define ONE_K                                   1024
#define MAX_HEAPLENGHT                         50*ONE_K*ONE_K  // Maximum length for static array used to simulated heap
#define INVALID                                  -2
#define VALID                                    -1
#define NOT_ENTER                                 1
#define ENTERED                                   0
#define BREAK_STEP_SIZE                          ONE_K
#define STATE1                                   INVALID
#define STATE2                                   VALID
#define INFO_NODE                                 2
#define ENABLE                                    1
#define DISABLE                                   0
#define ON                                        1
#define OFF                                       0


/*==============================  Configurations   =====================================*/
#define DEBUGGING                                DISABLE


/*==============================  typedef   =====================================*/
typedef unsigned char boolean         ;
typedef signed char sint8             ;
typedef unsigned char uint8           ;
typedef signed short int sint16       ;
typedef unsigned short int uint16     ;
typedef signed int sint32             ;
typedef unsigned int uint32           ;
typedef signed long long int sint64   ;
typedef unsigned long long int uint64 ;
typedef float float32                 ;
typedef double float64                ;

typedef struct FreeBlock {
    size_t BlockSize;
    struct FreeBlock* NextFreeBlock;
    struct FreeBlock* PreviousFreeBlock;
} FreeBlock;

/*==============================  Functions Prototypes   ==========================*/
/*
 * Name             : HeapUtils_AllocationCoreLoop
 * Description      : Handles the core loop for allocating memory from a free block. Depending on the 
 *                    size of the free block and the requested size, the function either splits the block, 
 *                    removes it from the free list, or returns an invalid pointer if allocation fails.
 * Input            : FreeBlock* ptrBlock - A pointer to the free block from which memory is to be allocated.
 *                    size_t ReqSize - The size of memory requested for allocation.
 * Output           : None.
 * Return           : sint8* - A pointer to the allocated memory or NULL if allocation fails.
 * Notes            : The function handles cases where padding is required and adjusts the requested size 
 *                    accordingly. It then attempts to allocate memory by either splitting the block or 
 *                    removing it from the free list.
 */
sint8* HeapUtils_AllocationCoreLoop(FreeBlock* ptrBlock,size_t size);

/*
 * Name             : HeapUtils_sbrk
 * Description      : Simulates the behavior of the sbrk system call, which increments the program's 
 *                    data space by a specified size. The function ensures that the new program break 
 *                    is within the simulated heap limits.
 * Input            : size_t size - The amount of memory to extend the program's data space.
 * Output           : None.
 * Return           : sint8* - A pointer to the previous program break or NULL if the requested size is invalid.
 * Notes            : The function checks if the requested size would cause the program break to exceed 
 *                    the simulated heap's boundaries, returning NULL if the request is invalid.
 */
sint8* HeapUtils_sbrk (size_t size);

/*
 * Name             : HeapUtils_sbrkResize
 * Description      : Extends the heap by a specified size using the sbrk function and attempts to 
 *                    allocate memory from the newly extended space. Depending on the provided flag, 
 *                    the function either creates a new free block or extends the size of the current block.
 * Input            : size_t ReqSize - The size of memory requested for allocation.
 *                    sint8 flag - A flag indicating the state of the heap (e.g., STATE1 for creating a new block, 
 *                    STATE2 for resizing the existing block).
 * Output           : None.
 * Return           : sint8* - A pointer to the allocated memory or NULL if allocation fails.
 * Notes            : The function loops until memory is successfully allocated, extending the heap 
 *                    as necessary. It handles cases where no suitable free space exists, creating 
 *                    or resizing blocks based on the provided flag.
 */
sint8* HeapUtils_sbrkResize(size_t ReqSize,sint8 flag);

/*
 * Name             : HeapUtils_SplitFreeBlock
 * Description      : Splits a free block into two parts: one for allocation and the other as a smaller free block. 
 *                    Depending on the block's position in the free list (head, tail, or middle), the function 
 *                    updates the free list accordingly and returns a pointer to the allocated memory.
 * Input            : FreeBlock* Node - A pointer to the free block to be split.
 *                    size_t spliting_size - The size of the memory to be allocated from the block.
 * Output           : None.
 * Return           : sint8* - A pointer to the allocated memory.
 * Notes            : The function handles different cases for head, tail, and middle nodes, ensuring 
 *                    that the free list is correctly updated. It also manages the remaining free space 
 *                    after allocation and adjusts the free list pointers as needed.
 */
sint8* HeapUtils_SplitFreeBlock (FreeBlock* Node, size_t spliting_size);

/*
 * Name             : HeapUtils_RemoveFreeBlock
 * Description      : Removes a free block from the free list after allocating memory from it. 
 *                    The function adjusts the head and tail pointers of the free list as necessary 
 *                    and updates the free list structure to maintain consistency.
 * Input            : FreeBlock* Node - A pointer to the free block to be removed.
 *                    size_t spliting_size - The size of memory that was allocated from the block.
 * Output           : None.
 * Return           : sint8* - A pointer to the allocated memory within the removed block.
 * Notes            : The function handles different cases for the head, tail, and middle nodes of 
 *                    the free list, ensuring that the list remains properly linked. It also checks if 
 *                    the block is in the free list and updates the list pointers accordingly.
 */
sint8* HeapUtils_RemoveFreeBlock (FreeBlock* Node, size_t spliting_size);

/*
 * Name             : HeapUtils_SearchOnIndexInFreeList
 * Description      : Searches for a specific free block within the free list. The function iterates 
 *                    through the list to check if the given block is present.
 * Input            : FreeBlock* block - A pointer to the block to be searched for in the free list.
 * Output           : None.
 * Return           : sint8 - Returns VALID (non-zero) if the block is found in the free list, INVALID (zero) otherwise.
 * Notes            : The function traverses the free list starting from the head node, comparing 
 *                    each block with the provided block to determine if it exists in the list.
 */
void   HeapUtils_SetFreeNodeInfo(FreeBlock* Node, size_t metadata, FreeBlock* previous_content, FreeBlock* next_content);

/*
 * Name             : HeapUtils_SetFreeNodeInfo
 * Description      : Sets the metadata and linkage information for a free block. This includes updating 
 *                    the block size, previous block pointer, and next block pointer.
 * Input            : FreeBlock* Node - A pointer to the free block to be updated.
 *                    size_t metadata - The new block size to be set for the free block.
 *                    FreeBlock* previous_content - Pointer to the previous free block in the list.
 *                    FreeBlock* next_content - Pointer to the next free block in the list.
 * Output           : None.
 * Return           : None.
 * Notes            : The function updates the block's metadata and pointers to maintain the correct 
 *                    structure of the free list.
 */
sint8  HeapUtils_SearchOnIndexInFreeList(FreeBlock* block);


void Shrink_Break(sint8 flag);

#endif 