#include "types.h"
#include "stat.h"
#include "user.h"

#define MIN_NICE 1
#define MAX_NICE 5

int
main(int argc, char *argv[])
{
  int pid, value;
  
  if(argc < 2 || argc > 3){
    printf(2, "Usage: nice pid value OR nice value\n");
    exit();
  }
  
  if(argc == 2){
    pid = getpid(); // Get current process ID
    value = atoi(argv[1]);
  } else {
    pid = atoi(argv[1]);
    value = atoi(argv[2]);
  }

  // Validate nice value
  if(value < MIN_NICE || value > MAX_NICE) {
    printf(2, "Error: Nice value must be between %d and %d\n", MIN_NICE, MAX_NICE);
    exit();
  }

  int old_value = nice(pid, value);
  
  if(old_value < 0){
    printf(2, "Error: Invalid PID\n");
    exit();
  }
  
  printf(1, "%d %d\n", pid, old_value);
  exit();
}