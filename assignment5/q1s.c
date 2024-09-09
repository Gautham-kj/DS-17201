// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <limits.h>

#define MAX_SIZE 99999

long long map[MAX_SIZE];

long long factorial(int n)
{
    if (n < 2)
    {
        return map[n] = 1;
    }
    if (map[n] != 0)
    {
        return map[n];
    }
    return map[n] = factorial(n - 1) * n;
}

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
    struct sockaddr_in server_addr, client_addr;
    char client_message[200], *server_message;
    socklen_t client_struct_length = sizeof(client_addr);

    // Clean buffers:
    memset(client_message, '\0', sizeof(client_message));

    // Create UDP socket:
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

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Couldn't bind to the port\n");
        return -1;
    }

    printf("Listening for incoming messages...\n\n");

    // Receive client's message:
    if (recvfrom(socket_desc, client_message, sizeof(client_message), 0, (struct sockaddr *)&client_addr, &client_struct_length) < 0)
    {
        printf("Couldn't receive\n");
        return -1;
    }
    printf("Received message from IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    printf("Msg from client: %s\n", client_message);

    // Respond to client:

    int number = atoi(client_message);
    printf("Number: %d\n", number);
    sprintf(server_message, "%lld", factorial(number));
    if (sendto(socket_desc, server_message, strlen(server_message), 0, (struct sockaddr *)&client_addr, client_struct_length) < 0)
    {
        printf("Can't send\n");
        return -1;
    }

    // Close the socket:
    close(socket_desc);

    return 0;
}
