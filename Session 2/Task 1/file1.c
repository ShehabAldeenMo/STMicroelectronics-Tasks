#include <stdio.h>
#include "file2.h" 


int Global_FirsTNumber, Global_SecondNumber;

int GetNumber (){
        printf("Enter first Number = ");
        scanf("%d",&Global_FirsTNumber);
        printf("Enter second Number = ");
        scanf("%d",&Global_SecondNumber);
        return (Global_FirsTNumber+Global_SecondNumber);
}

