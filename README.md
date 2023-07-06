 # Summary
 * [About the project](#about-the-project)
 * [How to run](#how-to-run)
 * [Input and Ouput](#input-and-output)

## About the project

Final project of the discipline Embedded Operating Systems of UNIFEI (Federal University of Itajubá). The goal is to practice concepts from programming (void pointers and circular buffers) 
and operating systems (schedulers) in the builiding of a nano preemptive kernel simulator in C. The kernel contains two scheduler, one preemptive (using priority with multiple queues)
and the other non-preemptive (using SJF - shortest job first). A brief explanation (in portuguese) can be seen at: (https://www.youtube.com/watch?v=_bvn6X-UBuY).

## How to run
To run the program, after clonning, run the following commands at the terminal in the right directory: 
```
gcc -c kernel.c -o kernel.o
gcc main.c kernel.o -o main
```
After running the main.exe, a file named "outputFile.txt" will be created.

## Input and Output
The input file contains multilpe rows with three numbers in each. The first one is assigned to "arrival", the second is assigned to "cpu_burst_time" and the third one is assigned
to "priority", all these variables are part of a struct named "Process".

You can change the values of the processes at the input file and add more of them by altering the defined value "NUM_ALL_PROCESSES" in the "kernel.h" file.
You can also change the time slice value of the preemptive scheduler by alterting the defined value "QUANTUM" in the "kernel.h" file.

The ouput file contains the scheduler's name and its logs during the management of the processes. Below this, a "graphic" is generated, ilustrating what process was being executed
at a determined period.

Example of graphic - During time 0 to 4, process 01 was being executed and so on:
```
-------------------------
| P01 | P02 | P03 | P04 |
-------------------------
000   004   006   012   018

```
