#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_PROCESSES 3

typedef struct
{
    int time;
} LamportClock;

typedef struct
{
    int time[NUM_PROCESSES];
} VectorClock;

typedef struct
{
    int id;
    LamportClock lamport_clock;
    VectorClock vector_clock;
    bool requesting;
    bool in_critical_section;
} Process;

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

void request_critical_section(Process *processes, int process_id)
{
    processes[process_id].requesting = true;
    lamport_send_event(&processes[process_id].lamport_clock);
    vector_send_event(&processes[process_id].vector_clock, process_id);
    printf("Process %d requests critical section\n", process_id);
}

void release_critical_section(Process *processes, int process_id)
{
    processes[process_id].in_critical_section = false;
    processes[process_id].requesting = false;
    lamport_send_event(&processes[process_id].lamport_clock);
    vector_send_event(&processes[process_id].vector_clock, process_id);
    printf("Process %d releases critical section\n", process_id);
}

void reply_to_request(Process *processes, int from_process_id, int to_process_id)
{
    lamport_receive_event(&processes[to_process_id].lamport_clock, processes[from_process_id].lamport_clock.time);
    vector_receive_event(&processes[to_process_id].vector_clock, to_process_id, processes[from_process_id].vector_clock.time);
    printf("Process %d replies to Process %d\n", to_process_id, from_process_id);
}

int main()
{
    Process processes[NUM_PROCESSES];

    // Initialize processes
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        processes[i].id = i;
        init_lamport_clock(&processes[i].lamport_clock);
        init_vector_clock(&processes[i].vector_clock);
        processes[i].requesting = false;
        processes[i].in_critical_section = false;
    }

    // Simulate events
    printf("Simulating events...\n");

    // Process 0 requests critical section
    request_critical_section(processes, 0);

    // Process 1 replies to Process 0
    reply_to_request(processes, 0, 1);

    // Process 2 replies to Process 0
    reply_to_request(processes, 0, 2);

    // Process 0 enters critical section
    processes[0].in_critical_section = true;
    printf("Process %d enters critical section\n", 0);

    // Process 0 releases critical section
    release_critical_section(processes, 0);

    // Print clocks after events
    printf("\nClock values after events:\n");
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        print_lamport_clock(&processes[i].lamport_clock, i);
        print_vector_clock(&processes[i].vector_clock, i);
    }

    return 0;
}