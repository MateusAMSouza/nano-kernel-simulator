//Embedded Operating Systems - Final Project - 2023.1
//Kernel with two schedulers - SJF and Priority with multiple queues (preemptive)
//Mateus Alexandre M. de Souza - 2021004023 - UNIFEI

#ifndef KERNEL_H
#define KERNEL_H

#define MAX_QUEUE_SIZE 10
#define MAX_NUM_QUEUES 3
#define NUM_ALL_PROCESSES 20
#define QUANTUM 2

typedef struct
{
    int id; //not defined in the input file
    int arrival;
    int cpu_burst_time;
    int priority;
} Process;

typedef struct
{
    Process *buffer[MAX_QUEUE_SIZE];
    int head;
    int tail;
} Queue;

typedef struct
{
    Queue queues[MAX_NUM_QUEUES];
} InteractiveScheduler;

void batch_scheduler_init(Queue *batch);

void interactive_scheduler_init(InteractiveScheduler *scheduler);

int enqueueProcess(Queue *queue, Process *process);

Process *dequeueProcess(Queue *queue);

int interactive_scheduler_enqueue(InteractiveScheduler *scheduler, Process *process);

Process *interactive_scheduler_dequeue(InteractiveScheduler *scheduler);

int numberProcess(Queue *queue);

int partition(Process *array, int lowIndex, int highIndex, int criterion);

void quickSort(Process *array, int lowIndex, int highIndex, int criterion);

void sortBuffer(Queue *queue, int criterion);

void kernelLoop(Process *allProcesses, int quantityProcesses, void *schedulerType, int type);

void printKernel(int *processes, int *timeStamps, int type, int length);

#endif