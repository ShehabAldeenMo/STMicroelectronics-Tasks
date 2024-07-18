# Q&A
1. Write a C program consists of 2 C files, one file contains a function and one or more global variables
   (used in this function), and the second one contains the main function and uses the function defined in
   the first file.

<Commands>
   
```
$ vim file1.c
$ vim file1.h
$ vim file2.c
$ gcc -static -o Session2_Task1 file1.c file2.c
```
<Description>
+ Write into file1.c, file1.h, file2.c. Compile files to generate Session2_Task1 into executable file.
    
![Screenshot from 2024-07-16 19-34-45](https://github.com/user-attachments/assets/2ef68618-cc9e-436e-86c1-dc881dab6c38)

2. Take screenshots of the symbol table for the object files and for the generated Executable.
<Commands>
```
$ gcc -static -c file1.c -o file1.o
$ gcc -static -c file2.c -o file2.o
```

<Description>
+ Generate machine code of each file before linked the in linker stage.

![Screenshot from 2024-07-16 19-36-31](https://github.com/user-attachments/assets/13e50562-874d-4e28-bd85-bcf2c7f71431)

+ Display the executable file after linker stage of compilation process

![Screenshot from 2024-07-16 19-35-38](https://github.com/user-attachments/assets/8261a0c9-2b26-432a-b9f2-164fb13bc110)


3. Get the size of .data and .text sections for the object files and the generated Executable by at least 2
   commands (hint: search for "size" command), and take screenshots of the output of the commands you used
   to get these numbers from.

```
$ size file1.o
$ size file2.o
$ size Session2_Task1
```

<Description>
+ Generate the size of each section in object files.

![Screenshot from 2024-07-16 19-38-58](https://github.com/user-attachments/assets/530643d8-5128-4bf8-ac07-322d7dbfc843)
