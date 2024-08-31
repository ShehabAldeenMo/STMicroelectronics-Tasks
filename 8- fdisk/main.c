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
    if ((uint8_t)buf[450] == GPT_FORMATE) {
        printf("%-10s%-10s %-10s %-10s %-15s %-40s\n",
               "Partition", "Start", "End", "Sectors", "Size(MB)", "Type");

        // Move the file descriptor's position by SECTOR_SIZE bytes forward from the current position.
        lseek(fd,SECTOR_SIZE, SEEK_SET);
        if (read(fd, buf, SECTOR_SIZE) != SECTOR_SIZE) {
            // Fails to read complete sector
            perror("Failed to read sector");
            exit(INVALID);
        }

        // Structure to hold partition entry data
        GPT_PartitionEntry entry;
        // Partition number counter
        uint8_t PartitionNum = 1;

        for (uint8_t i = 0; i < GPT_ENTRIES_NUM; i++) {
            lseek(fd,(SECTOR_SIZE*GPT_OFFEST_HEADER + i * GPT_ENTRY_SIZE), SEEK_SET);
            if (read(fd, &entry, GPT_ENTRY_SIZE) != GPT_ENTRY_SIZE) {
                // Fails to read complete sector
                perror("Failed to read sector");
                exit(INVALID);
            }

            // print partition informations
            Gpt_PartitionInfo(argv[1], PartitionNum++, &entry);
        }
    }    
    else 
    {
        uint8_t PartitionNum = 1;
        uint8_t Ebr_StartIndex = 5;  // Start logical partitions at index 5

        printf("%-10s%-5s %-10s %-10s %-10s %-10s %-5s %-5s\n", "Partition", "Boot", "Start", "End", "Sectors", "Size(MB)", "Id", "Type");

        MBR_PartitionEntry *entry = ( MBR_PartitionEntry* )&buf[INITIAL_BOOTLOADER];
        for (uint8_t i = 0; i < NUM_OF_MBR_PARRTITIONS ; i++) {
            if (entry[i].lba != 0) 
            {
                MBR_PartitionInfo(argv[1], PartitionNum++, &entry[i], 0);

                // Check on extended partition
                if (entry[i].partition_type == CHS_EXTENDED_PARTITION     || 
                    entry[i].partition_type == LBA_EXTENDED_PARTITION     || 
                    entry[i].partition_type == LINUX_EXTENDED_PARTITION)  {
                    MBR_ParseEbr(fd, argv[1], entry[i].lba, &Ebr_StartIndex);
                }
            }
        }
    }

    // close file descriptor
    close(fd);

    return 0;
}
