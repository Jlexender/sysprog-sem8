#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>

void* create_shared_memory(size_t size) {
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

int main() {
    int* shmem = (int*)create_shared_memory(10 * sizeof(int));
    int pipefd[2];
    pipe(pipefd);
    sem_t* sem_mem = (sem_t*)create_shared_memory(sizeof(sem_t));
    sem_t* sem_signal = (sem_t*)create_shared_memory(sizeof(sem_t));
    sem_init(sem_mem, 1, 1);
    sem_init(sem_signal, 1, 0);

    for (int i = 0; i < 10; i++) {
        shmem[i] = i + 1;
    }

    int pid = fork();

    if (pid == 0) {
        close(pipefd[0]);
        int index, new_value;
        while (1) {
            printf("id val ");
            scanf("%d %d", &index, &new_value);
            if (index < 0) {
                write(pipefd[1], &index, sizeof(int));
                sem_post(sem_signal);
                break;
            }
            if (index >= 0 && index < 10) {
                sem_wait(sem_mem);
                shmem[index] = new_value;
                sem_post(sem_mem);
                write(pipefd[1], &index, sizeof(int));
                sem_post(sem_signal);
            }
        }
        close(pipefd[1]);
        exit(0);
    } else {
        close(pipefd[1]);
        int index;
        while (1) {
            sem_wait(sem_signal);
            if (read(pipefd[0], &index, sizeof(int)) > 0) {
                if (index < 0) {
                    break;
                }
                sem_wait(sem_mem);
                printf("\nArray: ");
                for (int i = 0; i < 10; i++) {
                    printf("%d ", shmem[i]);
                }
                printf("\n");
                sem_post(sem_mem);
            }
        }
        close(pipefd[0]);
        wait(NULL);
        sem_destroy(sem_mem);
        sem_destroy(sem_signal);
    }

    return 0;
}