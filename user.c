#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

void print_usage() {
    printf("Usage: pmap_analog <PID>\n");
}


/* формируем команду для чтения карты памяти процесса */

void print_memory_map(int pid) {
    char command[MAX_LINE_LENGTH];
    sprintf(command, "cat /sys/kernel/debug/%d/maps", pid);

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error: Failed to execute command.\n");
        return;
    }

    printf("Memory map for PID %d:\n", pid);
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        printf("%s", line);
    }

    pclose(fp);
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
