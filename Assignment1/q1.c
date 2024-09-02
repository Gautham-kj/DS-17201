#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

void concatenate_strings(char *dest, const char *src1, const char *src2)
{
    while (*src1)
    {
        *dest++ = *src1++;
    }
    while (*src2)
    {
        *dest++ = *src2++;
    }
    *dest = '\0';
}

int main()
{
    int pipefd1[2], pipefd2[2];
    pid_t pid1, pid2;
    char buffer[BUFFER_SIZE];
    char input_string[] = "Hello";
    char additional_string[] = " World";

    if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1)
    {
        perror("pipe");
        return 1;
    }

    pid1 = fork();
    if (pid1 == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid1 == 0)
    {
        close(pipefd2[1]);
        close(pipefd1[0]);
        write(pipefd1[1], input_string, strlen(input_string) + 1);

        wait(NULL);

        read(pipefd2[0], buffer, BUFFER_SIZE);

        printf("Concatenated string: %s\n", buffer);

        close(pipefd1[1]);
        close(pipefd2[0]);
    }
    else
    {
        pid2 = fork();
        if (pid2 == -1)
        {
            perror("fork");
            return 1;
        }

        if (pid2 == 0)
        {
            close(pipefd1[1]);
            close(pipefd2[0]);
            read(pipefd1[0], buffer, BUFFER_SIZE);

            char concatenated_string[BUFFER_SIZE];
            concatenate_strings(concatenated_string, buffer, additional_string);

            write(pipefd2[1], concatenated_string, strlen(concatenated_string) + 1);

            close(pipefd2[1]);
            close(pipefd1[0]);
        }
        else
        {
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}