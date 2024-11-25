#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Receive file name
    bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0)
    {
        perror("read");
        close(client_socket);
        return;
    }
    buffer[bytes_read] = '\0';
    char *file_name = strdup(buffer);

    // Open file for writing
    FILE *file = fopen(file_name, "wb");
    if (file == NULL)
    {
        perror("fopen");
        free(file_name);
        close(client_socket);
        return;
    }

    // Receive file data
    while ((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0)
    {
        fwrite(buffer, 1, bytes_read, file);
    }

    printf("File %s received successfully.\n", file_name);

    fclose(file);
    free(file_name);
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