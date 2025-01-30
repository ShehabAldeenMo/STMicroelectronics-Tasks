# Directory and File Utilities

🔍 This project provides utilities for reading directory contents and processing files. It includes functions to read directory entries, process file information, print file permissions, and handle paths.

## Table of Contents

- 📜 [Overview](#overview)
- 📁 [Files](#files)
  - 📂 [directories.c](#directoriesc)
  - 📂 [fileutils.c](#fileutilsc)
  - 📂 [main.c](#mainc)
  - 📂 [sorting.c](#sortingc)
- 🛠️ [Compilation and Execution](#compilation-and-execution)
- 📋 [Notes](#notes)
- 🎥 [Testing Video](#testing-video)
- ✍️ [Author](#author)

## Overview

📁 The project consists of several C files implementing functionalities to interact with directories and files. The primary functionalities include:

- **Reading Directory Contents**: Functions to read and list directory entries.
- **Processing Files**: Functions to process file attributes and print information.
- **File Utilities**: Utility functions to print file permissions and handle paths.

## Files

### `directories.c`

This file contains functions for directory operations:

- **`read_directory`**: Reads directory contents and stores file names in an array.
- **`process_file`**: Processes and prints file details including block size.
- **`process_fileHelper`**: Prints detailed file information based on options.
- **`cleanup`**: Frees allocated memory and closes the directory stream.

### `fileutils.c`

This file contains utility functions for handling file attributes:

- **`print_permissions`**: Prints file permissions in a human-readable format.
- **`getAbsolutePath`**: Returns the absolute path of a file.
- **`PrintColoredType`**: Prints file types with color coding based on file type.
- **`Print_L_OptionInfo`**: Prints detailed file information for the `-l` option.

### `main.c`

This file implements the main logic for the custom 'ls'-like utility `myls`:

- **`main`**: Handles command-line arguments, processes directories and files, and applies various options to format and display the output.
- **`CheckOnOptions`**: Parses command-line options and sets flags accordingly.
- **`do_ls`**: Main function for listing directory contents or processing a single directory/file.
- **`binarySearch`**: Finds a path in the sorted list of paths.

### `sorting.c`

This file provides functions for sorting file paths based on different criteria, such as name, access time, change time, and modification time.

- **`sortBuffer`**:Sorts an array of file paths according to specified sorting options.
- **`cmpstringp`**:Compares two file paths by name for sorting.
- **`cmpAcecssp`**:Compares two file paths by access time for sorting.
- **`cmpChangep`**Compares two file paths by change time for sorting.
- **`cmpModifip`**:Compares two file paths by modification time for sorting.
- **`cmpHelper`**:Retrieves file statistics and helps compare two files based on their attributes.

## Compilation and Execution

🛠️ To compile the code, use the `gcc` compiler:

```
git clone https://github.com/ShehabAldeenMo/STMicroelectronics-Tasks/tree/main/9-ls
cd 9-ls
make
```

## Notes
📋 The code includes basic error handling for directory reading and file operations. It supports various options like -l for long listing format and -i for inode number.

### Testing Video
🎥 A video demonstrating the testing procedures and results for the project is available. This video provides an overview of the test cases, validation routines, and overall functionality.
- **Watch the testing video**: [Video Link](https://drive.google.com/your-video-link) 

## Author
✍️ Name: Shehab aldeen mohammed <br />
🌐 Github: [ShehabAldeenMo](https://github.com/ShehabAldeenMo)<br />
🔗 LinkedIn: [Shehab aldeen mohammed](https://drive.google.com/file/d/1q5FbMjbhKv2fezgMuhaNF8h11_nbouUt/view?usp=sharing)<br />