# Shellio

## Table of Contents

- [Description](#description)
- [Features](#features)
- [Commands](#commands)
- [Function Descriptions](#function-descriptions)
- [Examples](#examples)
- [Build Instruction](#build-instruction)
- [GDB Script](#gdb-script)


## Description
Shellio is a custom command-line shell designed for basic file operations and terminal commands. This shell provides a simple interface for interacting with files and system commands, serving as an educational tool for understanding shell command processing and file manipulation in C.

## Features
+ **Display Current Working Directory:** Use the `path` command to display the absolute path of the current working directory.
+ **Clear Terminal Screen:** Clear the terminal screen with the `cls` command.
+ **Print Messages:** Use the `display` command to print messages to the terminal.
+ **Exit Shell:** Exit the Shellio terminal with the `leave` command.
+ **Help Command:** Displays information about available commands with the `assist` command.
+ **File Copying:** Copy files with the `clone` command, including handling file copying with optional append mode.
+ **File Moving:** Move files with the `shift` command, which also supports file copying.
+ **Change Directory:** Change the current working directory with the `cd` command.
+ **Command Type Checking:** Check if a command is built-in or external using the `type` command.
+ **Environment Variables:** Print all environment variables with the `envir` command or a specific variable with `envir <varname>`.
+ **Process History:** Print the history of commands executed in the session with the `phist` command.
+ **External Command Execution:** Execute external commands not built into Shellio.

## Commands

+ `path`
  + Displays the absolute path of the current working directory.

+ `cls`
  + Clears the terminal screen.

+ `display <message>`
  + Prints the specified message to the terminal.

+ `leave`
  + Exits the Shellio terminal.

+ `assist`
  + Displays information about available commands in Shellio.

+ `clone "source" "destination"[-a]`
  + Copies the file from the source path to the destination path.
  + Use `-a` to append to the destination file if it exists.

+ `shift "source" "destination"[-f]`
  + Moves the file from the source path to the destination path.
  + Use `-f` to forcefully overwrite the destination file if it exists.

+ `cd <directory>`
  + Changes the current working directory to the specified directory.

+ `type <command>`
  + Checks if the given command is built-in or external.

+ `envir [varname]`
  + Prints all environment variables if no variable name is provided.
  + Prints the value of the specified environment variable if a variable name is given.

+ `phist`
  + Displays the history of commands executed in the current session.

## Function Descriptions
+ **Shellio_GetPath:**
  Retrieves and prints the current working directory using the `getcwd` system call. If the directory cannot be retrieved, an error message is printed.

+ **Shellio_EchoInput:**
  Outputs a given string to the standard output. This simulates the `display` command in a shell, printing the input string followed by a newline.

+ **Shellio_Copy:**
  Copies the content of a source file to a destination file. It supports appending to the destination file or overwriting it based on provided options.

+ **Shellio_MoveFile:**
  Moves the file from the source path to the destination path. It supports forceful overwriting of the destination file if specified.

+ **Shellio_Help:**
  Displays a help menu with information on available commands in Shellio. It prints usage instructions for various Shellio commands.

+ **Shellio_Clear:**
  Clears the terminal screen.

+ **Shellio_Exit:**
  Exits the Shellio terminal.

+ **Shellio_ChangeDir:**
  Changes the current working directory to the specified path.

+ **Shellio_TypeCommand:**
  Checks if a given command is built-in or external, and prints the result.

+ **Shellio_PrintEnv:**
  Prints all environment variables.

+ **Shellio_PrintEnvVar:**
  Prints the value of a specific environment variable if it exists.

+ **Shellio_Phist:**
  Prints the history of commands executed during the session.

+ **Shellio_ExecExternalCommands:**
  Executes external commands by searching for them in the directories listed in the PATH environment variable.

## Examples
+ Displaying the current working directory:
```
  path
```

+ Clear the terminal screen:
```
cls
```

+ Print a message to the terminal:
```
display Hello, Shellio!
```

+ Exit the Shellio terminal:
```
leave
```

+ Show information about available commands:
```
assist
```

+ Copy a file:
```
clone "/path/to/source.txt" "/path/to/destination.txt"
```

+ Copy a file with append mode:
```
clone "/path/to/source.txt" -a "/path/to/destination.txt"
```

+ Move a file:
```
shift "/path/to/source.txt" "/path/to/destination.txt"
```

+ Move a file with force overwrite:
```
shift "/path/to/source.txt" -f "/path/to/destination.txt" 
```

+ Change the current working directory:
```
cd /path/to/new/directory
```

+ Check if a command is built-in or external:
```
type command_name
```

+ Print all environment variables:
```
envir
```

+ Print a specific environment variable:
```
envir VAR_NAME
```

+ Print the process history:
```
phist
```

+ Execute an external command:
```
some_external_command
```

## Build Instruction 
To build the Shellio project, please follow the steps below. This section outlines the necessary dependencies, commands, and configurations required for a successful build.

**Build Steps**
1. Clone the Repository:
First, clone the repository to your local machine using Git:
```
git clone https://github.com/ShehabAldeenMo/Shellio.git
cd Shellio
```

2. Compilation:
Compile the project using GCC. Navigate to the project directory and use the following command:
```
gcc -o shellio Shellio.c Commands.c
```

3. Running the Shell:
After successful compilation, you can run the shell by executing:
```
./shellio
```

**Configuration**
No specific configuration is required for this project. However, ensure that you have the necessary permissions to execute and access the required files.

> [!NOTE] 
> If you encounter any issues during the build process, please check that all dependencies are correctly installed and that your compiler version is compatible with the project code.

## GDB Script
**Parse Command** The purpose of this GDB script is to parse and print the contents of the sharedString variable, character by character, in a running	program.

## Video 
For more information, refer to the [ShellioExtension](https://drive.google.com/file/d/1Z_O5qnkvb15eSDk8xQGERKtIijxOo8D2/view?usp=sharing) video.
For GDB usage refer to [GDB Task](https://drive.google.com/file/d/1VSCTlDdLTB5_fs3Q54X4z_g7njjMshai/view?usp=sharing) video.
