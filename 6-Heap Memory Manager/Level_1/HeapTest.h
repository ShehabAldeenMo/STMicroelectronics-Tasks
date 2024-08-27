/*============================================================================
 * @file name      : HeapTest.h
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This header file declares the functions used for testing the simulated heap memory.
 * It includes functions for printing heap borders, performing random allocations and frees,
 * and printing the free list from both the head and tail of the heap.
 *
=============================================================================
 * @Notes:
 * - Ensure that the header file is included in the source files where these functions are
 *   used.
 * - Modify the `#include` directives as needed based on the file structure.
 *
 ******************************************************************************
 ==============================================================================
*/
#ifndef HEAP_TEST_H_
#define HEAP_TEST_H_

/*===================================  Includes ===============================*/
#include "../Level_2/HeapManager.h"
//#include "../Level_2/HeapExtras.h"
#include <time.h>



/*==================================  Definitions =============================*/
#define NUM_ALLOCS 100000
#define MAX_SIZE 10240
#define MAX_ITERATIONS 1000000

/*==========================  Function Prototypes ===========================*/
/*
 * Name             : HeapTest_PrintBordersState
 * Description      : Prints the current state of the heap borders including the head and 
 *                    tail of the heap, as well as the values at the head and tail of the 
 *                    simulated heap array.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : This function helps to visualize the boundaries and initial state 
 *                    of the heap during testing.
 */
void HeapTest_PrintBordersState(void);

/*
 * Name             : HeapTest_RandomAllocateFreeTest
 * Description      : Performs a series of random memory allocations and deallocations 
 *                    on the simulated heap, printing the state of the heap and the free 
 *                    list after each operation.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : This function is useful for stress testing the heap manager by 
 *                    repeatedly allocating and freeing memory.
 */
void HeapTest_RandomAllocateFreeTest(void);
 

void VerifyData(sint8* ptr);

#endif