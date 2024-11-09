#include "types.h"
#include "stat.h"
#include "user.h"

int main() {
    int pid1 = fork();
    if (pid1 == 0) {
        nice(getpid(), 1); // Higher priority (lower nice value)
        printf(1, "higher priority\n");
        
        char *args[] = { "prime", 0 };
        if (exec("prime", args) < 0) {
            printf(1, "Error: exec failed for prime\n");
        }
        exit();
    }

    int pid2 = fork();
    if (pid2 == 0) {
        nice(getpid(), 5); // Lower priority
        printf(1, "lower priority\n");
        
        char *args[] = { "prime", 0 };
        if (exec("prime", args) < 0) {
            printf(1, "Error: exec failed for prime\n");
        }
        exit();
    }

    wait();
    wait();
    exit();
}