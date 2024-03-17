#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024


/* пользовательский код -- ввод PID процесса, который передаем в файл
 * читаем вывод информации о карте памяти процесса из этого pmap_info и выводим пользователю */
int main(int argc, char *argv[]) {
    int pid;
    FILE *file = fopen("/sys/kernel/debug/pmap_debugfs/pmap_info", "r+");
    if (file == NULL) {
        printf("Error while opeining file\n");
        return 0;
    }
    printf("Enter pid to send\n");
    if (sscanf(argv[1], "%d", &pid)) {
        char *buffer[BUFFER_SIZE];
        fprintf(file, "pid: %d", pid);
        while (!feof(file)) {
            char *result = fgets(buffer, BUFFER_SIZE, file);
            printf(result);
        }
    } else {
        printf("Something went wrong");
    }
    fclose(file);
    return 0;
}