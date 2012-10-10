#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include "cmrecorder.h"

struct pinfo{
  unsigned long pid;
  unsigned long utime;
  unsigned long stime;
  unsigned long mem;
};

struct {
  FILE* logger;
  unsigned long total_mem;
  int counter;
  int pid_num;
  unsigned long last_total_time;
  struct pinfo* last_infos;
  int page_size;
} env;

static unsigned long get_current_total_time(){
  FILE *fd = fopen("/proc/stat", "r");
  char line[512];
  char cpu[10];
  unsigned long t0, t1, t2, t3, t4, t5, t6, t7, t8, t9;
  fgets(line, 512, fd);
  sscanf(line, "%s%lu%lu%lu%lu%lu%lu%lu%lu%lu%lu", cpu, &t0, &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9);
  unsigned long ret = t0 + t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9;
  fclose(fd);
  return ret;
}

static void get_current_pinfo(struct pinfo* info, unsigned long pid){
  info->pid = pid;
  char path[512];
  char line[512];
  char *p;
  int counter = 0;
  unsigned long temp;

  sprintf(path, "/proc/%lu/stat", pid);
  FILE *fd = fopen(path, "r");
  if(!fgets(line, 512, fd)){
    fclose(fd);
    return;
  }
  for (p = strtok(line, " "); counter < 14; p = strtok(NULL, " "), counter++);
  info->utime = strtoul(p, NULL, 10);
  p = strtok(NULL, " ");
  info->stime = strtoul(p, NULL, 10);
  fclose(fd);

  sprintf(path, "/proc/%lu/statm", pid);
  fd = fopen(path, "r");
  if(!fgets(line, 512, fd)){
    fclose(fd);
    return;
  }
  sscanf(line, "%lu%lu", &temp, &info->mem);
  info->mem *= env.page_size;
  fclose(fd);
}

static void init(const struct record_param* param){
  struct sysinfo si;
  int i;

  env.logger = fopen(param->output_path, "w");
  sysinfo(&si);
  env.total_mem = si.totalram;
  env.counter = 0;
  env.pid_num = param->pid_num;
  env.last_infos = (struct pinfo*)malloc(env.pid_num * sizeof(struct pinfo));
  for (i = 0; i < env.pid_num; ++i)
    get_current_pinfo(&env.last_infos[i], param->pids[i]);
  env.last_total_time = get_current_total_time();
  env.page_size = getpagesize();
}

static void deinit(){
  free(env.last_infos);
  fclose(env.logger);
}

static void output_overall_info(){
  fprintf(env.logger, "Total Memory(Bytes)=%lu\n", env.total_mem);
  fprintf(env.logger, "%10s%20s%20s%20s%20s\n", "ID", "Timestamp", "Mem(Bytes)", "Mem%", "CPU%");
}

static double get_current_time(){
  struct timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec + now.tv_usec / 1000000.0;
}

static void output_one_result(unsigned long mem, double cpu){
  double timestamp = get_current_time();
  fprintf(env.logger, "%10d%20.5lf%20lu%20.2lf%20.2lf\n", env.counter++, timestamp, mem, mem*100.0/env.total_mem, cpu);
}

static void record_one(){
  int i;
  unsigned long mem = 0;
  unsigned long cpu = 0;
  struct pinfo tempInfo;

  for (i = 0; i < env.pid_num; ++i){
    get_current_pinfo(&tempInfo, env.last_infos[i].pid);
    mem += tempInfo.mem;
    cpu += tempInfo.utime + tempInfo.stime - env.last_infos[i].utime - env.last_infos[i].stime;
    env.last_infos[i].utime = tempInfo.utime;
    env.last_infos[i].stime = tempInfo.stime;
    env.last_infos[i].mem = tempInfo.mem;
  }
  
  unsigned long current_cpu = get_current_total_time();
  double cpu_rate = cpu * 100.0 / (current_cpu - env.last_total_time);
  
  output_one_result(mem, cpu_rate);
}

int start_record(struct record_param* param){  
  init(param);
  output_overall_info();
  double terminal = get_current_time() + param->total_time;
  struct timespec sleep_interval;
  sleep_interval.tv_sec = (unsigned long)floor(param->record_interval);
  sleep_interval.tv_nsec = (unsigned long)(floor((param->record_interval - floor(param->record_interval)) * 1000000000));
  
  while (get_current_time() < terminal || param->total_time == -1){
    nanosleep(&sleep_interval, NULL);
    record_one();
  }

  deinit();
  return 0;
}
