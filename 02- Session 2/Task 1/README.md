# Q&A

**Q1. Write a C program consists of 2 C files, one file contains a function and one or more global variables (used in this function), and the second one contains the main function and uses the function defined in the first file.**

**Commands**
```
$ vim file1.c
$ vim file1.h
$ vim file2.c
$ gcc -static -o Session2_Task1 file1.c file2.c
```

**Illustration**
+ Write into file1.c, file1.h, file2.c. Compile files to generate Session2_Task1 into executable file.
    
![Screenshot from 2024-07-16 19-34-45](https://github.com/user-attachments/assets/2ef68618-cc9e-436e-86c1-dc881dab6c38)

------------------------------------------------------------------------------------------------------------------

**Q2. Take screenshots of the symbol table for the object files and for the generated Executable.**

**Commands**
```
$ gcc -static -c file1.c -o file1.o
$ gcc -static -c file2.c -o file2.o
```

**Illustration**
+ Generate machine code of each file before linker stage.

![Screenshot from 2024-07-16 19-36-31](https://github.com/user-attachments/assets/13e50562-874d-4e28-bd85-bcf2c7f71431)

+ Display the executable file after linker stage of compilation process

![Screenshot from 2024-07-16 19-35-38](https://github.com/user-attachments/assets/8261a0c9-2b26-432a-b9f2-164fb13bc110)

------------------------------------------------------------------------------------------------------------------

**Q3. Get the size of .data and .text sections for the object files and the generated Executable by at least 2
   commands (hint: search for "size" command), and take screenshots of the output of the commands you used
   to get these numbers from.**

**Commands**
```
$ size file1.o
$ size file2.o
$ size Session2_Task1
```

**Illustration** 
+ Generate the size of each section (.text, .bss, .data) in object files.

![Screenshot from 2024-07-16 19-38-58](https://github.com/user-attachments/assets/530643d8-5128-4bf8-ac07-322d7dbfc843)

--------------------------------------------------------------------------------------------------------------------

**Q4. Write a single command to compile a C file and generate all the files produced by the Build process (.i , .s, .o and exec files)**

```
$ gcc -E file1.c -o file1.i && gcc -S file1.c -o file1.s && gcc -c file1.c -o file1.o
$ gcc -E file2.c -o file2.i && gcc -S file2.c -o file2.s && gcc -c file2.c -o file2.o
```
**Illustration**
+ Using previous commend, we will generate all output files from compilation process.

-------------------------------------------------------------------------------------------------------------------

**Q5. For program 1, print the required data as mentioned before, in this format : 
file1) text : number, data: number
file2) text : number, data: number
exec) text : number, data: number**


**commands**
```
$ size file1.o > sizes.txt
$ size file2.o >> sizes.txt
$ size Session2_Task1 >> sizes.txt
$ awk 'NR==2 {print "file1) text: " $1 ", data: " $2}
       NR==4 {print "file2) text: " $1 ", data: " $2}
       NR==6 {print "exec)  text: " $1 ", data: " $2}' sizes.txt
```

**Illustration**
+ Use first command to create sizes.txt file which store the output of size command in.
+ Use awk commend to select specific rows and columns from this file to display (Explaination of awk will be representated in next).

![Screenshot from 2024-07-20 14-44-44](https://github.com/user-attachments/assets/554b1add-b239-43c1-bf78-7729d9f61762)

--------------------------------------------------------------------------------------------------------------------

Explaination of awk:-
---------------------
+ Why do we use awk ?
--> It's used to display some elements in specific format or edit on them.

+ How to use it ?
1. To display the whole file.

> [!NOTE] 
> It's used to refer to speific colume in FileName.txt. By default, space ' ' is regarded as separation between two columns. which $0 refer to whole file $1 refer to column 1 .. etc.

![Screenshot from 2024-07-20 15-16-17](https://github.com/user-attachments/assets/8faf391b-06a4-43f1-998a-6293b74ed675)

2. To display specific column.
+ make random file and use in testing
![Screenshot from 2024-07-20 15-08-02](https://github.com/user-attachments/assets/e588d281-2f00-4bd1-97c0-a678ec8b2eac)

3. Piping
![Screenshot from 2024-07-20 15-15-10](https://github.com/user-attachments/assets/2f06a998-9dce-432c-b3c7-63b584fe72ea)

4. We can select the last column only in file.
![Screenshot from 2024-07-20 15-19-55](https://github.com/user-attachments/assets/fad2d7f3-3478-4213-8eb5-06991b7088d4)

5. We can search on specific word and print this line.
> [!NOTE] 
> word*, means print any line is fill with this begining "word".

![Screenshot from 2024-07-20 15-23-56](https://github.com/user-attachments/assets/b6089b1a-3c7c-4499-9688-44cbbe786749)

6. Change space '   ' to be ':' or any symbol that separate between different columns.
![Screenshot from 2024-07-20 15-29-54](https://github.com/user-attachments/assets/959e5fe9-014c-42b9-a8e7-9d9811b66058)

+ Change the letter of space
![Screenshot from 2024-07-20 15-32-06](https://github.com/user-attachments/assets/dc9c6b9f-45a5-427e-b36e-f0695a51cc04)

7. Change column with word
![Screenshot from 2024-07-20 15-37-12](https://github.com/user-attachments/assets/9e0f12ec-2f00-421a-9ea6-1ba80d900cde)


8. Display Speific line in txt file
![Screenshot from 2024-07-20 15-41-56](https://github.com/user-attachments/assets/9c4a122e-c27e-4b93-9653-faff989b8af3)







