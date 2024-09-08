#include <stdio.h>
#include <stdlib.h>

#define NUM_PROCESSES 3

typedef struct
{
    int time;
} LamportClock;

typedef struct
{
    int time[NUM_PROCESSES];
} VectorClock;

void init_lamport_clock(LamportClock *clock)
{
    clock->time = 0;
}

void init_vector_clock(VectorClock *clock)
{
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        clock->time[i] = 0;
    }
}

void lamport_send_event(LamportClock *clock)
{
    clock->time++;
}

void lamport_receive_event(LamportClock *clock, int received_time)
{
    clock->time = (clock->time > received_time ? clock->time : received_time) + 1;
}

void vector_send_event(VectorClock *clock, int process_id)
{
    clock->time[process_id]++;
}

void vector_receive_event(VectorClock *clock, int process_id, int received_time[NUM_PROCESSES])
{
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        clock->time[i] = (clock->time[i] > received_time[i] ? clock->time[i] : received_time[i]);
    }
    clock->time[process_id]++;
}

void print_lamport_clock(LamportClock *clock, int process_id)
{
    printf("Process %d Lamport Clock: %d\n", process_id, clock->time);
}

void print_vector_clock(VectorClock *clock, int process_id)
{
    printf("Process %d Vector Clock: [", process_id);
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        printf("%d", clock->time[i]);
        if (i < NUM_PROCESSES - 1)
        {
            printf(", ");
        }
    }
    printf("]\n");
}

int main()
{
    LamportClock lamport_clocks[NUM_PROCESSES];
    VectorClock vector_clocks[NUM_PROCESSES];

    // Initialize clocks for each process
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        init_lamport_clock(&lamport_clocks[i]);
        init_vector_clock(&vector_clocks[i]);
    }

    // Simulate events
    printf("Simulating events...\n");

    // Process 0 sends a message to Process 1
    printf("Process 0 sends a message to Process 1\n");
    lamport_send_event(&lamport_clocks[0]);
    lamport_receive_event(&lamport_clocks[1], lamport_clocks[0].time);

    vector_send_event(&vector_clocks[0], 0);
    vector_receive_event(&vector_clocks[1], 1, vector_clocks[0].time);

    // Print clocks after events
    printf("\nClock values after events:\n");
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        print_lamport_clock(&lamport_clocks[i], i);
        print_vector_clock(&vector_clocks[i], i);
    }

    return 0;
}