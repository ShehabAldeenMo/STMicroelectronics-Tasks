/*============================================================================
 * @file name      : main.c
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This file contains the main function for running the heap memory tests.
 * It starts the random allocation and deallocation test to evaluate the 
 * functionality and robustness of the heap manager.
 *
 =============================================================================
 * @Notes:
 * - Ensure that the `HeapTest.h` file is correctly included and that the
 *   `random_alloc_free_test` function is defined in the `HeapTest.c` file.
 * - Update the include path if necessary based on your project directory structure.
 *
 ******************************************************************************
 ==============================================================================
*/


/*===================================  Includes ===============================*/
#include "Level_1/HeapTest.h"

extern uint32 Fail ;

/*==================================  main =====================================*/
int main (){
    printf("Starting random allocation and deallocation test...\n");
    HeapTest_RandomAllocateFreeTest();
    printf("Test complete.\n");
    printf("Fails = %f\n", ((float)Fail/MAX_ITERATIONS)*100 );
    return 0 ;
}
