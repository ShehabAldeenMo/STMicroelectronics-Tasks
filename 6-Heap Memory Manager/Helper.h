/*============================================================================
 * @file name      : Helper.h
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This header file defines the interface for the helper functions used in the 
 * Heap Memory Manager (HMM). It declares functions and data structures that 
 * are essential for the dynamic memory allocation and deallocation within the 
 * simulated heap environment.
 *
 =============================================================================
 * @Notes:
 * - Ensure that this header is included where the HMM helper functions are 
 *   needed.
 * - This header provides necessary declarations for the `Helper.c` file.
 * - The functions declared here must be implemented in `Helper.c`.
 *
 ******************************************************************************
 ==============================================================================
*/



#ifndef HMM_H_
#define HMM_H_

/*===================================  Includes ==============================*/

#include <stdio.h>          // Standard I/O functions
#include <stdlib.h>         // Standard library functions: memory management, program utilities, etc.
#include <string.h>         // String manipulation functions



/*==================================  Definitions ===========================*/
#define ONE_K                                   1024
#define MAX_HEAPLENGHT                         20*ONE_K  // Maximum length for static array used to simulated heap
#define INVALID                                  -1
#define VALID                                     0
#define PREVIOUS_FREE_BLOCK_SHIFT                 1
#define NEXT_FREE_BLOCK_SHIFT                     2
#define SYMBOL_OF_HEAP_NULL                      '!'
#define BEGIN_DATA_SHIFTER                        1
#define METADATA_CELL                             1
#define ENABLE                                    1
#define DISABLE                                   0 

/*==========================  typedef   =====================================*/
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



/*============================  Helper Functions ==============================*/
/*
 * Name             : Helper_sbrk
 * Description      : Adjusts the simulated program break (heap end) by the specified size. 
 *                    It checks if the new break position is within the valid heap boundaries 
 *                    and updates the `CurBreak` pointer accordingly.
 * Input            : size - The size to adjust the program break by (can be positive or negative).
 * Output           : None
 * Return           : Returns the previous value of `CurBreak` if the adjustment is successful, 
 *                    or returns `INVALID` if the requested size exceeds the heap boundaries.
 * Notes            : This function is used to manage the dynamic allocation of memory within 
 *                    the simulated heap by effectively controlling the size of the heap.
 */
sint32 Helper_sbrk (sint32 size);


/*
 * Name             : Helper_FirstFit
 * Description      : Searches the simulated heap for the first suitable block of memory that 
 *                    can accommodate the requested size using the First Fit strategy. If a suitable 
 *                    block is found, it allocates the block and adjusts the free space metadata.
 * Input            : size - The size of the memory block to be allocated (in bytes).
 * Output           : None
 * Return           : Returns the index of the allocated memory block in the simulated heap, or 
 *                    `INVALID` if no suitable block is found.
 * Notes            : The function handles various cases where the block to be allocated is either 
 *                    at the head, tail, or in between, adjusting the linked list of free blocks 
 *                    to reflect the new allocation.
 */
sint32 Helper_FirstFit(uint32 size);

/*
 * Name             : Helper_SetFreeSpaceNode
 * Description      : Updates the metadata and linked list pointers for a free space node in the simulated heap.
 * Input            : index - The index in the simulated heap where the free space node is located.
 *                    metadata - The size of the free block to be stored at the given index.
 *                    previous_content - The index of the previous free block in the linked list.
 *                    next_content - The index of the next free block in the linked list.
 * Output           : None
 * Return           : None
 * Notes            : This function sets the previous and next free block pointers and the metadata for 
 *                    the free space node at the specified index in the simulated heap.
 */
void Helper_SetFreeSpaceNode(uint32 index, uint32 metadata, uint32 previous_content, uint32 next_content);

/*
 * Name             : Helper_FreeOperationBeforeHead
 * Description      : Handles the case where a block of memory is being freed and is located before the current head.
 * Input            : index - The index of the freed block in the simulated heap.
 *                    size - The size of the freed block.
 * Output           : None
 * Return           : None
 * Notes            : This function manages the merging of the freed block with adjacent free blocks if necessary.
 *                    It checks if the freed block is adjacent to the head or separate and updates the head 
 *                    and free block pointers accordingly.
 *                    - If the freed block is before the head and separate, it creates a new free block node 
 *                      and adjusts the previous and next pointers.
 *                    - If the freed block is just before the head and adjacent, it extends the current head 
 *                      block to include the freed block.
 */
void Helper_FreeOperationBeforeHead(uint32 index, uint32 size);

/*
 * Name             : Helper_FreeOperationAfterTail
 * Description      : Manages the case where a block of memory is being freed and is located after the current tail.
 * Input            : index - The index of the freed block in the simulated heap.
 *                    size - The size of the freed block.
 * Output           : None
 * Return           : None
 * Notes            : This function handles merging of the freed block with adjacent free blocks if necessary.
 *                    - If the freed block is separate from the tail, it creates a new free block node and adjusts pointers.
 *                    - If the freed block is adjacent to the tail, it extends the current tail block to include the freed block.
 */
void Helper_FreeOperationAfterTail(uint32 index,uint32 size);

/*
 * Name             : Helper_FreeOperationMiddleNode
 * Description      : Handles the case where a block of memory is being freed and is located between existing free blocks.
 * Input            : index - The index of the freed block in the simulated heap.
 *                    size - The size of the freed block.
 * Output           : None
 * Return           : None
 * Notes            : This function manages merging of the freed block with adjacent free blocks if necessary.
 *                    - If the freed block is adjacent to the previous free block, it extends the previous block to include the freed block.
 *                    - If the freed block is adjacent to the next free block, it extends the next block to include the freed block.
 *                    - If the freed block is between two adjacent free blocks, it merges all three blocks into a single larger block.
 *                    - If the freed block is not adjacent to any free block, it creates a new free block and adjusts pointers accordingly.
 */
void Helper_FreeOperationMiddleNode(uint32 index,uint32 size);

/*
 * Name             : Helper_BestFit
 * Description      : Finds the best fit for a memory allocation request based on the Best Fit algorithm. Searches for the smallest free block that is large enough to satisfy the request. If no suitable block is found, it extends the heap.
 * Input            : size - The size of the memory to be allocated.
 * Output           : Returns the index of the allocated memory block in the simulated heap or INVALID if allocation fails.
 * Return           : sint32 - The index of the allocated memory block plus an offset indicating the start of the data section, or INVALID if no suitable block is found.
 * Notes            : 
 * 1. Searches through the free list to find the smallest free block that fits the requested size.
 * 2. If no suitable block is found, the function extends the heap using `Helper_sbrk` and creates a new free block.
 * 3. Adjusts the metadata and pointers in the simulated heap to allocate the memory.
 * 4. Handles three scenarios for allocation:
 *    - If the suitable block is at the head of the list.
 *    - If the suitable block is at the tail of the list.
 *    - If the suitable block is neither at the head nor at the tail.
 * 5. Updates the free list and the metadata of the free blocks accordingly.
 */

sint32 Helper_BestFit(uint32 size);


/*============================  Configurations ==============================*/
/*
* to enable first fit algorthim set 'ENABLE'
* to disable it set 'DISABLE'
*/
#define FIRSTFIT         ENABLE

#endif