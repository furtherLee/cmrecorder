#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "cmrecorder.h"

static char* version = "CPU Memory Recorder 0.1";
static char* license = "GPL2";

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
  // TODO
  return 1;
}

static int set_time(const char* opt){
  // TODO
  return 1;
}

static int set_output(const char* opt){
  // TODO
  return 1;
}

static int set_pids_by_command(const char* opt){
  // TODO
  return 1;
}

static int set_pids(const char* opt){
  // TODO
  return 1;
}

static struct record_param* global_param = NULL;

static void global_init(){
  global_param = (struct record_param*)malloc(sizeof(struct record_param));
  global_param->total_time = -1;
  // TODO register signal
}

static void global_deinit(){
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
