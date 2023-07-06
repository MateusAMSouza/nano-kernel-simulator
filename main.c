//Embedded Operating Systems - Final Project - 2023.1
//Kernel with two schedulers - SJF and Priority with multiple queues (preemptive)
//Mateus Alexandre M. de Souza - 2021004023 - UNIFEI

#include <stdio.h>
#include "kernel.h"

int main()
{
    FILE *inputFile;
    FILE *outputFile;

    inputFile = fopen("inputProcesses.txt", "r");
    if(inputFile == NULL)
    {
        printf("The input file wasn't open.\n");
        return 1;
    }
    
    outputFile = freopen("outputProcesses.txt", "w", stdout);
    if(outputFile == NULL)
    {
        printf("The output file wasn't open.\n");
        fclose(inputFile);
        return 1;
    }
    
    Process allProcesses[NUM_ALL_PROCESSES];

    for(int i = 0; i < NUM_ALL_PROCESSES; i++)
    {
        fscanf(inputFile, "%d %d %d", &allProcesses[i].arrival, &allProcesses[i].cpu_burst_time, &allProcesses[i].priority);
        allProcesses[i].id = i;
    }

    fclose(inputFile);
    
    InteractiveScheduler s;
    Queue batch;
    
    interactive_scheduler_init(&s);
    batch_scheduler_init(&batch);

    kernelLoop(allProcesses, NUM_ALL_PROCESSES, &s, 1);
    kernelLoop(allProcesses, NUM_ALL_PROCESSES, &batch, 0);

    fclose(outputFile);

    return 0;
}