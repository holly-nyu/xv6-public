# Priority-Based Scheduling in xv6

This project implements a priority-based scheduling system in xv6, including a `nice` system call to adjust process priorities.

## Setup
The priority flag in `params.h` must be set to `0` for Round Robin, or `1` for our priority scheduler.

## Files to Run

1. `test1.c`: Tests the nice system call
2. `prime.c`: Helper program that prints prime numbers (used by test programs)
3. `test1p.c`: Tests two processes with different priorities
4. `test2p.c`: Tests three processes with different priorities
5. `test3p.c`: Tests two processes with the same priority

## How to Run

1. Compile xv6 with the modified files:
```
make
```

2. Run xv6 in QEMU:
```
make qemu-nox
```

3. Once in the xv6 shell, run the test programs:
```
test1
test1p
test2p
test3p
```

4. To test the `nice` system call directly:
```
nice <pid> <value>
```
or
```
nice <value>
```

## Explanation

The PDF file accompanying this README includes detailed explanations and screenshots of the test cases. It covers:

1. The implementation of the priority-based scheduler
2. The `nice` system call implementation
3. Detailed analysis of the output from `test1p`, `test2p`, and `test3p`
4. Observations on how process priorities affect CPU time allocation

Please refer to the PDF for a comprehensive understanding of the project implementation and test results.
