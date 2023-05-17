#include <scheduling.h>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <sstream>
#include <random>
using namespace std;

#define INTERACTIVE 1
#define CPU_INTENSIVE 0

pqueue_arrival read_workload(string filename) {
  pqueue_arrival workload;
  Process p = Process();
  ifstream file(filename);
  if(!file.is_open()) cout << "failed" << endl;
  string line;
  string num1, num2, num3, num4;

  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> dis(1, 100);

  while(getline(file,line)){
    istringstream iss(line);
    if (!(iss >> num1 >> num2 >> num3 >> num4)) { break; }
      p.arrival = stoi(num1);
      p.duration = stoi(num2);
      p.job_type = stoi(num3);
      p.pid = stoi(num4);
      p.lottery_tickets =dis(gen);
      workload.push(p);
      p = Process();
  }
  file.close();
  return workload;
}

void show_workload(pqueue_arrival workload) {
  pqueue_arrival xs = workload;
  cout << "Workload:" << endl;
  while (!xs.empty()) {
    Process p = xs.top();
    cout << '\t' << p.arrival << ' ' << p.duration << endl;
    xs.pop();
  }
}

void show_processes(list<Process> processes) {
  list<Process> xs = processes;
  cout << "Processes:" << endl;
  while (!xs.empty()) {
    Process p = xs.front();
    cout << "\tarrival=" << p.arrival << ", duration=" << p.duration
         << ", first_run=" << p.first_run << ", completion=" << p.completion
         << ", pid=" << p.pid << ", lottoery_number" << p.lottery_tickets
         << endl;
    xs.pop_front();
  }
}

list<Process> fifo(pqueue_arrival workload) {
  list<Process> complete;
  int time = 0;
  while(!workload.empty()){
    Process cur = workload.top();
    workload.pop();
    cur.first_run = time;
    time += cur.duration;
    cur.completion = time;
    complete.push_back(cur);
  }
  return complete;
}

list<Process> sjf(pqueue_arrival workload) {
  list<Process> complete;
  pqueue_duration d;
  Process temp;
  int time = 0;
  while(!workload.empty()){ 
    while(workload.top().arrival <= time && !workload.empty()){
      d.push(workload.top());
      workload.pop();
    }
    
    while(!d.empty()){
      temp = d.top();
      d.pop();
      temp.first_run = time;
      time += temp.duration;
      temp.completion = time;
      complete.push_back(temp);
    }

  }

  return complete;
}

list<Process> stcf(pqueue_arrival workload) {
  list<Process> complete;
  pqueue_duration d;
  Process cur;
  int currentTime = 0;
  while(!workload.empty()|| !d.empty()) {

    while(workload.top().arrival <= currentTime && !workload.empty()){
      cur = workload.top();
      cur.first_run = -1;
      d.push(cur);
      workload.pop();
    }

    if(!d.empty()){
      Process temp = d.top();
      d.pop();
      if(temp.first_run == -1){
        //cout << currentTime << endl;
        temp.first_run = currentTime;
      }
      temp.duration -= 1;
      if(temp.duration == 0){
        temp.completion = currentTime + 1;
        complete.push_back(temp);
      }else{
        d.push(temp);
      }
    }
    currentTime += 1;
  }
  return complete;
}

list<Process> rr(pqueue_arrival workload) {
  list<Process> complete;
  queue<Process> d;
  Process cur;
  int currentTime = 0;
    while(!workload.empty()|| !d.empty()) {

    while(workload.top().arrival <= currentTime && !workload.empty()){
      cur = workload.top();
      cur.first_run = -1;
      d.push(cur);
      workload.pop();
    }
    
    if(!d.empty()){
      Process temp = d.front();
      d.pop();
      if(temp.first_run == -1){
        temp.first_run = currentTime;
      }
      temp.duration -= 1;
      currentTime += 1;
      if(temp.duration == 0){
        temp.completion = currentTime;
        complete.push_back(temp);
      }else{
        d.push(temp);
      }
    }else{
      currentTime += 1;
    }

  }
  return complete;
}

list<Process> mlfq(pqueue_arrival workload, int num_queues, int time_quantum[], int boosting_interval) {
  list<Process> complete;
  queue<Process> queues[num_queues];
  int current_time = 0;
  int time_quantum_remaining[num_queues];
  int before = 0;
  int boost = boosting_interval;

  // Initialize time_quantum_remaining array
  for (int i = 0; i < num_queues; ++i) {
    time_quantum_remaining[i] = time_quantum[i];
  }

  // Function to find the next non-empty queue
  auto find_next_queue = [&]() -> int {
    for (int i = 0; i < num_queues; ++i) {
      if (!queues[i].empty()) {
        return i;
      }
    }
    return -1;
  };
  
  while (!workload.empty() || find_next_queue() != -1) {
    // Priority boost
    if (boost == 0) {
      cout << current_time << endl;
      for (int i = 1; i < num_queues; ++i) {
        while (!queues[i].empty()) {
          Process boosted_process = queues[i].front();
          queues[i].pop();
          boosted_process.priority = 0;
          queues[boosted_process.priority].push(boosted_process);
        }
      }
      //reset boost to boosting_interval
      boost = boosting_interval;
    }
    
    // Add new processes to the queue
    while (!workload.empty() && workload.top().arrival <= current_time) {
      Process cur = workload.top();
      cur.priority = 0; // 1 means interactive, 0 means CPU intensive
      cur.first_run = -1;
      queues[cur.priority].push(cur);
      workload.pop();
    }

    // Handle the process at the front of the queue
    int next_queue = find_next_queue();
    if (next_queue != -1) {

      Process cur = queues[next_queue].front();
      queues[next_queue].pop();

      //if the current process is first run, assign current time to it
      if (cur.first_run == -1) {
        cur.first_run = current_time;
      }


      before = current_time;
      //if the job is INTERACTIVE when allows to run half of the time quantum for its current queue 
      if (cur.job_type == INTERACTIVE) {
        //decrement duration and increment current_time
        if(cur.duration >= (time_quantum_remaining[next_queue] / 2)){
          cur.duration -= time_quantum_remaining[next_queue] / 2;
          current_time += time_quantum_remaining[next_queue] / 2;
        }else{
          current_time += cur.duration;
          cur.duration = 0;        
        }
      //if the job is CPU-intensive when allows to run full of the time quantum for its current queue 
      }else{
        //decrement duration and increment current_time
        if(cur.duration >= time_quantum_remaining[cur.priority]){
        cur.duration -= time_quantum_remaining[cur.priority];
        current_time += time_quantum_remaining[cur.priority];
        }else{
        current_time += cur.duration;
        cur.duration = 0;
        }
      } 

      boost = (current_time - before) > boost ? 0 : boost - (current_time - before);

      //when process completes
      if (cur.duration == 0) {
        cur.completion = current_time + 1;
        complete.push_back(cur);
      } else {
          // Round Robin for the first N-1 levels
          if (next_queue < num_queues - 1) {
            // interactive job allows to work half of time after that reset time quantum for its queue
            if (cur.job_type == INTERACTIVE){
              time_quantum_remaining[next_queue] = time_quantum[next_queue];
            }else{
              /**
               * cpu job allows to work full of time if used up full time slice, it will reset time quantum for its queue
               * and demote to lower priority queue
               */ 
              if(cur.duration >= time_quantum_remaining[next_queue]){
                time_quantum_remaining[next_queue] = 0;
              }else{
                time_quantum_remaining[next_queue] = time_quantum_remaining[next_queue] - cur.duration;
              }

              if (time_quantum_remaining[next_queue] == 0) {
                time_quantum_remaining[next_queue] = time_quantum[next_queue];
                if (next_queue < num_queues - 2) {
                  cur.priority = min(num_queues - 1, cur.priority + 1);
                }
              }
            }
            //last level using First Come First Server(FCFS) algorithm
          }else if (next_queue == num_queues - 1 && time_quantum_remaining[next_queue] > 0) {

            //allows process to run until it is finish
              time_quantum_remaining[next_queue] = time_quantum[next_queue];
          }
        queues[cur.priority].push(cur);
      }
    }

    // Increment the current_time only if the process is running and queues are empty
    int find_process = find_next_queue();
     if(find_process == -1 && !workload.empty()){
      // Fast forward the current_time to the arrival time of the next process
      current_time = max(current_time, workload.top().arrival);
    }
  }
  return complete;
}


int find_lottery_winner(list<Process>& q) {
    int total_tickets = 0;
    //loop through the list to get the total sum of lottery tickets
    for (auto it = q.begin(); it != q.end(); ++it) {
        total_tickets += it->lottery_tickets;
    }

    //generate random number from 1-100 using uniform distribution 
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, total_tickets);
    int lucky_ticket = dis(gen);

    int count = 0;
    for (auto it = q.begin(); it != q.end(); ++it) {
        count += it->lottery_tickets;
        if (count >= lucky_ticket) {
            return distance(q.begin(), it);
        }
    }
    return -1;
}

list<Process> lo(pqueue_arrival workload) {
    list<Process> complete;
    list<Process> d;
    Process cur;
    int currentTime = 0;
    int quantum = 2;

    while(!workload.empty() || !d.empty()) {
        while(!workload.empty() && workload.top().arrival <= currentTime){
            cur = workload.top();
            cur.first_run = -1;
            d.push_back(cur);
            workload.pop();
        }
        
        //check if the list is empty
        if(!d.empty()){
          //get the winner index
            int winner_index = find_lottery_winner(d);
            //if the winner index is not -1 which means it has running processes
            if (winner_index != -1) {
                auto it = d.begin();
                // increment the iterator to winner index
                advance(it, winner_index);
                //get the winner process 
                Process temp = *it;
                //delete from list and start to run
                d.erase(it);

                // if it is first run, assign currentTime to process
                if(temp.first_run == -1){
                    temp.first_run = currentTime;
                }

                /**
                 * check if the duration is whether within quantum or not.
                 * if duration is more than quantum; then, it will update duration 
                 * and currentTime; then, push back to the list waiting for scheduling
                 * if duration is within quantum; it will update duration to 0 which means 
                 * the process has finished, and update current time. After that, push it to 
                 * complete list
                 */ 
                if(temp.duration > quantum){
                    temp.duration -= quantum;
                    currentTime += quantum;
                    d.push_back(temp);
                } else {
                    currentTime += temp.duration;
                    temp.duration = 0;
                    temp.completion = currentTime;
                    complete.push_back(temp);
                }
            }

        } else {
          //increment time for when the current process list is empty
            currentTime += quantum;
        }
    }
    return complete;
}

float avg_turnaround(list<Process> processes) {
  float total = 0;
  float sum = 0;
  int size = processes.size();
std::list<Process>::iterator it;
for (it = processes.begin(); it != processes.end(); ++it){
    total = it->completion - it->arrival;
    sum += total;
}
  return sum / size;
}

float avg_response(list<Process> processes) {
  float total = 0;
  float sum = 0;
  int size = processes.size();
std::list<Process>::iterator it;
for (it = processes.begin(); it != processes.end(); ++it){
    total = it->first_run - it->arrival;
    sum += total;
}
  return sum / size;
}

void show_metrics(list<Process> processes) {
  float avg_t = avg_turnaround(processes);
  float avg_r = avg_response(processes);
  show_processes(processes);
  cout << '\n';
  cout << "Average Turnaround Time: " << avg_t << endl;
  cout << "Average Response Time:   " << avg_r << endl;
}
