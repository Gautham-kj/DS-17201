#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

#define SHARED_PATH "/tmp/counterresource"
#define SEMAPHORE_NAME "Semaphore"

volatile sig_atomic_t stop = 0;

void handle_signal(int signal)
{
    stop = 1;
}

void increment_counter(sem_t *semaphore, int *counter)
{
    sem_wait(semaphore);
    (*counter)++;
    printf("Counter: %d\n", *counter);
    sleep(1);
    sem_post(semaphore);
}

int main()
{
    sem_t *semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED)
    {
        perror("sem_open");
        return 1;
    }

    pid_t pid = fork();

    int fd = open(SHARED_PATH, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("Could Not Open Shared File");
        return 1;
    }

    if (ftruncate(fd, sizeof(int)) == -1)
    {
        perror("ftruncate");
        return 1;
    }

    int *counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (counter == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    *counter = 0;

    // Set up the signal handler for SIGINT
    signal(SIGINT, handle_signal);

    if (pid == -1)
    {
        perror("Failure while creating Child Process");
        return 1;
    }
    if (pid == 0)
    {
        while (!stop)
        {
            increment_counter(semaphore, counter);
        }
    }
    else
    {
        sleep(1);
        while (!stop)
        {
            increment_counter(semaphore, counter);
        }
    }

    sem_close(semaphore);
    sem_unlink(SEMAPHORE_NAME);
    munmap(counter, sizeof(int));
    close(fd);
    shm_unlink(SHARED_PATH);
    return 0;
}