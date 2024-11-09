#include "types.h"
#include "stat.h"
#include "user.h"

int main() {
    int pid1 = fork();
    if (pid1 == 0) {
        nice(getpid(), 3); // Medium priority
        printf(1, "first process with medium priority\n");
        
        char *args[] = { "prime", 0 };
        if (exec("prime", args) < 0) {
            printf(1, "Error: exec failed for prime\n");
        }
        exit();
    }

    int pid2 = fork();
    if (pid2 == 0) {
        nice(getpid(), 3); // Same medium priority
        printf(1, "second process with medium priority\n");
        
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