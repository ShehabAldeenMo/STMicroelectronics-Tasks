/*============================================================================
 * @file name      : main.c
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This program reads and parses partition information from a disk image or
 * partition table file. It handles both GPT (GUID Partition Table) and MBR
 * (Master Boot Record) formats. It prints details about the partitions 
 * in a formatted manner.
=============================================================================
 * @Notes:
 * Ensure that the disk image or partition table file is correctly formatted
 * and accessible. This program assumes specific sector and partition entry
 * sizes based on typical disk structures.
 *
 ******************************************************************************
 ==============================================================================
*/



/*===================================  Includes ==============================*/
#include "Fdisk.h"




/*===================================    Main   ==============================*/
int main(int argc, char **argv) {
    // buffer to store the sectors infotrmations
    char buf[SECTOR_SIZE];

    // ensure that there is path of file descriptor
    if (argc != 2) {
        printf("There is no input device\n");
        return 0;
    }

    // open fd in read only mode
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    // read Sector informations
    if (read(fd, buf, SECTOR_SIZE) != SECTOR_SIZE) {
        perror("Failed to read sector");
        exit(INVALID);
    }

    // to check if that the disk is in GPT format 
    if ((uint8_t)buf[INITIAL_BOOTLOADER+NUM_OF_MBR_PARRTITIONS] == GPT_FORMATE) {
        Gpt_ParseInfo(argv[1],fd);
    }    
    else
    {
        MBR_ParseInfo(argv[1],fd,buf);
    }

    // close file descriptor
    close(fd);

    return 0;
}
