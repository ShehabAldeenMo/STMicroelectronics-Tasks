#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>


/*===================================  Functions ==========================================*/
void Print_ElfHeader(const char* FilePath);          // print elf header
void Print_SectionHeader(const char* FilePath);      // print section header
void ChooseTable (const char* FilePath);             // choose which header table needed

/*====================================  main ============================================== */
int main (int argc, char *argv[]){
        const char* loc_FilePath = argv[1] ;

	/*To check on inputs */
	if (argc != 2){
		printf ("Error in entering file path");
		return 0 ;
	}

	/* To choose the option table in this code */
	ChooseTable(loc_FilePath);

	return 0 ;
}

/*===================================== Functions Implementations ======================================= */
void ChooseTable (const char* FilePath){
	/* buffering user input */
	char option ; 

	/* user dashboard */
        printf("\nElf Header enter           --> 1 \nSection Header Table enter --> 2\nChoose : ");
        scanf("%c",&option);
        printf("-----------------------------------------------------------------------------------\n");


	/* check on inout */
        if (option == '1'){
                Print_ElfHeader(FilePath);
        }
        else if (option == '2' ){
                Print_SectionHeader (FilePath);
        }
        else{
                printf("Incorrect Input\n");
        }
	printf("\n");
}


void Print_SectionHeader(const char* FilePath){
	/* Open file */
        FILE* file = fopen(FilePath,"r");

        /* Check if isn't opened  */
        if (file == NULL){
                printf("Error In Opening file");
                return;
        }

	/* structure of elf header*/
        Elf64_Ehdr ElfHeader;

        /* Because of elf header is allocated at the begining of elf file so we don't need to make offest in reading this header from
         * file  */
        size_t ret = fread(&ElfHeader, 1 , sizeof(ElfHeader), file);

        if (ret != sizeof (ElfHeader)){
                printf("Error in reading file");
                fclose(file);
                return ;
        }

	printf("There are %d section headers, starting at offset 0x%lx:\n\n",ElfHeader.e_shnum,ElfHeader.e_shoff);

	/*Go to the section header table by (base_address+ offset)*/
    	fseek(file, ElfHeader.e_shoff, SEEK_SET);

	/* Get copy of all section headers */
	Elf64_Shdr sectionHeaders[ElfHeader.e_shnum] ;
        ret = fread(&sectionHeaders, ElfHeader.e_shnum , sizeof(Elf64_Shdr), file);

	/* To check that the number of read bytes from elf file equals the disered number */
        if (ret != sizeof(Elf64_Shdr) ){
                printf("Error in Reading Section Header Table");
                fclose(file);
                return ;
        }

	/* Read Section Header String Table and allocate memory for it into memory to ease mapping on table and get sections names */
	Elf64_Shdr shStrTabHeader = sectionHeaders[ElfHeader.e_shstrndx];
    	char* shStrTab = malloc(shStrTabHeader.sh_size);
	
	/* To take single copy of section header string table  */
	fseek(file, shStrTabHeader.sh_offset, SEEK_SET);
    	ret = fread(shStrTab, 1, shStrTabHeader.sh_size, file);
    	
	/* To check that the file is read */
	if (ret != shStrTabHeader.sh_size) {
        	printf("Error in reading section header string table");
        	fclose(file);
        	return;
    	}

	/* Print section header */
	printf ("Section Headers:\n[Nr] Name              Type             Address           Offset\n     Size              EntSize          Flags  Link  Info  Align \n");

	    for (int i = 0; i < ElfHeader.e_shnum; i++) {
       	       printf("[%2d] %-17s  %-15x  %016lx  %08lx\n",
               i, &shStrTab[sectionHeaders[i].sh_name],
               sectionHeaders[i].sh_type, sectionHeaders[i].sh_addr,
               sectionHeaders[i].sh_offset);
     	       printf("     %016lx  %016lx  %c%c%c%c%c  %3d  %3d  %3lu\n",
               sectionHeaders[i].sh_size, sectionHeaders[i].sh_entsize,
               (sectionHeaders[i].sh_flags & SHF_WRITE) ? 'W' : ' ',
               (sectionHeaders[i].sh_flags & SHF_ALLOC) ? 'A' : ' ',
               (sectionHeaders[i].sh_flags & SHF_EXECINSTR) ? 'X' : ' ',
               (sectionHeaders[i].sh_flags & SHF_MERGE) ? 'M' : ' ',
               (sectionHeaders[i].sh_flags & SHF_STRINGS) ? 'S' : ' ',
               sectionHeaders[i].sh_link, sectionHeaders[i].sh_info,
               sectionHeaders[i].sh_addralign);
    }

	printf("Key to Flags:\nW (write), A (alloc), X (execute), M (merge), S (strings), I (info),\nL (link order), O (extra OS processing required), G (group), T (TLS),\nC (compressed), x (unknown), o (OS specific), E (exclude),\nD (mbind), l (large), p (processor specific)");

    /* Free allocated memory and close the file */
    free(shStrTab);
    fclose(file);
}

void Print_ElfHeader(const char* FilePath){
	/* Open file */
	FILE* file = fopen(FilePath,"r");
	
	/* Check if isn't opened  */
	if (file == NULL){
		printf("Error In Opening file");
		return;
	}

	/* structure of elf header*/
	Elf64_Ehdr ElfHeader;

	/* Because of elf header is allocated at the begining of elf file so we don't need to make offest in reading this header from
	 * file  */
	size_t ret = fread(&ElfHeader, 1 , sizeof(ElfHeader), file);

	if (ret != sizeof (ElfHeader)){
		printf("Error in reading file");
		fclose(file);
		return ;
	}

	/* Close file because we are already read the elf header */
	fclose (file);

	printf("ELF Header:\n");
    	printf("  Magic:   ");
    	for (int i = 0; i < EI_NIDENT; i++){
        	printf("%02x ", ElfHeader.e_ident[i]);
	}
    	printf("\n");

	printf("  Class:                             ");
    	switch (ElfHeader.e_ident[EI_CLASS]) {
        	case ELFCLASS32: printf("ELF32\n"); break;
        	case ELFCLASS64: printf("ELF64\n"); break;
        	default: printf("Invalid class\n"); break;
    	}

	printf("  Version:                           %d (current)\n", ElfHeader.e_ident[EI_VERSION]);
   	printf("  OS/ABI:                            ");
    	switch (ElfHeader.e_ident[EI_OSABI]) {
        	case ELFOSABI_SYSV: printf("UNIX - System V\n"); break;
       	 	case ELFOSABI_HPUX: printf("UNIX - HP-UX\n"); break;
        	case ELFOSABI_NETBSD: printf("UNIX - NetBSD\n"); break;
        	case ELFOSABI_LINUX: printf("UNIX - Linux\n"); break;
        	default: printf("Unknown\n"); break;
    	}

	printf("  ABI Version:                       %d\n", ElfHeader.e_ident[EI_ABIVERSION]);
    	printf("  Type:                              ");
    	switch (ElfHeader.e_type) {
        	case ET_NONE: printf("No file type\n"); break;
        	case ET_REL: printf("Relocatable file\n"); break;
        	case ET_EXEC: printf("Executable file\n"); break;
        	case ET_DYN: printf("Shared object file\n"); break;
        	case ET_CORE: printf("Core file\n"); break;
        	default: printf("Unknown\n"); break;
    	}

    	printf("  Machine:                           ");
    	switch (ElfHeader.e_machine) {
        	case EM_386: printf("Intel 80386\n"); break;
        	case EM_X86_64: printf("AMD x86-64\n"); break;
       	default: printf("Unknown\n"); break;
    	}

    	printf("  Version:                           0x%x\n", ElfHeader.e_version);
    	printf("  Entry point address:               0x%lx\n", ElfHeader.e_entry);
    	printf("  Start of program headers:          %ld (bytes into file)\n", ElfHeader.e_phoff);
    	printf("  Start of section headers:          %ld (bytes into file)\n", ElfHeader.e_shoff);
    	printf("  Flags:                             0x%x\n", ElfHeader.e_flags);
    	printf("  Size of this header:               %d (bytes)\n", ElfHeader.e_ehsize);
    	printf("  Size of program headers:           %d (bytes)\n", ElfHeader.e_phentsize);
    	printf("  Number of program headers:         %d\n", ElfHeader.e_phnum);
    	printf("  Size of section headers:           %d (bytes)\n", ElfHeader.e_shentsize);
    	printf("  Number of section headers:         %d\n", ElfHeader.e_shnum);
    	printf("  Section header string table index: %d\n", ElfHeader.e_shstrndx);
}
