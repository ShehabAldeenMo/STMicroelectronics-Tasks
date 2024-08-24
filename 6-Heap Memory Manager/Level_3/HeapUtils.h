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
#define MAX_HEAPLENGHT                         200*ONE_K*ONE_K  // Maximum length for static array used to simulated heap
#define INVALID                                  -2
#define VALID                                    -1
#define PREVIOUS_FREE_BLOCK_SHIFT                 1
#define NEXT_FREE_BLOCK_SHIFT                     2
#define SYMBOL_OF_HEAP_NULL                      -1
#define BEGIN_DATA_SHIFTER                        1
#define METADATA_CELL                             1
#define NUMBER_OF_FREE_NODE_ELEMENTS              3
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


/*==============================  Functions Prototypes   ==========================*/
/*
 * Name             : HeapUtils_AllocationCoreLoop
 * Description      : Core function for memory allocation within the simulated heap. It handles
 *                    the allocation process by managing padding and updating the simulated heap.
 * Input            : index - The starting index in the heap where allocation is to be attempted.
 *                    size - The size of memory to be allocated.
 * Output           : None
 * Return           : Returns the index of the allocated memory if successful, or `INVALID` if 
 *                    allocation fails.
 * Notes            : This function considers padding and handles cases where the requested size
 *                    fits partially or exactly in the available space. It calls helper functions 
 *                    to manage memory allocation and node adjustments.
 */
sint32 HeapUtils_AllocationCoreLoop(sint32 index, sint32 size);

/*
 * Name             : HeapUtils_sbrk
 * Description      : Adjusts the simulated program break (heap end) by the specified size. It
 *                    checks if the new break position is within valid heap boundaries and updates
 *                    the `CurBreak` pointer accordingly.
 * Input            : size - The size to adjust the program break by (can be positive or negative).
 * Output           : None
 * Return           : Returns the previous value of `CurBreak` if the adjustment is successful,
 *                    or returns `INVALID` if the requested size exceeds the heap boundaries.
 * Notes            : This function is used to manage the dynamic allocation of memory within
 *                    the simulated heap by effectively controlling the size of the heap.
 */
sint32 HeapUtils_sbrk (sint32 size);

/*
 * Name             : HeapUtils_sbrkResize
 * Description      : Extends the heap by adjusting the program break and resizing the heap if 
 *                    necessary. It ensures that there is enough space for the requested size 
 *                    and manages heap expansion if required.
 * Input            : size - The size to allocate within the heap.
 *                    flag - A flag indicating the state or mode for resizing.
 * Output           : None
 * Return           : Returns the index of the allocated memory if successful, or `INVALID` if 
 *                    resizing fails.
 * Notes            : This function may extend the heap in steps and handle different states 
 *                    to ensure the heap has sufficient space for allocation. It calls helper 
 *                    functions to manage heap resizing and memory allocation.
 */
sint32 HeapUtils_sbrkResize(sint32 size,sint8 flag);

/*
 * Name             : HeapUtils_SplitFreeBlock
 * Description      : Splits a free block of memory in the simulated heap to allocate a specific 
 *                    size. It updates the metadata of the free blocks and adjusts pointers 
 *                    to maintain the integrity of the free list.
 * Input            : Node - The starting index of the free block to split.
 *                    spliting_size - The size of memory to allocate from the free block.
 * Output           : None
 * Return           : Returns the index of the newly allocated memory.
 * Notes            : This function handles different scenarios for splitting free blocks, 
 *                    including cases where the block is at the head or tail of the free list. 
 *                    It updates the metadata and pointers to reflect the split.
 */
sint32 HeapUtils_SplitFreeBlock (sint32 Node, sint32 spliting_size);

/*
 * Name             : HeapUtils_RemoveFreeBlock
 * Description      : Removes a free block of memory from the simulated heap. It updates 
 *                    the free list and adjusts pointers to maintain the integrity of the heap 
 *                    after the block has been removed.
 * Input            : index - The starting index of the free block to remove.
 *                    size - The size of the block to remove.
 * Output           : None
 * Return           : Returns the index of the newly allocated memory after removal.
 * Notes            : This function handles different cases for removing free blocks, including 
 *                    updates to the head and tail of the free list and adjusting pointers to 
 *                    maintain the heap's free list structure.
 */
sint32 HeapUtils_RemoveFreeBlock (sint32 index, sint32 size);

/*
 * Name             : HeapUtils_SetFreeNodeInfo
 * Description      : Sets the metadata and pointers for a free node in the simulated heap.
 *                    This function updates the free block's metadata and the pointers to 
 *                    previous and next free blocks.
 * Input            : index - The index of the free block to update.
 *                    metadata - The metadata value to set for the free block.
 *                    previous_content - The index of the previous free block.
 *                    next_content - The index of the next free block.
 * Output           : None
 * Return           : None
 * Notes            : This function helps in maintaining the free list structure by setting 
 *                    appropriate metadata and pointers for free blocks.
 */
void   HeapUtils_SetFreeNodeInfo(sint32 index, sint32 metadata, sint32 previous_content, sint32 next_content);


sint8 HeapUtils_SearchOnIndexInFreeList(sint32 index);


#endif