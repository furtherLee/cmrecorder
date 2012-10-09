#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include "cmrecorder.h"

struct{
  FILE* logger;
  unsigned long total_mem;
  
}env;

static void setup_logger(const char* path){
  env.logger = fopen(path, "w");
  struct sysinfo si;
  sysinfo(&si);
  env.total_mem = si.totalram;
}

static void output_overall_info(){
  // TODO
}

static void output_one_result(unsigned long mem, double cpu){
  // TODO
}

static void close_logger(){
  fclose(env.logger);
}

int start_record(struct record_param* param){
  
  setup_logger(param->output_path);
  output_overall_info();
  // TODO

  close_logger();
  return 0;
}
