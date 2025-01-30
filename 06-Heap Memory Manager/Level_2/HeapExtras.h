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
 * Description      : Allocates memory using the first-fit strategy from the heap's free list. 
 *                    If no suitable free block is found, the heap is expanded using sbrk.
 * Input            : size - The requested size of memory to allocate.
 * Output           : None.
 * Return           : Pointer to the allocated memory block if successful, or NULL if the allocation fails.
 * Notes            : The function ensures that the requested size is aligned to an 8-byte boundary 
 *                    and that the size is at least as large as the FreeBlock structure to prevent overwriting.
 */
sint8* HeapExtras_FirstFit(size_t size);

/*
 * Name             : HeapExtras_Init
 * Description      : Initializes the simulated heap by setting up the initial free block. 
 *                    This function sets the head and tail of the free list to point to the 
 *                    initial block and sets the break pointer (CurBreak) to the end of the simulated heap.
 * Input            : None.
 * Output           : None.
 * Return           : None.
 * Notes            : The function assumes that the simulated heap (SimHeap) has been declared and 
 *                    is available for use. The size of the heap is assumed to be ONE_K.
 */
void   HeapExtras_Init();

/*
 * Name             : HeapExtras_FreeOperationBeforeHead
 * Description      : Handles the operation of freeing a block of memory located before the current head 
 *                    of the free list. Depending on the position of the block relative to the head, 
 *                    the function either inserts the block as the new head or merges it with the existing head.
 * Input            : FreeBlock* Node - A pointer to the block of memory that needs to be freed and placed before the current head.
 * Output           : None.
 * Return           : None.
 * Notes            : The function checks if the block is adjacent to the current head or not, and then either 
 *                    updates the free list structure accordingly or merges the block with the head.
 */
void   HeapExtras_FreeOperationBeforeHead(FreeBlock* Node);

/*
 * Name             : HeapExtras_FreeOperationAfterTail
 * Description      : Handles the operation of freeing a block of memory located after the current tail 
 *                    of the free list. Depending on the position of the block relative to the tail, 
 *                    the function either appends the block as the new tail or merges it with the existing tail.
 * Input            : FreeBlock* Node - A pointer to the block of memory that needs to be freed and placed after the current tail.
 * Output           : None.
 * Return           : None.
 * Notes            : The function checks if the block is adjacent to the current tail or not, and then either 
 *                    updates the free list structure accordingly or merges the block with the tail.
 */
void   HeapExtras_FreeOperationAfterTail(FreeBlock* Node);

/*
 * Name             : HeapExtras_FreeOperationMiddleNode
 * Description      : Handles the operation of freeing a block of memory located between two existing free blocks 
 *                    in the middle of the free list. Depending on the position of the block relative to its 
 *                    neighboring blocks, the function either inserts the block in the correct position or merges 
 *                    it with adjacent blocks.
 * Input            : FreeBlock* Node - A pointer to the block of memory that needs to be freed and placed between 
 *                    two existing free blocks.
 * Output           : None.
 * Return           : None.
 * Notes            : The function checks if the block is adjacent to one or both neighboring blocks and then 
 *                    either updates the free list structure accordingly or merges the block with the adjacent blocks.
 */
void   HeapExtras_FreeOperationMiddleNode(FreeBlock* Node);

void TailBreakStatus();

#endif