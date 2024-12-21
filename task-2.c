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
    sem_t* sem = (sem_t*)create_shared_memory(sizeof(sem_t));
    sem_init(sem, 1, 1); // Инициализация семафора для разделяемой памяти

    for (int i = 0; i < 10; i++) {
        shmem[i] = i + 1;
    } // Shared mem array initialization

    int pid = fork();

    if (pid == 0) {
        // Дочерний процесс
        close(pipefd[0]); // Закрываем неиспользуемый конец для чтения
        int index, new_value;
        while (1) {
            printf("Index & new value: ");
            scanf("%d %d", &index, &new_value);
            if (index < 0) {
                break;
            }
            if (index >= 0 && index < 10) {
                sem_wait(sem); // Ожидание семафора
                // Ждёт пока родительский процесс не освободит семафор
                shmem[index] = new_value;
                sem_post(sem); // Освобождение семафора
                write(pipefd[1], &index, sizeof(int)); // Уведомляем родительский процесс
            }
        }
        close(pipefd[1]); // Закрываем конец для записи
        exit(0);
    } else {
        // Родительский процесс
        close(pipefd[1]); // Закрываем неиспользуемый конец для записи
        int index;
        while (read(pipefd[0], &index, sizeof(int)) > 0) {
            sem_wait(sem); // Ожидание семафора
            // Ждёт пока дочерний процесс не освободит семафор
            printf("Array: ");
            for (int i = 0; i < 10; i++) {
                printf("%d ", shmem[i]);
            }
            printf("\n");
            sem_post(sem); // Освобождение семафора
        }
        close(pipefd[0]); // Закрываем конец для чтения
        wait(NULL);
        sem_destroy(sem); // Уничтожение семафора
    }

    return 0;
}