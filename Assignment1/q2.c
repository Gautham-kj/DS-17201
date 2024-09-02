#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

int *extract_matrix(char *buffer)
{
    char *token = strtok(buffer, " ");
    int m = atoi(token);
    token = strtok(NULL, " ");
    int n = atoi(token);
    int *matrix1 = (int *)malloc(m * n * sizeof(int));
    int *matrix2 = (int *)malloc(m * n * sizeof(int));
    token = strtok(NULL, " ");
    int *ptr1 = matrix1;
    int *ptr2 = matrix2;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (token == NULL)
            {
                perror("Invalid input");
            }
            *ptr1 = atoi(token);
            token = strtok(NULL, " ");
            ptr1++;
        }
    }
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (token == NULL)
            {
                perror("Invalid input");
            }
            *ptr2 = atoi(token);
            token = strtok(NULL, " ");
            ptr2++;
        }
    }
    int *ans = (int *)malloc(m * n * sizeof(int));
    for (int i = 0; i < m * n; i++)
    {
        ans[i] = matrix1[i] + matrix2[i];
    }
    return ans;
}

int main()
{
    int pipefdp[2], pipefdc[2];
    pid_t pid;

    if (pipe(pipefdp) == -1 || pipe(pipefdc) == -1)
    {
        perror("pipe");
        return 1;
    }

    pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        // Child Process
        close(pipefdp[1]);
        close(pipefdc[0]);
        // assume that the  matrix size is sent as the first two integers
        char buffer[BUFFER_SIZE];
        int *matrix;
        wait(NULL);
        read(pipefdp[0], buffer, BUFFER_SIZE);
        // extract the matrix from the buffer and calculate the sums
        matrix = extract_matrix(buffer);
        write(pipefdc[1], matrix, BUFFER_SIZE);
        close(pipefdp[0]);
        close(pipefdc[1]);
    }
    else
    {
        // Parent Process
        close(pipefdp[0]);
        close(pipefdc[1]);
        // assume that the matrix is sent as the first two integers
        char buffer[BUFFER_SIZE];
        int m, n;
        printf("Enter the size of the matrix: ");
        scanf("%d %d", &m, &n);
        printf("Enter the first matrix: ");
        sprintf(buffer, "%d %d", m, n);
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                int num;
                scanf("%d", &num);
                sprintf(buffer, "%s %d", buffer, num);
            }
        }
        printf("Enter the second matrix: ");
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                int num;
                scanf("%d", &num);
                sprintf(buffer, "%s %d", buffer, num);
            }
        }
        printf("Buffer: %s\n", buffer);
        write(pipefdp[1], buffer, BUFFER_SIZE);
        int *result = (int *)malloc(m * n * sizeof(int));
        wait(NULL);
        read(pipefdc[0], result, BUFFER_SIZE);
        printf("The sum of the matrices is: \n");
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                printf("%d ", result[i * n + j]);
            }
            printf("\n");
        }
        close(pipefdp[1]);
        close(pipefdc[0]);
    }
    return 0;
}