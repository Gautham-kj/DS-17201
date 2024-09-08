#include <stdio.h>
#include <stdlib.h>

#define NODE_B_ADDRESS "gauthamk@localhost" // Replace with actual user and node B address
#define SSH_COMMAND "ssh " NODE_B_ADDRESS " uptime"

int main()
{
    char buffer[128];
    FILE *pipe = popen(SSH_COMMAND, "r");
    if (!pipe)
    {
        perror("popen");
        return 1;
    }

    // Read the output of the SSH command
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        printf("%s", buffer);
    }

    // Close the pipe
    pclose(pipe);

    return 0;
}