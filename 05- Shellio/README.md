# Shellio

## Table of Contents

- [Description](#description)
- [Layered Diagram](#layered-diagram)
- [Features](#features)
- [Commands](#commands)
- [Function Descriptions](#function-descriptions)
- [Examples](#examples)
- [Build Instruction](#build-instruction)
- [Video](#video)

## Description
Shellio is a custom command-line shell designed for basic file operations and terminal commands. This shell provides a simple interface for interacting with files and system commands, serving as an educational tool for understanding shell command processing and file manipulation in C.

## Layered Diagram
![Screenshot from 2024-08-12 01-14-02](https://github.com/user-attachments/assets/0787465a-a80f-4121-95b8-e9cbc2968b7a)

## Features
- **Display Current Working Directory:** Use the `path` command to display the absolute path of the current working directory.
- **Clear Terminal Screen:** Clear the terminal screen with the `cls` command.
- **Print Messages:** Use the `display` command to print messages to the terminal.
- **Exit Shell:** Exit the Shellio terminal with the `leave` command.
- **Help Command:** Displays information about available commands with the `assist` command.
- **File Copying:** Copy files with the `clone` command, including handling file copying with optional append mode.
- **File Moving:** Move files with the `shift` command, which also supports file copying.
- **Change Directory:** Change the current working directory with the `cd` command.
- **Command Type Checking:** Check if a command is built-in or external using the `type` command.
- **Environment Variables:** Print all environment variables with the `envir` command or a specific variable with `envir <varname>`.
- **Process History:** Print the history of commands executed in the session with the `phist` command.
- **External Command Execution:** Execute external commands not built into Shellio.
- **Advanced Redirection and Piping:** Support for various combinations of input, output, and error redirection, as well as piping between commands.

## Commands

- `path`
  - Displays the absolute path of the current working directory.
  - Redirects output and errors:
    - `path > "file.txt"`
    - `path 2> "file.txt"`
    - `path > "file.txt" 2> "file2.txt"`
    - `path 2> "file.txt" > "file2.txt"`

- `cls`
  - Clears the terminal screen.

- `display <message>`
  - Prints the specified message to the terminal.
  - Examples of redirection and piping:
    - `display She hab > "file.txt"`
    - `display She hab 2> "file.txt"`
    - `display She hab > "file.txt" 2> "file2.txt"`
    - `display She hab 2> "file.txt" > "file2.txt"`
    - `display < "file3.txt"`
    - `display < "file3.txt" > "file.txt" 2> "file2.txt"`
    - `display < "file3.txt" 2> "file.txt" > "file2.txt"`
    - `display 2> "file.txt" > "file2.txt" < "file3.txt"`

- `leave`
  - Exits the Shellio terminal.

- `assist`
  - Displays information about available commands in Shellio.

- `clone "source" -a "destination"`
  - Copies the file from the source path to the destination path.
  - Use `-a` to append to the destination file if it exists.

- `shift "source" -f "destination"`
  - Moves the file from the source path to the destination path.
  - Use `-f` to forcefully overwrite the destination file if it exists.

- `cd <directory>`
  - Changes the current working directory to the specified directory.
  - Examples:
    - `cd ../`
    - `cd 5- Shellio`

- `type <command>`
  - Checks if the given command is built-in or external.
  - Examples of redirection:
    - `type cd 2> "file.txt" > "file2.txt"`
    - `type cd > "file.txt" 2> "file2.txt"`
    - `type < "file3.txt" > "file.txt" 2> "file2.txt"`
    - `type 2> "file.txt" > "file2.txt" < "file3.txt"`

- `envir [varname]`
  - Prints all environment variables if no variable name is provided.
  - Prints the value of the specified environment variable if a variable name is given.
  - Examples of redirection:
    - `envir PWD 2> "file.txt" > "file2.txt"`
    - `envir PWD > "file.txt" 2> "file2.txt"`
    - `envir < "file3.txt" 2> "file.txt" > "file2.txt"`
    - `envir > "file.txt" 2> "file2.txt" < "file3.txt"`

- `phist`
  - Displays the history of commands executed in the current session.
  - Example of redirection:
    - `phist > "file.txt"`

- `allVar`
  - Print all shell variables and functions.

- `ls -l`
  - List directory contents in long format.
  - Examples of redirection and piping:
    - `ls -l > "file.txt"`
    - `ls -l 2> "file.txt"`
    - `ls -l 2> "file.txt" > "file2.txt"`
    - `ls -l > "file.txt" 2> "file2.txt"`
    - `ls -l | grep ".txt"`
    - `ls -l | grep ".txt" > "file.txt"`

- `uptime 2> "file.txt"`
  - Redirects errors from the `uptime` command to a file.

- `free > "file.txt"`
  - Redirects output of the `free` command to a file.

- `grep "shehab" non_existent_file.txt 2> "file.txt"`
  - Redirects errors from the `grep` command to a file.

- `display Shehab | grep "She"`
  - Pipes the output of `display` to `grep`.

- `ls -l | grep ".txt" | sort`
  - Chains multiple commands together using pipes.

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
gcc -o shellio Shellio.c Commands.c Helper.c Utility.c
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


## Video 
For more information, refer to the [ShellioExtension](https://drive.google.com/file/d/1DLD2KaxgyIb-XGvdjJUe9hwI7Du5w_BR/view?usp=sharing) video. [Top Command](https://drive.google.com/file/d/1u3KhDqusk7k4APHgFe96Ub4sGcGf5L7j/view?usp=drive_link).
