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

ClientInfo client_table[MAX_CLIENTS];
int client_count = 0;

int main()
{
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Receive updated master table from server
    read(client_socket, &client_count, sizeof(client_count));
    read(client_socket, client_table, sizeof(ClientInfo) * client_count);

    printf("Updated client table:\n");
    for (int i = 0; i < client_count; i++)
    {
        printf("ID: %d, IP: %s, Port: %d\n", client_table[i].id, client_table[i].ip, client_table[i].port);
    }

    close(client_socket);
    return 0;
}