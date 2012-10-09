#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include "cmrecorder.h"

#define MAX_PIDS 32

static char* version = "CPU Memory Recorder 0.1";
static char* license = "GPL2";
static struct record_param* global_param = NULL;

static void usage(){
  printf("%s\n%s\n"
	 "usage:\n"
	 " cmrecord -t<time> -i<interval> -c<command>|-p<pids> -o<output>\n"
	 "  -t = total recording time(s)\n"
	 "  -i = recording interval(s)\n"
	 "  -o = output file\n"
	 "  -p = pids seperated by .\n"
	 "  -c = tracing command\n",
	 version,
	 license
	 );
}

static int set_interval(const char* opt){
  global_param->record_interval = atof(opt);
  if (global_param->record_interval > 0.0)
    return 1;
  else
    return 0;
}

static int set_time(const char* opt){
  global_param->total_time = atof(opt);
  if (global_param->total_time > 0.0)
    return 1;
  else
    return 0;
}

static int set_output(char* opt){
  global_param->output_path = opt;
  return 1;
}

static int set_pids_by_command(const char* opt){
  char line[512];
  char cmd[512];
  sprintf(cmd, "pidof %s", opt);
  FILE *fd = popen(cmd, "r");
  if(!fgets(line, 512, fd)){
    pclose(fd);
    return 0;
  };
  char *p;
  for (p = strtok(line, " "); p != NULL; p = strtok(NULL, " "))
    global_param->pids[global_param->pid_num++] = strtoul(p, NULL, 10);
  pclose(fd);
  return 1;
}

static int set_pids(char* opt){
  char* p;
  for (p = strtok(opt, "."); p != NULL; p = strtok(NULL, "."))
    global_param->pids[global_param->pid_num++] = strtoul(p, NULL, 10);
  
  return 1;
}


static void global_init(){
  global_param = (struct record_param*)malloc(sizeof(struct record_param));
  global_param->total_time = -1;
  global_param->pids = (unsigned long *)malloc(MAX_PIDS*sizeof(unsigned long));
  global_param->pid_num = 0;
  // TODO register signal
}

static void global_deinit(){
  free(global_param->pids);
  free(global_param);
}

int main(int argc, char* argv[]){
  int exitcode = 0;
  char c;

  char pids_filled = 0;
  char interval_filled = 0;
  char output_filled = 0;
    
  global_init();
  
  while ((c = getopt(argc, argv, "t:i:c:p:o:")) != -1)
    switch (c){
    case 't':
      if (!set_time(optarg))
	goto error;

      break;

    case 'i':
      if (!set_interval(optarg))
	goto error;

      interval_filled++;
      break;

    case 'c':
      if (pids_filled)
	goto error;

      if (!set_pids_by_command(optarg))
	goto error;

      pids_filled++;
      break;

    case 'p':

      if (pids_filled)
	goto error;

      if (!set_pids(optarg))
	goto error;

      pids_filled++;
      break;

    case 'o':
      if (!set_output(optarg))
	goto error;

      output_filled++;
      break;

    case '?':
      if (optopt == 't' || optopt == 'i' || optopt == 'c' || optopt == 'p')
	printf("Option -%c requires an argument.\n", optopt);
      else
	printf("Unknown option -%c.\n", optopt);
      usage();
      goto error;
      break;
    default:
      usage();
      exitcode = 0;
      goto out;
    }
  
  if (!(pids_filled & interval_filled & output_filled)){
    printf("Some required fields are not set!\n");
    usage();
    goto error;
  }

  return start_record(global_param);

 error:
  exitcode = -1;
  global_deinit();

 out:
  return exitcode;
}
