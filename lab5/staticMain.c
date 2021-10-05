#include "sharedRealization.c"
#include <stdio.h>

int main() {
    int action = 0;
    while(scanf("%d", &action)) {
        if(action == 1) {
            float A, deltaX;
            scanf("%f%f", &A, &deltaX);
            float result = Derivative(A, deltaX);
            printf("Derivative is %f!\n", result);
        }
        else if(action == 2) {
            float A, B;
            scanf("%f%f", &A, &B);
            float result = Square(A, B);
            printf("Square is %f!\n", result);
        }
        else {
            printf("Wrong action format!\n");
            return -1;
        }
    }
}

