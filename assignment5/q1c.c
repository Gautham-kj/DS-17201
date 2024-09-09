#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <port_no> <ip_addr>\n", argv[0]);
        return -1;
    }

    int port_no = atoi(argv[1]);
    char *ip_addr = argv[2];

    int socket_desc;
    struct sockaddr_in server_addr;
    char server_message[2000], client_message[2000];
    socklen_t server_struct_length = sizeof(server_addr);

    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socket_desc < 0)
    {
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_no);
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);

    // Get input from the user:
    printf("Enter input number: ");
    fgets(client_message, 8, stdin);

    // Send the message to server:
    if (sendto(socket_desc, client_message, strlen(client_message), 0,
               (struct sockaddr *)&server_addr, server_struct_length) < 0)
    {
        printf("Unable to send message\n");
        return -1;
    }

    if (recvfrom(socket_desc, client_message, sizeof(client_message), 0, (struct sockaddr *)&server_addr, &server_struct_length) < 0)
    {
        printf("Couldn't receive\n");
        return -1;
    }
    printf("factorial is: %s\n", client_message);

    close(socket_desc);

    return 0;
}
