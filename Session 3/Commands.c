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
    /* Try to open source and destination files */
    FILE* SrcFile = fopen(Copy_1st_Path,"r");
    FILE* DesFile = fopen(Copy_2nd_Path,"wb"); // if the user give file name that didn't exist. "wb" is attribute to create this name. 

    /* To use these buffers in creation destintion file with the same name of source */
    char SrcFileName[MAX_FILE_NAME];
    char ConcatenatedDesFile [PATH_MAX];

    /* Terminate this operation if the source file isn't existed */
    if (SrcFile == NULL ){
        perror("Src fopen() error");
        return;
    }

    /* If the destination file name didn't pass or directory is unfound */
    if (DesFile == NULL ){
        /* copy base name of source file to create destination file with the same name */
        strcpy (SrcFileName, basename(strdup(Copy_1st_Path) ) ) ;  // Use strdup to avoid modifying the original path
        snprintf(ConcatenatedDesFile, PATH_MAX, "%s/%s", Copy_2nd_Path, SrcFileName);
        DesFile = fopen(ConcatenatedDesFile,"wb");

        /* Terminate this operation if the destination file name is created but the Concatenated path of directory isn't correct */
        if (DesFile == NULL ){
            printf("Given path of directory isn't correct\n");
            perror("Destination fopen() error");
            return;
        }

    }

    /* Buffering copied info from source file  */    
    uint8 Buffer[MAX_COPIED_CONTENT];

    /* read source file content */
    size_t ret_Size = fread(Buffer, 1 , sizeof(Buffer), SrcFile) ;

    /* write into destination file */
    while ( ret_Size > 0 ){
        fwrite(Buffer, 1, ret_Size, DesFile);
        ret_Size = fread(Buffer, 1 , sizeof(Buffer), SrcFile) ;
    }

    fclose(SrcFile);
    fclose(DesFile);
}