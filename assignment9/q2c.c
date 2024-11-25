#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

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

    printf("Connected to server. Type messages and press Enter to send.\n");

    while (1)
    {
        printf("You: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

        write(client_socket, buffer, strlen(buffer));

        bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0)
        {
            perror("read");
            break;
        }

        buffer[bytes_read] = '\0';
        printf("Server: %s\n", buffer);
    }

    close(client_socket);
    return 0;
}