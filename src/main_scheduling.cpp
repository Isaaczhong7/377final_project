#include <scheduling.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cout << "usage: [fifo|sjf|stcf|rr|mlfq|lo] workload_file" << endl;
    exit(1);
  }

  string algorithm = argv[1];
  string workload_file = argv[2];
  
  pqueue_arrival workload = read_workload(workload_file);

  if (algorithm == "fifo") {
    show_metrics(fifo(workload));
  } else if (algorithm == "sjf") {
    show_metrics(sjf(workload));
  } else if (algorithm == "stcf") {
    show_metrics(stcf(workload));
  } else if (algorithm == "rr") {
    show_metrics(rr(workload));
  } else if (algorithm == "lo") {
    show_metrics(lo(workload));
  }else if (algorithm == "mlfq"){
    int time_quantum[] = {2, 4, 8, 16,32,64,128, 1000};
    int num_queues = 8;
    int boosting_interval = 10;
    show_metrics(mlfq(workload,num_queues,time_quantum,boosting_interval));
  } else {
    cout << "Error: Unknown algorithm: " << algorithm << endl;
    cout << "usage: [fifo|sjf|stcf|rr|mlfq|lo] workload_file" << endl;
    exit(1);
  }

  return 0;
}
