#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>

#define SHARED_PATH "/tmp/counterresource"
#define SEMAPHORE_NAME "Semaphore"

int main()
{
    // Open the semaphore
    sem_t *semaphore = sem_open(SEMAPHORE_NAME, 0);
    if (semaphore == SEM_FAILED)
    {
        perror("sem_open");
        return 1;
    }

    // Open the shared memory file
    int fd = open(SHARED_PATH, O_RDWR);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    // Map the shared memory
    int *counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (counter == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    // Read the counter value between sem_wait() and sem_post()
    sem_wait(semaphore);
    printf("Counter: %d\n", *counter);
    sem_post(semaphore);

    // Clean up resources
    munmap(counter, sizeof(int));
    close(fd);
    sem_close(semaphore);

    return 0;
}