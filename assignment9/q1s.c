#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct
{
    int client_socket;
    int client_count;
} client_info_t;

void *handle_client(void *arg)
{
    client_info_t *client_info = (client_info_t *)arg;
    int client_socket = client_info->client_socket;
    int client_count = client_info->client_count;
    free(client_info);
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0)
    {
        buffer[bytes_read] = '\0';
        printf("Received from client %d: %s\n", client_count, buffer);
        write(client_socket, buffer, bytes_read); // Echo back to client
    }

    close(client_socket);
    return NULL;
}

int main()
{
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t tid;
    int client_count = 0;

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
        client_info_t *client_info = malloc(sizeof(client_info_t));
        if (client_info == NULL)
        {
            perror("malloc");
            continue;
        }

        client_info->client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_info->client_socket == -1)
        {
            perror("accept");
            free(client_info);
            continue;
        }

        client_info->client_count = ++client_count;

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if (pthread_create(&tid, NULL, handle_client, client_info) != 0)
        {
            perror("pthread_create");
            free(client_info);
        }

        pthread_detach(tid);
    }

    close(server_socket);
    return 0;
}