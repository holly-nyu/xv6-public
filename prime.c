#include "types.h"
#include "stat.h"
#include "user.h"

int is_prime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void print_primes() {
    for (int i = 1; i <= 100; i++) {
        if (is_prime(i)) {
            printf(1, "%d ", i);
            // Add a small delay to allow scheduler time to switch between processes
            for (volatile int j = 0; j < 1000000; j++); // Simple delay
        }
    }
    printf(1, "\n");
}

int main() {
    print_primes();
    exit();
}