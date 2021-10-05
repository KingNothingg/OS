#include <math.h>
#include "sharedRealization.c"

float Derivative(float A, float deltaX) {
    float result = (cosf(A+deltaX) - cosf(A - deltaX))/(2*deltaX);
    return result;
}

float Square(float A, float B) {
    float result = (A*B)/2;
    return result;
}

