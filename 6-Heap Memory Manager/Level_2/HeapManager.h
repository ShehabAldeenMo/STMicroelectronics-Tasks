/*============================================================================
 * @file name      : HeapManager.h
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This header file declares functions for managing a simulated heap memory.
 * It includes prototypes for allocating and freeing memory blocks. The allocation
 * strategy can be configured to use either the First Fit or Best Fit algorithm.
 * Functions handle memory allocation, deallocation, and coalescing of free blocks
 * to reduce fragmentation.
 *
=============================================================================
 * @Notes:
 * - The allocation strategy is controlled by the macro `FIRSTFIT`. If `FIRSTFIT` is 
 *   set to `ENABLE`, the First Fit algorithm is used for allocation; otherwise, the 
 *   Best Fit algorithm is used.
 * - Ensure that the corresponding `HeapManager.c` file is included in the build to
 *   provide implementations for these functions.
 *
 ******************************************************************************
 ==============================================================================
*/


#ifndef HEAP_MANGER_H_
#define HEAP_MANGER_H_

/*===================================  Includes ===============================*/
#include "../Level_3/HeapUtils.h"
#include "../Level_2/HeapExtras.h"

/*============================  Configurations ==============================*/
/*
* to enable first fit algorthim set 'ENABLE'
* to disable it set 'DISABLE'
*/
#define FIRSTFIT         ENABLE



/*==========================  Function Prototypes ===========================*/
/*
 * Name             : HeapManager_Malloc
 * Description      : Allocates a block of memory from the simulated heap using the specified 
 *                    allocation strategy (First Fit or Best Fit). The allocated memory's index 
 *                    is determined based on the chosen fit algorithm.
 * Input            : size - The size of the memory block to be allocated (in bytes).
 * Output           : None
 * Return           : Returns a pointer to the allocated memory block within the simulated heap.
 * Notes            : This function abstracts the memory allocation process, depending on the 
 *                    strategy defined by the user. If FIRSTFIT is enabled, the allocation uses 
 *                    the First Fit strategy; otherwise, it uses the Best Fit strategy.
 */
sint32* HeapManager_Malloc(sint32 size);

/*
 * Name             : HeapManager_Free
 * Description      : Frees a previously allocated block of memory, identified by the pointer 
 *                    provided. The function coalesces adjacent free blocks to minimize fragmentation.
 * Input            : ptr - A pointer to the memory block that needs to be freed.
 * Output           : None
 * Return           : None
 * Notes            : This function handles different scenarios depending on the position of 
 *                    the block to be freed relative to the head and tail nodes. It ensures 
 *                    optimal memory usage by merging adjacent free blocks when possible.
 */
void HeapManager_Free(sint32* ptr);

#endif