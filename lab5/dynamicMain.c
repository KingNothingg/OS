#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef enum {
    FIRST,
    SECOND,
} CONTEXT;

CONTEXT currContext = FIRST;

const char* libName1 = "libfirst.so";
const char* libName2 = "libsecond.so";

float (*Derivative)(float, float) = NULL;
float (*Square)(float, float) = NULL;
char *err;
void *libHandle = NULL;

void loadLibs(CONTEXT context){
    const char *name = NULL;
    if(context == FIRST){
        name = libName1;
    } else{
        name = libName2;
    }
    libHandle = dlopen(name, RTLD_LAZY);
    if(!libHandle){
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}

void loadContext(){
    loadLibs(currContext);
    Derivative = (float (*)(float, float))dlsym(libHandle, "Derivative");
    Square = (float (*)(float, float))dlsym(libHandle, "Square");
    if((err = dlerror())) {
        fprintf(stderr, "%s\n", err);
        exit(EXIT_FAILURE);
    }
}
void changeContext(){
    dlclose(libHandle);
    if(currContext == FIRST){
        currContext = SECOND;
    } else {
        currContext = FIRST;
    }
    loadContext();
}

int main() {
    currContext = FIRST;
    loadContext();
    int action = 0;
    while(scanf("%d", &action)) {
        if(action == 0){
            changeContext();
            if(currContext == FIRST){
                printf("Now context is first\n");
            } else{
                printf("Now context is second\n");
            }
            continue;
        }
        else if(action == 1) {
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
            printf("Wrong action format!");
            return -1;
        }
    }
}

