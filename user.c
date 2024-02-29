#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

void print_usage() {
    printf("Usage: pmap_analog <PID>\n");
}

void print_memory_map(int pid) {
    char command[MAX_LINE_LENGTH];
    sprintf(command, "/sys/kernel/debug/%d/maps/pmap_info", pid);

    FILE *fp = fopen(command, "r");
    if (fp == NULL) {
        printf("Error: Failed to open debugfs file.\n");
        return;
    }

    printf("Memory map for PID %d:\n", pid);
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        printf("%s", line);
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_usage();
        return 1;
    }

    int pid = atoi(argv[1]);
    if (pid <= 0) {
        printf("Error: Invalid PID.\n");
        return 1;
    }

    print_memory_map(pid);

    return 0;
}
