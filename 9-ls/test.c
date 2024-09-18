#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#define MAX_PATH_LENGTH 1024

void print_file_info(const char *path) {
    struct stat file_info;

    // Use lstat to get file information (including for symbolic links)
    if (lstat(path, &file_info) == -1) {
        perror("lstat error");
        return;
    }

    // Check the file type
    printf("File: %s\n", path);
    if (S_ISREG(file_info.st_mode)) {
        printf("Type: Regular file\n");
    } else if (S_ISDIR(file_info.st_mode)) {
        printf("Type: Directory\n");
    } else if (S_ISLNK(file_info.st_mode)) {
        printf("Type: Symbolic link\n");
    } else if (S_ISCHR(file_info.st_mode)) { 
        printf("Type: Character device\n");
    } else if (S_ISBLK(file_info.st_mode)) {
        printf("Type: Block device\n");
    } else if (S_ISFIFO(file_info.st_mode)) {
        printf("Type: FIFO/pipe\n");
    } else if (S_ISSOCK(file_info.st_mode)) {
        printf("Type: Socket\n");
    } else {
        printf("Type: Unknown file type\n");
    }

    // Print file size
    printf("Size: %lld bytes\n", (long long) file_info.st_size);

    // Print file permissions
    printf("Permissions: ");
    printf( (file_info.st_mode & S_IRUSR) ? "r" : "-");
    printf( (file_info.st_mode & S_IWUSR) ? "w" : "-");
    printf( (file_info.st_mode & S_IXUSR) ? "x" : "-");
    printf( (file_info.st_mode & S_IRGRP) ? "r" : "-");
    printf( (file_info.st_mode & S_IWGRP) ? "w" : "-");
    printf( (file_info.st_mode & S_IXGRP) ? "x" : "-");
    printf( (file_info.st_mode & S_IROTH) ? "r" : "-");
    printf( (file_info.st_mode & S_IWOTH) ? "w" : "-");
    printf( (file_info.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n\n");
}

void list_directory(const char *dir_path) {
    DIR *dir;
    struct dirent *entry;
    char full_path[MAX_PATH_LENGTH];

    // Open the directory
    if ((dir = opendir(dir_path)) == NULL) {
        perror("opendir error");
        return;
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Create the full path for the file
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        // Print file information
        print_file_info(full_path);
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    const char *dir_path = ".";  // Default to current directory

    if (argc > 1) {
        dir_path = argv[1];  // Use the provided directory path
    }

    list_directory(dir_path);

    return 0;
}

