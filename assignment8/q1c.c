#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void send_file(int socket, const char *file_name)
{
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Send file name
    write(socket, file_name, strlen(file_name) + 1);

    // Open file for reading
    FILE *file = fopen(file_name, "rb");
    if (file == NULL)
    {
        perror("fopen");
        return;
    }

    // Send file data
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        write(socket, buffer, bytes_read);
    }

    printf("File %s sent successfully.\n", file_name);

    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *file_name = argv[1];
    int client_socket;
    struct sockaddr_in server_addr;

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

    send_file(client_socket, file_name);

    close(client_socket);
    return 0;
}