/*===================================  Includes ==========================================*/
#include "Commands.h"

/*==============================  Functions Implementations ===============================*/
void Shellio_GetPath(){
    /* Buffer of path */
    uint8 cwd[PATH_MAX];

    /* to get our working directory */
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}

void Shellio_EchoInput(const char* Copy_Statment){
    printf("%s\n",Copy_Statment);
}

void Shellio_CopyFile (const char* Copy_1st_Path,const char* Copy_2nd_Path ){
	/* Open file */
    FILE* SrcFile = fopen(Copy_1st_Path,"r");
    FILE* DesFile = fopen(Copy_2nd_Path,"wb");

    /* Check if isn't opened  */
    if (SrcFile == NULL || DesFile == NULL ){
        perror("fopen() error");
        return;
    }

    /* structure of elf header*/
    uint8 Buffer[MAX_COPIED_CONTENT];

    /* reading file */
    size_t ret_Size = fread(Buffer, 1 , sizeof(Buffer), SrcFile) ;
    while ( ret_Size > 0 ){
        fwrite(Buffer, 1, ret_Size, DesFile);
        ret_Size = fread(Buffer, 1 , sizeof(Buffer), SrcFile) ;
    }

    /* Close both of files */
    fclose(SrcFile);
    fclose(DesFile);
}