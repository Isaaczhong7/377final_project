# CS377 FINAL PROJECT - Implementation of Multi Level Feedback Queue (MLFQ) and Lottery Scheduling 

## Header File
I extended Project 3 into two more scheduling algorithm. the first one is call Multi Level Feedback Queue (MLFQ) and the second one is Lottery Scheduling. Because the complexity of these algorithm, I updated Process struct which allows me to have more access to processes. For MLFQ I added priority field using for dynamically moving through queues and job_type field simulating different type of jobs. For lottery algorithm, I added lottery_ticket field storing how many tickets each process received. Lastly. A new pid field for tracking processes.
``` 
struct Process {
  int arrival;
  int first_run;
  int duration;
  int completion;
  int priority;
  int job_type;
  int pid;
  int lottery_tickets;
};
```
## Scheduling File
### Multi Level Feedback Queue

#### Design 
This algorithm is designed to prioritize short and I/O-bound (interactive) processes over long and CPU-bound processes to optimize CPU usage and responsiveness.The algorithm operates with multiple queues, each having a different priority level. The highest priority is assigned to the top queue, and priority decreases as we move down the queues. Each queue operates its own scheduling algorithm - the first N-1 queues use Round Robin scheduling, while the lowest level queue often uses FCFS (First-Come, First-Served) scheduling. New processes are inserted into the highest priority queue. If they do not finish execution within their time quantum (time slice), they are moved down to a lower priority queue. If a process voluntarily yields control of the CPU before its time quantum expires, it stays in the same queue. In this implementation, a "boosting" mechanism is used to prevent starvation of lower-priority processes. At regular intervals (```boosting_interval```), all processes are moved back to the highest priority queue.

#### Documentation 
``` read_workload(string filename)  ``` Reads a list of processes from a file. Each process is represented by a line in the file, with the arrival time, duration, job type, process ID separated by spaces. The processes are added to a priority queue sorted by arrival time.

```mlfq function```: This function is the main implementation of the MLFQ scheduling algorithm. It takes a workload (a priority queue of processes), the number of queues, an array of time quanta for each queue, and a boosting interval as parameters. It returns a list of completed processes.
```workload```: A priority queue of processes sorted by their arrival time. Processes are removed from this queue and added to the MLFQ when their arrival time comes.

```num_queues```: The number of queues in the MLFQ. Each queue corresponds to a different priority level.

```time_quantum[]```: An array specifying the time quantum for each queue in the MLFQ.

```boosting_interval```: After this amount of time, all processes are boosted back to the highest priority queue to prevent starvation.

```find_next_queue``` function: A helper function that finds the next non-empty queue in the MLFQ.

```Priority Boost```: At regular intervals (specified by boosting_interval), all processes are moved back to the highest priority queue.

Round Robin and FCFS Scheduling: The scheduling within each queue depends on the queue's priority. The highest priority queues use a Round Robin scheduling algorithm (with time quantum specified by time_quantum[]), while the lowest priority queue uses FCFS scheduling.
### Lottery Scheduling
#### Design
This algorithm implemented is a combination of the Lottery scheduling and the Round Robin scheduling algorithms, adapted for a system that handles processes. This scheduling algorithm is meant to be fair and efficient, avoiding the starvation of any process and ensuring that all processes get their fair share of CPU time. The design of this algorithm involves a queue of processes and a function to find the lottery winner. The function find_lottery_winner calculates the total number of tickets and generates a random number (the lucky ticket). It then iterates over the process queue to find the process that holds the lucky ticket. The lo function uses this to implement the combined lottery and round-robin scheduling. In each round, if there are processes in the queue, a winner is selected using the lottery system, and this process is allowed to run for the time quantum. If the process finishes within this time, it is removed from the queue; otherwise, it is added back to the end of the queue to await the next round.

#### Documentation
``` read_workload(string filename)  ``` Reads a list of processes from a file. Each process is represented by a line in the file, with the arrival time, duration, job type, process ID, and number of lottery tickets separated by spaces. The processes are added to a priority queue sorted by arrival time.

``` find_lottery_winner(list<Process>& q) ``` Finds the winner of the lottery for the next time quantum. The winner is determined by generating a random number between 1 and the total number of lottery tickets, and then iterating through the processes in the queue until the sum of their lottery tickets is greater than or equal to this random number. The function returns the index of the winning process in the queue.

``` lo(pqueue_arrival workload) ``` Implements the combined Lottery and Round Robin scheduling algorithm. It operates in a loop until all processes have been handled, selecting the next process using the Lottery scheduling algorithm and executing it for up to the time quantum using the Round Robin scheduling algorithm. The function returns a list of all processes in the order they were completed, with their first run and completion times updated to reflect when they were actually run by the scheduler.

The Process struct represents a process, with members for the arrival time, first run time, duration, completion time, job type, process ID, and number of lottery tickets.



Here is a link for video:
