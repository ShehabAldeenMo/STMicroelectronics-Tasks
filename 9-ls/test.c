#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILES 1024

// Structure to store file name and access time
typedef struct {
    char name[1024];
    time_t access_time;
} FileInfo;

// Comparison function for qsort to sort by access time in descending order
int compareAccessTimeDescending(const void *a, const void *b) {
    FileInfo *file1 = (FileInfo *)a;
    FileInfo *file2 = (FileInfo *)b;

    if (file1->access_time > file2->access_time) return -1;
    if (file1->access_time < file2->access_time) return 1;
    return 0;
}

int main() {
    struct dirent *entry;
    struct stat fileStat;
    DIR *dir;
    FileInfo files[MAX_FILES];
    int file_count = 0;

    // Open the current directory
    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir error");
        return 1;
    }

    // Read files in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Get the file stats
        if (lstat(entry->d_name, &fileStat) < 0) {
            perror("lstat error");
            continue;
        }

        // Store the file name and access time
        if (file_count >= MAX_FILES) {
            fprintf(stderr, "Maximum file limit reached.\n");
            break;
        }

        strncpy(files[file_count].name, entry->d_name, sizeof(files[file_count].name) - 1);
        files[file_count].name[sizeof(files[file_count].name) - 1] = '\0'; // Ensure null-termination
        files[file_count].access_time = fileStat.st_atime;
        file_count++;
    }

    closedir(dir);

    // Sort files by access time in descending order
    qsort(files, file_count, sizeof(FileInfo), compareAccessTimeDescending);

    // Print the files and their access times in order
    printf("Files sorted by access time (descending):\n");
    for (int i = 0; i < file_count; i++) {
        printf("File: %s\t", files[i].name);
        printf("Access time (epoch): %ld\t", files[i].access_time); // Print raw time_t value
        printf("Access time: %s\n", ctime(&files[i].access_time));
    }

    return 0;
}
