# Shellio 
## 1. Description
Shellio is a simple, custom command-line shell designed to interact with users through a terminal interface. It provides a basic environment for entering and processing commands. Here’s an overview of its features and functionality:

**1.1 Prompt Display:**
Shellio displays a prompt (Shellio > ) to indicate that it is ready to accept user input.

**1.2 Command Input:**
Users can input commands directly into the shell. The commands can include various text entries separated by spaces as delimiter.

**1.3 Command Parsing:**
Shellio uses strtok() to tokenize the input string based on predefined delimiters. This allows it to break down the input into individual commands or arguments.

**1.4 Exit Command:**
The shell recognizes the command "exit" as a signal to terminate the shell session. When the user inputs "exit", Shellio will exit its loop and show "Good Bye" then terminate.

**1.5 Error Handling:**
Shellio uses fgets() to safely read user input, handling errors by printing a descriptive message if input reading fails.

**1.6 Buffer Management:**
After processing the input, Shellio clears the buffer to prepare for the next command, ensuring that residual data from previous inputs does not interfere with new commands.
