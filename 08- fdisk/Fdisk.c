/*============================================================================
 * @file name      : Fdisk.c
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This file contains functions for handling and displaying partition information
 * from disk images or partition tables. It supports both GPT and MBR formats.
=============================================================================
 * @Notes:
 *
 ******************************************************************************
 ==============================================================================
*/



/*===================================  Includes ==============================*/
#include "Fdisk.h"



/*============================= Global array of struct =======================*/
static GPT_PartitionType GptPartitionTypes[] = {
    {"EBD0A0A2-B9E5-4433-87C0-68B6B72699C7", "Microsoft basic data"},
    {"C12A7328-F81F-11D2-BA4B-00A0C93EC93B", "EFI System"},
    {"E3C9E316-0B5C-4DB8-817D-F92DF00215AE", "Microsoft reserved"},
    {"DE94BBA4-06D1-4D40-A16A-BFD50179D6AC", "Windows recovery environment"},
    {"0FC63DAF-8483-4772-8E79-3D69D8477DE4", "Linux filesystem"},
    {"A19D880F-05FC-4D3B-A006-743F0F84911E", "Linux swap"},
    {"48465300-0000-11AA-AA11-00306543ECAC", "Apple HFS+"},
    {"426F6F74-0000-11AA-AA11-00306543ECAC", "Apple Boot"},
    {NULL, NULL}  
};

static MBR_PartitionType MbrPartitionTypes[] = {
    {0x01,"FAT12"},
    {0x05,"Extended"},
    {0x06,"FAT16"},
    {0x07,"NTFS or exFAT"},
    {0x83,"Linux"},
    {0xF4,"NTFS"},
    {0xFE,"EFI GPT"},
    {0x0B,"W95 FAT32"},
    {0x00,NULL}
};




/*=========================== Functions Implementations ======================*/
void Gpt_ParseInfo(char* path,int fd){
    // buffer to store the sectors infotrmations
    char buf[SECTOR_SIZE];

    printf("%-10s%-10s %-10s %-10s %-15s %-40s\n",
           "Partition", "Start", "End", "Sectors", "Size(MB)", "Type");

    // Move the file descriptor's position by SECTOR_SIZE bytes forward from the current position.
    lseek(fd,SECTOR_SIZE, SEEK_SET); // first 512 bytes is used as protected MBR
    if (read(fd, buf, SECTOR_SIZE) != SECTOR_SIZE) {
        // Fails to read complete sector
        perror("Failed to read sector");
        exit(INVALID);
    }

    // Structure to hold partition entry data
    GPT_PartitionEntry entry;

    // Partition number counter
    uint8_t PartitionNum = 1;

    // loop on each parition header
    for (uint8_t i = 0; i < GPT_ENTRIES_NUM; i++) {
        lseek(fd,(SECTOR_SIZE*GPT_OFFEST_HEADER + i * GPT_ENTRY_SIZE), SEEK_SET);
        if (read(fd, &entry, GPT_ENTRY_SIZE) != GPT_ENTRY_SIZE) {
            // Fails to read complete sector
            perror("Failed to read sector");
            exit(INVALID);
        }
        // print partition informations
        Gpt_PrintPartitionInfo(path, PartitionNum++, &entry);
    }
}


void Gpt_PrintPartitionInfo(char *partition, uint8_t num, GPT_PartitionEntry *entry) {
    if (entry->first_lba == 0 && entry->end_lba == 0) 
        return;  

    char guid[ GPT_GUID_TYPE_LENGTH + 1 ];  
    Gpt_ReverseGUID(entry->type, guid);
    const char *type = GPT_GetPartitionTypeName(guid);

    // Concatenate partition and num
    char result[50];  // Adjust size as needed
    sprintf(result, "%s%d", partition, num);

    printf("%-10s%-10lu %-10lu %-10lu %-15lu %-40s \n",
           result,(uint64_t)entry->first_lba,(uint64_t)entry->end_lba,
           (uint64_t)(entry->end_lba - entry->first_lba + 1),
           (uint64_t)((entry->end_lba - entry->first_lba + 1) * SECTOR_SIZE / (_1MB)),type);
}

void Gpt_ReverseGUID(const uint8_t *type_guid, char *guid) {
    sprintf(guid,"%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            type_guid[3], type_guid[2], type_guid[1], type_guid[0],                   /* Reversed byte order */
            type_guid[5], type_guid[4],                                               /* Reversed byte order */
            type_guid[7], type_guid[6],                                               /* Reversed byte order */
            type_guid[8], type_guid[9],                                               /* Keep original order */
            type_guid[10], type_guid[11], type_guid[12], type_guid[13], type_guid[14], type_guid[15]);    /* Keep original order */
}


const char* GPT_GetPartitionTypeName(const char *type) {
    uint8_t i = 0;
    while ( GptPartitionTypes[i].guid != NULL) {
        if (strncmp(GptPartitionTypes[i].guid, type, strlen(GptPartitionTypes[i].guid)) == VALID) {
            return GptPartitionTypes[i].name;
        }
        i++;
    }
    return "Unresolved Type";
}




/* Parse MBR informations */
void MBR_ParseInfo(char* path, int fd,char* buf){
    // Initialize the partition number to 1
    uint8_t PartitionNum = 1;

    // Initialize the starting index for logical partitions in the EBR (Extended Boot Record)
    uint8_t Ebr_StartIndex = 5;

    // Print the header for partition information
    printf("%-10s%-5s %-10s %-10s %-10s %-10s %-5s %-5s\n", "Partition", "Boot", "Start", "End", "Sectors", "Size(MB)", "Id", "Type");

    // Cast the buffer pointer to an MBR_PartitionEntry pointer, starting at the initial bootloader section
    MBR_PartitionEntry *entry = ( MBR_PartitionEntry* )&buf[INITIAL_BOOTLOADER];

    // Loop through the four primary partition entries in the MBR
    for (uint8_t i = 0; i < NUM_OF_MBR_PARRTITIONS ; i++) {

        // Check if the partition has a valid LBA (Logical Block Address) - if not, it's unused
        if (entry[i].lba != 0) 
        {
            // Parse and display partition information
            MBR_PrintPartitionInfo(path, PartitionNum++, &entry[i], 0);

            // Check if the current partition is an extended partition type
            if (entry[i].partition_type == CHS_EXTENDED_PARTITION     || 
                entry[i].partition_type == LBA_EXTENDED_PARTITION     || 
                entry[i].partition_type == LINUX_EXTENDED_PARTITION)  {
    
                    // If it's an extended partition, parse its extended boot record (EBR)
                    MBR_ParseEbr(fd, path, entry[i].lba, &Ebr_StartIndex);
            }
        }
    }
}

void MBR_PrintPartitionInfo(char *partition, uint8_t num, MBR_PartitionEntry *entry, uint32_t base_lba){
    if (entry->lba == 0 && entry->sector_count == 0) 
    {
        /* Empty Partition => skip */
        return;  
    }

    // Concatenate partition and num
    char result[50];  // Adjust size as needed
    sprintf(result, "%s%d", partition, num);

    printf("%-10s%-5c %-10u %-10u %-10u %-10u %-5X %-5s\n",
           result, entry->status == 0x80 ? '*' : ' ',
           (base_lba + entry->lba), (base_lba + entry->lba + entry->sector_count - 1) ,
           entry->sector_count,
           (uint32_t)(((uint64_t)entry->sector_count * SECTOR_SIZE) / _1MB),
           entry->partition_type,
           MBR_GetPartitionTypeName(entry->partition_type)); 
}


const char* MBR_GetPartitionTypeName(uint8_t type){
    uint8_t i = 0;
    while ( MbrPartitionTypes[i].name != NULL) {
        if ( (MbrPartitionTypes[i].id - type) == VALID) {
            return MbrPartitionTypes[i].name;
        }
        i++;
    }
    return "Unresolved Type";
}

void MBR_ParseEbr(int fd, char *partition, uint32_t ebr_start, uint8_t *num) {
    char buf[SECTOR_SIZE];
    MBR_PartitionEntry *entry;
    uint32_t current_ebr_block = ebr_start;

    while (current_ebr_block != 0) {
        /* Move the file descriptor to the current EBR block */
        lseek(fd, current_ebr_block * SECTOR_SIZE, SEEK_SET);
        read(fd, buf, SECTOR_SIZE);

        /* The first partition entry in the EBR points to the logical partition */
        entry = (MBR_PartitionEntry *)&buf[INITIAL_BOOTLOADER];

        /* Print the information of the current logical partition */
        MBR_PrintPartitionInfo(partition, *num, entry, current_ebr_block);
        (*num)++;

        /* The second partition entry points to the next EBR */
        if (entry[1].lba != 0) {
            current_ebr_block = ebr_start + entry[1].lba;
        } else {
            /* No more logical partitions */
            break;
        }
    }
}
