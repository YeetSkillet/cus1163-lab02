#include "proc_reader.h"

int list_process_directories(void) {
    DIR* proc_dir = opendir("/proc");
    struct dirent* entry;
    int process_count = 0;

    if (!proc_dir) {
        perror("opendir failed");
        return -1;
    }

    printf("Process directories in /proc:\n");
    printf("%-8s %-20s\n", "PID", "Type");
    printf("%-8s %-20s\n", "---", "----");

    while ((entry = readdir(proc_dir)) != NULL) {
        if (is_number(entry->d_name)) {
            printf("%-8s %-20s\n", entry->d_name, "Process");
            process_count++;
        }
    }

    if (closedir(proc_dir) == -1) {
        perror("closedir failed");
        return -1;
    }

    printf("\nTotal process directories found: %d\n", process_count);
    return 0; // Replace with proper error handling
}

int read_process_info(const char* pid) {
    char filepath[256];

    // Read status file
    snprintf(filepath, sizeof(filepath), "/proc/%s/status", pid);
    printf("\n--- Process Information for PID %s ---\n", pid);

    if (read_file_with_syscalls(filepath) == -1) {
        fprintf(stderr, "Failed to read status file for PID %s\n", pid);
        return -1;
    }

    // Read cmdline file
    snprintf(filepath, sizeof(filepath), "/proc/%s/cmdline", pid);
    printf("\n--- Command Line ---\n");

    if (read_file_with_syscalls(filepath) == -1) {
        fprintf(stderr, "Failed to read cmdline file for PID %s\n", pid);
        return -1;
    }

    printf("\n"); // Add extra newline for readability

    return 0; // Replace with proper error handling
}

int show_system_info(void) {
    int line_count = 0;
    const int MAX_LINES = 10;
    char buffer[1024];
    FILE* file;

    printf("\n--- CPU Information (first %d lines) ---\n", MAX_LINES);
    file = fopen("/proc/cpuinfo", "r");
    if (!file) {
        perror("Failed to open /proc/cpuinfo");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), file) && line_count < MAX_LINES) {
        printf("%s", buffer);
        line_count++;
    }
    fclose(file);

    printf("\n--- Memory Information (first %d lines) ---\n", MAX_LINES);
    file = fopen("/proc/meminfo", "r");
    if (!file) {
        perror("Failed to open /proc/meminfo");
        return -1;
    }

    line_count = 0;
    while (fgets(buffer, sizeof(buffer), file) && line_count < MAX_LINES) {
        printf("%s", buffer);
        line_count++;
    }
    fclose(file);

    return 0; // Replace with proper error handling
}

void compare_file_methods(void) {
    const char* test_file = "/proc/version";

    printf("Comparing file reading methods for: %s\n\n", test_file);

    printf("=== Method 1: Using System Calls ===\n");
    read_file_with_syscalls(test_file);

    printf("\n=== Method 2: Using Library Functions ===\n");
    read_file_with_library(test_file);

    printf("\nNOTE: Run this program with strace to see the difference!\n");
    printf("Example: strace -e trace=openat,read,write,close ./lab2\n");
}

int read_file_with_syscalls(const char* filename) {
    int fd;
    char buffer[1024];
    ssize_t bytes_read;

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open");
        return -1;
    }

    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }

    if (bytes_read == -1) {
        perror("Failed to read");
        close(fd);
        return -1;
    }

    if (close(fd) == -1) {
        perror("Failed to close");
        return -1;
    }

    return 0; //Replace with proper error handling
}

int read_file_with_library(const char* filename) {
    FILE* file = fopen(filename, "r");
    char buffer[1024];

    if (file == NULL) {
        perror("fopen failed to open file");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }

    if (fclose(file) != 0) {
        perror("flcose failed to close file");
        return -1;
    }

    return 0; //Replace with proper error handling
}

int is_number(const char* str) {
    if (str == NULL || *str == '\0') return 0;
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}