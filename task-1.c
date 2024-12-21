/* fork-example-1.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

void* create_shared_memory(size_t size) {
    return mmap(NULL,
                size,
                PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS,
                -1, 0);
}


int main() {
    int* shmem = (int*) create_shared_memory(128);

    printf("Shared memory at: %p\n" , shmem);

    for (int i = 1; i <= 10; i++) {
        shmem[i-1] = i;
    } // store 1 to 10 in shared memory

    int pid = fork();
    if (pid == 0) {
        printf("Enter index and new value: ");
        int index, new_value;
        scanf("%d %d", &index, &new_value);
        shmem[index] = new_value;
        exit(0);
    } else {
        wait(NULL);
        printf("Array in shared memory: ");
        for (int i = 1; i <= 10; i++) {
            printf("%d ", shmem[i-1]);
        }
        printf("\n");
    }
}
