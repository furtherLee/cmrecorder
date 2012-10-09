#ifndef _CMRECORDER_H_
#define _CMRECORDER_H_

struct record_param{
  double record_interval;
  double total_time;
  int* pids;
  int pid_num;
  char* output_path;
};

int start_record(struct record_param* param);

#endif
