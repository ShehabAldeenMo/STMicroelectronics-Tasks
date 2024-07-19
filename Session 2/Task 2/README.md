# Task 2 
## Description 
Write an ELF parser application to parse an ELF file and print its header content, to be similar to "readelf -h" command.
Bonus (optional) List the name, size and type of all the ELF sections (if exist), you can make it as a separate program or you can combine the two programs (elf header and section headers parser) in one program and the user can choose between them using options (hint you can use getopt())

## Example 
```
$ readelf -h file1.o
```
**The output**
![Screenshot from 2024-07-19 18-51-28](https://github.com/user-attachments/assets/f4fe9417-e5b3-415e-8dc7-b54e22a0e574)

Steps:-
------
+ We will create "ElfParser.c" 
  
