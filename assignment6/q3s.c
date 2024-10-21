#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/sysinfo.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void get_system_info(char *buffer)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    struct sysinfo sys_info;

    if (sysinfo(&sys_info) != 0)
    {
        perror("sysinfo");
        exit(EXIT_FAILURE);
    }

    double load = sys_info.loads[0] / (double)(1 << SI_LOAD_SHIFT);

    snprintf(buffer, BUFFER_SIZE, "Date and Time: %02d-%02d-%04d %02d:%02d:%02d\nCPU Load: %.2f\n",
             t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
             t->tm_hour, t->tm_min, t->tm_sec, load);
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) == -1)
    {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1)
        {
            perror("accept");
            continue;
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        get_system_info(buffer);
        write(client_socket, buffer, strlen(buffer));

        close(client_socket);
    }

    close(server_socket);
    return 0;
}