#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/sysinfo.h>

#define PORT 8080
#define SERVER1_PORT 8081
#define SERVER2_PORT 8082
#define BUFFER_SIZE 1024

double get_cpu_load()
{
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != 0)
    {
        perror("sysinfo");
        exit(EXIT_FAILURE);
    }
    return sys_info.loads[0] / (double)(1 << SI_LOAD_SHIFT);
}

void forward_to_server(const char *server_ip, int server_port, const char *input, char *output)
{
    int server_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    write(server_socket, input, strlen(input));
    bytes_read = read(server_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        strcpy(output, buffer);
    }

    close(server_socket);
}

int main()
{
    int load_balancer_socket, client_socket;
    struct sockaddr_in load_balancer_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    int bytes_read;

    load_balancer_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (load_balancer_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    load_balancer_addr.sin_family = AF_INET;
    load_balancer_addr.sin_addr.s_addr = INADDR_ANY;
    load_balancer_addr.sin_port = htons(PORT);

    if (bind(load_balancer_socket, (struct sockaddr *)&load_balancer_addr, sizeof(load_balancer_addr)) == -1)
    {
        perror("bind");
        close(load_balancer_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(load_balancer_socket, 10) == -1)
    {
        perror("listen");
        close(load_balancer_socket);
        exit(EXIT_FAILURE);
    }

    printf("Load balancer listening on port %d\n", PORT);

    while (1)
    {
        client_socket = accept(load_balancer_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1)
        {
            perror("accept");
            continue;
        }

        bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';

            double cpu_load1 = get_cpu_load(); // Simulate getting CPU load for server 1
            double cpu_load2 = get_cpu_load(); // Simulate getting CPU load for server 2

            if (cpu_load1 < cpu_load2)
            {
                forward_to_server("127.0.0.1", SERVER1_PORT, buffer, output);
            }
            else
            {
                forward_to_server("127.0.0.1", SERVER2_PORT, buffer, output);
            }

            write(client_socket, output, strlen(output));
        }

        close(client_socket);
    }

    close(load_balancer_socket);
    return 0;
}