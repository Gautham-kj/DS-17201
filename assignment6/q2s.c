#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

typedef struct
{
    int id;
    char ip[INET_ADDRSTRLEN];
    int port;
} ClientInfo;

ClientInfo master_table[MAX_CLIENTS];
int client_count = 0;

void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int bytes_read;

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(client_socket, (struct sockaddr *)&client_addr, &addr_len);

    master_table[client_count].id = client_count + 1;
    inet_ntop(AF_INET, &client_addr.sin_addr, master_table[client_count].ip, INET_ADDRSTRLEN);
    master_table[client_count].port = ntohs(client_addr.sin_port);
    client_count++;

    // Send updated master table to client
    write(client_socket, &client_count, sizeof(client_count));
    write(client_socket, master_table, sizeof(ClientInfo) * client_count);

    close(client_socket);
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

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

        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}