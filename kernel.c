//Embedded Operating Systems - Final Project - 2023.1
//Kernel with two schedulers - SJF and Priority with multiple queues (preemptive)
//Mateus Alexandre M. de Souza - 2021004023 - UNIFEI

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "kernel.h"

void batch_scheduler_init(Queue *batch)
{
    batch->head = 0;
    batch->tail = 0;
}

void interactive_scheduler_init(InteractiveScheduler *scheduler)
{
    for(int i = 0; i < 3; i++)
    {
        batch_scheduler_init(&scheduler->queues[i]);
    }
}

int enqueueProcess(Queue *queue, Process *process)
{
    if(((queue->tail + 1) % MAX_QUEUE_SIZE) == queue->head)
    {
        return -1; 
    }
    queue->buffer[queue->tail] = process;
    queue->tail = (queue->tail + 1) % MAX_QUEUE_SIZE;
    return 0;
}

Process *dequeueProcess(Queue *queue)
{
    if(queue->head == queue->tail)
    {
        return NULL;
    }
    Process *process = queue->buffer[queue->head];
    queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;
    return process;
}

int interactive_scheduler_enqueue(InteractiveScheduler *scheduler, Process *process)
{
    //divides the processes into 3 queues, according to their priorities
    int queueNumber = -1;
    switch(process->priority)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            if(enqueueProcess(&scheduler->queues[2], process) == -1)
            {
                printf("Process %d wasn't queued\n", process->id);
            }
            else queueNumber = 2;
            break;
        case 4:
        case 5:
        case 6:
            if(enqueueProcess(&scheduler->queues[1], process) == -1)
            {
                printf("Process %d wasn't queued\n", process->id);
            }
            else queueNumber = 1;
            break;
        case 7:
        case 8:
        case 9:
        default:
             if(enqueueProcess(&scheduler->queues[0], process) == -1)
            {
                printf("Process %d wasn't queued\n", process->id);
            }
            else queueNumber = 0;
            break;    
    }
    return queueNumber;
}

Process *interactive_scheduler_dequeue(InteractiveScheduler *scheduler)
{
    //checks the highest priority queues first
    int queue_index = 0;
    Process *removed_process;
    for(queue_index = 0; queue_index < 3; queue_index++)
    {
        removed_process = dequeueProcess(&(scheduler->queues[queue_index]));
        if(removed_process != NULL)
        {
            return removed_process;
        }
    }
    return NULL;
}

int numberProcess(Queue *queue)
{
    int start = queue->head;
    int end = queue->tail;
    int size;
    if(start == 0)
    {
        size = end;
    }
    else if(start < end)
    {
        size = end - start;
    }
    else
    {
        size = MAX_QUEUE_SIZE - (start - end);
    }
    return size;
}

int partition(Process *array, int lowIndex, int highIndex, int criterion)
{
    int i, j;

    i = lowIndex; j = highIndex;
    Process pivot, auxiliary;

    pivot = array[lowIndex];

    switch(criterion)
    {
        case 0: //descending order of priority
            while(i < j)
            {
                while((array[i].priority >= pivot.priority) && (i < highIndex))
                    i++;
                while((array[j].priority <= pivot.priority) && (j > lowIndex))
                    j--;
                if(i < j)
                {
                    auxiliary = array[j];
                    array[j] = array[i];
                    array[i] = auxiliary;
                }
            }
            break;
        case 1:  //ascending order of cpu burst time
            while(i < j)
            {
                while((array[i].cpu_burst_time <= pivot.cpu_burst_time) && (i < highIndex))
                    i++;
                while((array[j].cpu_burst_time >= pivot.cpu_burst_time) && (j > lowIndex))
                    j--;
                if(i < j)
                {
                    auxiliary = array[j];
                    array[j] = array[i];
                    array[i] = auxiliary;
                }
            }
            break;
        case 2:  //ascending order of arrival
            while(i < j)
            {
                while((array[i].arrival <= pivot.arrival) && (i < highIndex))
                    i++;
                while((array[j].arrival >= pivot.arrival) && (j > lowIndex))
                    j--;
                if(i < j)
                {
                    auxiliary = array[j];
                    array[j] = array[i];
                    array[i] = auxiliary;
                }
            }
            break;
    }
    array[lowIndex] = array[j];
    array[j] = pivot;
    return j;
}

void quickSort(Process *array, int lowIndex, int highIndex, int criterion)
{
    if(lowIndex < highIndex)
    {
        int pivot = partition(array, lowIndex, highIndex, criterion);
        quickSort(array, lowIndex, pivot-1, criterion);
        quickSort(array, pivot+1, highIndex, criterion);
    }
}

void sortBuffer(Queue *queue, int criterion)
{
    //copies the processes to an array, order and copy them back to the queue
    int size = numberProcess(queue);
    int buffer_index = queue->head;
    Process *array = malloc(size * sizeof(Process));

    for(int i = 0; i < size; i++)
    {
        array[i] = *queue->buffer[buffer_index];
        buffer_index++;
    }
    
    quickSort(array, 0, size-1, criterion);
    
    queue->head = 0;
    queue->tail = size;
    for(int i = 0; i < size; i++)
    {
        queue->buffer[i] = malloc(sizeof(Process));
        memcpy(queue->buffer[i], &array[i], sizeof(Process));
    }

    free(array);
}

void kernelLoop(Process *allProcesses, int quantityProcesses, void *schedulerType, int type)
{
    int clock_tick = 0;
    int index = 0;

    //variables to generate the graphic
    int printingIndex = 0;
    int *printing_time_stamps = NULL;
    int *printing_executing_processes = NULL;

    Process *executingProcess;

    //order the input file according to the arrival
    quickSort(allProcesses, 0, quantityProcesses-1, 2);
    
    if(type == 0)
    {   
        printf("Batch scheduler - Shortest Job First\n");
        printf("Logs:\n");
        Queue *batch = (Queue *)schedulerType;
        int enqueued;

        while(index < quantityProcesses)
        {
            //processes move from "waiting queue" to the scheduler as their arrival time equals the clock
            while(clock_tick >= allProcesses[index].arrival && index < quantityProcesses)
            {
                enqueued = enqueueProcess(batch, &allProcesses[index]);
                if(enqueued != -1)
                {
                    index++;
                }
                else break;
            }
            sortBuffer(batch, 1);

            executingProcess = dequeueProcess(batch);
            clock_tick += executingProcess->cpu_burst_time;
            printf("Process %02d executed at the time: %d\n", executingProcess->id, clock_tick);

            //store values to generate the graphic
            printing_time_stamps = realloc(printing_time_stamps, (printingIndex+1) * sizeof(int));
            printing_time_stamps[printingIndex] = clock_tick;

            printing_executing_processes = realloc(printing_executing_processes, (printingIndex+1) * sizeof(int));
            printing_executing_processes[printingIndex] = executingProcess->id;
            printingIndex++;
        }
        
        while((executingProcess = dequeueProcess(batch)) != NULL) //all processes arrived
        {            
            clock_tick += executingProcess->cpu_burst_time;
            printf("Process %02d executed at the time: %d\n", executingProcess->id, clock_tick);

            //store values to generate the graphic
            printing_time_stamps = realloc(printing_time_stamps, (printingIndex+1) * sizeof(int));
            printing_time_stamps[printingIndex] = clock_tick;

            printing_executing_processes = realloc(printing_executing_processes, (printingIndex+1) * sizeof(int));
            printing_executing_processes[printingIndex] = executingProcess->id;
            printingIndex++;
        }
        printf("\n");
        printKernel(printing_executing_processes, printing_time_stamps, type, printingIndex);
    }
    else if(type == 1)
    {
        printf("Interactive scheduler - Priority (multiple queues)\n");
        printf("Logs:\n");
        InteractiveScheduler *scheduler = (InteractiveScheduler *)schedulerType;
        bool queueNumber[3] = {false, false, false};
        int priorityQueue;

        while(index < quantityProcesses)
        {
            while(clock_tick >= allProcesses[index].arrival && index < quantityProcesses)
            {
                priorityQueue = interactive_scheduler_enqueue(scheduler, &allProcesses[index]);

                if(priorityQueue != -1)
                {
                    index++;
                    queueNumber[priorityQueue] = true;
                }
                else break;
                
            }
            for(int i = 0; i < 3; i++)
            {
                if(queueNumber[i])
                {
                    sortBuffer(&scheduler->queues[i], 0);
                    queueNumber[i] = false;
                }
            }

            executingProcess = interactive_scheduler_dequeue(scheduler);
            executingProcess->cpu_burst_time -= QUANTUM;

            //store values to generate the graphic
            printing_executing_processes = realloc(printing_executing_processes, (printingIndex+1) * sizeof(int));
            printing_executing_processes[printingIndex] = executingProcess->id;
            printingIndex++;

            if(executingProcess->cpu_burst_time > 0)
            {
                executingProcess->arrival = clock_tick + QUANTUM;
                priorityQueue = interactive_scheduler_enqueue(scheduler, executingProcess);
                sortBuffer(&scheduler->queues[priorityQueue], 0);

                printf("Process %02d returning to queue at the time: %d\n", executingProcess->id, clock_tick+QUANTUM);
            }
            else printf("Process %02d executed at the time: %d\n", executingProcess->id, clock_tick+QUANTUM);

            clock_tick += QUANTUM;
        }
        while((executingProcess = interactive_scheduler_dequeue(scheduler)) != NULL) //all processes arrived
        {
            executingProcess->cpu_burst_time -= QUANTUM;

            //store values to generate the graphic
            printing_executing_processes = realloc(printing_executing_processes, (printingIndex+1) * sizeof(int));
            printing_executing_processes[printingIndex] = (executingProcess->id);
            printingIndex++;

            if(executingProcess->cpu_burst_time > 0)
            {
                executingProcess->arrival = clock_tick + QUANTUM;
                priorityQueue = interactive_scheduler_enqueue(scheduler, executingProcess);
                sortBuffer(&scheduler->queues[priorityQueue], 0);

                printf("Process %02d returning to queue at the time: %d\n", executingProcess->id, clock_tick+QUANTUM);
            }
            else printf("Process %02d executed at the time: %d\n", executingProcess->id, clock_tick+QUANTUM);
            
            clock_tick += QUANTUM;
        }
        printf("\n");
        printKernel(printing_executing_processes, printing_time_stamps, type, printingIndex);
    }
    free(printing_executing_processes);
    free(printing_time_stamps);
}

void printKernel(int *processes, int *timeStamps, int type, int length)
{
    int i = 0, max_columns, index = 0;
    if(type == 0)
    {
        printf("Graphic - SJF:\n");
        if(length > 20) max_columns = 20;
        else max_columns = length;

        while(i < length)
        {
            for(i = index; i < max_columns; i++)
            {
                printf("------");
            }
            printf("-\n");

            for(i = index; i < max_columns; i++)
            {
                printf("| P%02d ", processes[i]);
            }
            printf("|\n");

            for(i = index; i < max_columns; i++)
            {
                printf("------");
            }
            printf("-\n");

            printf("000   ");
            for(i = index; i < max_columns; i++)
            {
                printf("%03d   ", timeStamps[i]);
            }
            printf("\n");
            
            i++;
            index += max_columns;
            if((max_columns + 20) > length) max_columns = length;
            else max_columns += 20;
            
        }
        printf("\nFinished at time: %d\n\n", timeStamps[length-1]);
    }
    else
    {
        printf("Graphic - Priority (multilpe queues):\n");
        if(length > 20)
        {
            max_columns = 20;
        }
        while(i < length)
        {
            for(i = index; i < max_columns; i++)
            {
                printf("------");
            }
            printf("-\n");

            for(i = index; i < max_columns; i++)
            {
                printf("| P%02d ", processes[i]);
            }
            printf("|\n");

            for(i = index; i < max_columns; i++)
            {
                printf("------");
            }
            printf("-\n");

            for(i = index; i <= max_columns; i++)
            {
                printf("%03d   ", (QUANTUM*i));
            }
            printf("\n");
            i--;
            index += 20;
            if((max_columns + 20) > length) max_columns = length;
            else max_columns += 20;
        }
        printf("\nFinished at time: %d\n\n", (QUANTUM*(i)));
    }
}
