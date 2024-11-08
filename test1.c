#include "types.h"
#include "stat.h"
#include "user.h"

void test_specific_pid() {
    printf(1, "Test 1: Change nice value for PID 1\n");
    nice(1,3); //setting nice value of PID 1 to default (3)
    int result = nice(1, 4);
    printf(1, "Expected:\tPID 1, Old value 3 (default is 3)\n");
    printf(1, "Actual:\t\tPID 1, Old value %d\n\n", result);
}

void test_current_process() {
    printf(1, "Test 2: Change nice value for current process\n");
    int pid = getpid();
    int result = nice(pid, 2);
    printf(1, "Expected:\tPID %d, Old value 3 (default is 3)\n", pid);
    printf(1, "Actual:\t\tPID %d, Old value %d\n\n", pid, result);
}

void test_out_of_bounds_lower() {
    printf(1, "Test 3: Set out-of-bounds nice value (lower)\n");
    int result = nice(1, 0);
    printf(1, "Expected:\tError: Nice value must be between 1 and 5\n");
    printf(1, "Actual:\t\t%s\n\n", 
           (result == -2) ? "Error: Nice value must be between 1 and 5" : 
           (result < 0) ? "Error: Invalid PID or Value" : "Unexpected success");
}

void test_out_of_bounds_upper() {
    printf(1, "Test 4: Set out-of-bounds nice value (upper)\n");
    int result = nice(1, 6);
    printf(1, "Expected:\tError: Nice value must be between 1 and 5\n");
    printf(1, "Actual:\t\t%s\n\n", 
           (result == -2) ? "Error: Nice value must be between 1 and 5" : 
           (result < 0) ? "Error: Invalid PID or Value" : "Unexpected success");
}

void test_nonexistent_pid() {
    printf(1, "Test 5: Change nice value for non-existent PID\n");
    int result = nice(9999, 3);
    printf(1, "Expected:\tError: Invalid PID or Value\n");
    printf(1, "Actual:\t\t%s\n\n", (result < 0) ? "Error: Invalid PID or Value" : "Unexpected success");
}

void test_multiple_changes() {
    printf(1, "Test 6: Change nice value multiple times\n");
    int pid = getpid();
    int result1 = nice(pid, 2);
    int result2 = nice(pid, 4);
    printf(1, "Expected:\tFirst change - PID %d, Old value 3; Second change - PID %d, Old value 2\n", pid, pid);
    printf(1, "Actual:\t\tFirst change - PID %d, Old value %d; Second change - PID %d, Old value %d\n\n", pid, result1, pid, result2);
}

void run_test(void (*test_func)()) {
    if (fork() == 0) {
        test_func();
        exit();
    } else {
        wait();
    }
}

int main() {
    printf(1, "Running nice system call tests\n\n");

    run_test(test_specific_pid);
    run_test(test_current_process);
    run_test(test_out_of_bounds_lower);
    run_test(test_out_of_bounds_upper); 
    run_test(test_nonexistent_pid);
    run_test(test_multiple_changes);
    
    printf(1, "All tests completed\n");
    exit();
}