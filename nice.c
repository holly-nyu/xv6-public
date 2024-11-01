#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int pid, value;
  
  if(argc < 2 || argc > 3){
    printf(2, "Usage: nice pid value OR nice value\n");
    exit();
  }
  
  if(argc == 2){
    pid = 0;  // 0 means current process
    value = atoi(argv[1]);
  } else {
    pid = atoi(argv[1]);
    value = atoi(argv[2]);
  }
  
  int old_value = nice(pid, value);
  
  if(old_value < 0){
    printf(2, "Error: Invalid PID or Value\n");
    exit();
  }
  
  if(pid == 0) {
    pid = getpid();  // Get current process ID for output
  }
  
  printf(1, "%d %d\n", pid, old_value);
  
  exit();
}