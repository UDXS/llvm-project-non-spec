#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void compareValues(int a, int b) {
    int result = 0;

    if (a != b) {
        srand(time(NULL));
        for (int i = 0; i < 10000; ++i) {
            result += rand();  // Introduce computations using random values
        }
        printf("Values are not equal. Result: %d\n", result);
    } else {
        srand(time(NULL));
        for (int i = 0; i < 5000; ++i) {
            result -= rand();  // Introduce other computations using random values
        }
        printf("Values are equal. Result: %d\n", result);
    }
}

int main() {
    srand(time(NULL));
    int x = rand();
    int y = rand();
    compareValues(x, y);
    return 0;
}