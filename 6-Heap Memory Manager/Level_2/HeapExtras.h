/*============================================================================
 * @file name      : HeapExtras.h
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This header file declares functions for managing heap memory. It includes
 * prototypes for operations related to allocating and freeing memory, and
 * managing free blocks within a simulated heap. Functions for handling free
 * blocks before, after, and within the free list are also declared.
 *
=============================================================================
 * @Notes:
 * - Include this header file in modules that require heap management functionalities.
 * - Ensure that the corresponding `HeapExtras.c` file is included in the build
 *   to provide implementations for these functions.
 *
 ******************************************************************************
 ==============================================================================
*/

#ifndef HEAP_EXTRAS_H_
#define HEAP_EXTRAS_H_

/*===================================  Includes ===============================*/
#include "../Level_3/HeapUtils.h"


/*
 * Name             : HeapExtras_FirstFit
 * Description      : Finds the first free block in the heap that is large enough to accommodate the requested size. 
 *                     If no suitable block is found, the function resizes the heap using sbrk to accommodate the request.
 * Input            : sint32 size - The size of the memory block requested.
 * Output           : sint32 - The index of the allocated memory block, or INVALID if no suitable block is found and resizing is required.
 * Return           : sint32 - The index of the allocated memory block, or INVALID if resizing is needed.
 * Notes            : 
 * - The function first checks if the free list head and tail are adjacent to the heap break pointer to determine if resizing is necessary.
 * - If the free list has a suitable block, it is allocated. Otherwise, the heap is resized to accommodate the request.
 */
sint32 HeapExtras_FirstFit(sint32 size);

/*
 * Name             : HeapExtras_FreeOperationBeforeHead
 * Description      : Manages the freeing of memory blocks located before or at the head of the free list.
 * Input            : sint32 index - The index of the block to be freed.
 *                     sint32 size  - The size of the block to be freed.
 * Output           : None
 * Return           : None
 * Notes            :
 * - If the block to be freed is before the current head of the free list, it updates the head to point to the new block.
 * - If the block is adjacent to the head, it merges the block with the head and updates pointers accordingly.
 */
void   HeapExtras_FreeOperationBeforeHead(sint32 index, sint32 size);

/*
 * Name             : HeapExtras_FreeOperationAfterTail
 * Description      : Manages the freeing of memory blocks located after or at the tail of the free list.
 * Input            : sint32 index - The index of the block to be freed.
 *                     sint32 size  - The size of the block to be freed.
 * Output           : None
 * Return           : None
 * Notes            :
 * - If the block to be freed is after the current tail of the free list, it updates the tail to point to the new block.
 * - If the block is adjacent to the tail, it merges the block with the tail and updates pointers accordingly.
 */
void   HeapExtras_FreeOperationAfterTail(sint32 index,sint32 size);

/*
 * Name             : HeapExtras_FreeOperationMiddleNode
 * Description      : Handles the freeing of memory blocks located between existing free nodes in the free list.
 * Input            : sint32 index - The index of the block to be freed.
 *                     sint32 size  - The size of the block to be freed.
 * Output           : None
 * Return           : None
 * Notes            :
 * - The function merges the freed block with adjacent free blocks if they exist.
 * - It adjusts the free list pointers to ensure the free list remains contiguous and correctly ordered.
 * - If the freed block connects multiple adjacent free blocks, they are merged into a single larger block.
 */
void   HeapExtras_FreeOperationMiddleNode(sint32 index,sint32 size);

/*
 * Name             : HeapExtras_BestFit
 * Description      : Finds the best-fit block in the heap that is large enough to accommodate the requested size, 
 *                     choosing the smallest block that fits the requirement. If no suitable block is found, it resizes the heap.
 * Input            : sint32 size - The size of the memory block requested.
 * Output           : sint32 - The index of the allocated memory block, or INVALID if no suitable block is found and resizing is required.
 * Return           : sint32 - The index of the allocated memory block, or INVALID if resizing is needed.
 * Notes            :
 * - The function first searches the free list for the smallest block that fits the requested size.
 * - If no suitable block is found, the heap is resized to accommodate the request.
 * - It takes into account the state of the free list and the heap break pointer to decide if resizing is necessary.
 */
sint32 HeapExtras_BestFit(sint32 size);

/*
 * Name             : HMM_Init
 * Description      : Initializes the simulated heap by setting up the head, tail, and program 
 *                    break pointer. The heap is prepared with initial values to manage memory 
 *                    allocation and deallocation.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : This function must be called before any memory allocation or deallocation 
 *                    operations to ensure that the simulated heap is properly initialized.
 */
static void HeapExtras_Init();

#endif