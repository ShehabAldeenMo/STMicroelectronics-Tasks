# Shellio 

## Description
Shellio is a custom command-line shell designed for basic file operations and terminal commands. This shell provides a simple interface for interacting with files and system commands, serving as an educational tool for understanding shell command processing and file manipulation in C.

## Features
+ **Display Current Working Directory:** Use the pwd command to display the current directory path.
+ **File Copying:** Copy files with optional append mode using the cp command.
+ **File Moving:** Move files with optional overwrite mode using the mv command.
+ **Print Messages:** Use the echo command to print messages to the terminal.
+ **Clear Terminal:** Clear the terminal screen with the clear command.
+ **Exit Shell:** Exit the Shellio terminal with the exit command.
+ **Help Command:** Displays information about available commands with help.

## Commands

+ `pwd`
  + Displays the current working directory.

+ `cp <source>,<destination>[,-a]`
  + Copies the file from the source path to the destination path.
  + Use -a to append to the destination file if it exists.
  + If the destination is a directory and the file name is not provided, the source file’s name is used.

+ `mv <source>,<destination>[,-f]`
  + Moves the file from the source path to the destination path.
  + Use -f to forcefully overwrite the destination file if it exists.

+ `echo <message>`
  + Prints the specified message to the terminal.

+ `clear`
`+ Clears the terminal screen.

+ `exit`
`+ Exits the Shellio terminal.

+ `help`
Displays information about available commands.

## Function Descriptions
+ Shellio_GetPath
Retrieves and prints the current working directory using the getcwd system call. If the directory cannot be retrieved, an error message is printed.

+ Shellio_EchoInput
Outputs a given string to the standard output. This simulates the echo command in a shell, printing the input string followed by a newline.

+ Shellio_CopyFile
Copies the content of a source file to a destination file. It supports both appending to the destination file or overwriting it based on global flags. If the move operation flag is set, the source file is deleted after copying.

+ Shellio_FileOption
Sets global flags based on the file operation option provided by the user. This function handles options like append (-a) and forced overwrite (-f) for file operations.

+ Shellio_MoveFile
Sets the global flag for moving a file. This function indicates whether the current operation is a move operation.

+ Shellio_Help
Displays a help menu with information on available commands in Shellio. It prints usage instructions for various Shellio commands.

## Examples
+ Copying a file:
```
  cp /path/to/source.txt,/path/to/destination.txt
```

+ Copying a file with append mode:
```
cp /path/to/source.txt,-a,/path/to/destination.txt
```

+ Moving a file:
```
mv /path/to/source.txt,/path/to/destination.txt
```

+ Moving a file with force overwrite:
```
mv /path/to/source.txt,-f,/path/to/destination.txt
```

+ Printing a message:
```
echo "Hello, Shellio!"

+ Clear Screen:
```
clear
```

+ Exit from Shellio
```
exit
```

+ Print help messags about commands
```
help
```

+ Print Current Working Directory
```
pwd
```

## Video 
You can check the test cases from this link ""
