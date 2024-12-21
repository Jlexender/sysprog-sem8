/* thread-example.c */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void bad_print(char *s)
{
    for (; *s != '\0'; s++)
    {
        fprintf(stdout, "%c", *s);
        fflush(stdout);
        usleep(100);
    }
}

pthread_mutex_t m; // Мьютекс

void* my_thread(void* arg)
{
    for(int i = 0; i < 10; i++ )
    {
        pthread_mutex_lock(&m); // Лок
        bad_print((char *)arg);
        pthread_mutex_unlock(&m); // Анлок
        sleep(1);
    }
    return NULL;
}

int main()
{
    // Заметим, что программа всё ещё недетерминирована, но теперь нет каши из символов.
    pthread_mutex_init(&m, NULL); // Инициализация мьютекса
    pthread_t t1, t2;
    pthread_create(&t1, NULL, my_thread, "Hello\n");
    pthread_create(&t2, NULL, my_thread, "world\n");
    pthread_exit( NULL );
    return 0;
}

