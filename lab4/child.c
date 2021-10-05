#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>

const size_t mapSize = 4096;

const char *BackingFile = "lab4.back";
const char *SemaphoreName = "lab4.semaphore";

void SendResult(caddr_t memMap, sem_t *semaphore, double result) {
    while(true) {
        if (sem_wait(semaphore) == 0) {
            if(memMap[0] != '\0') {
                if (sem_post(semaphore) != 0) {
                    perror("Can't wake up");
                    exit(EXIT_FAILURE);
                }
                continue;
            }
            sprintf(memMap, "%lf\n", result);
            if (sem_post(semaphore) != 0) {
                perror("Can't wake up");
                exit(EXIT_FAILURE);
            }
            break;
        } else {
            perror("Can't go down");
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv) {
    double num, result = 0;
    char tempChar;
    if (argc != 2) {
        perror("Filename is not given to child");
        exit(EXIT_FAILURE);
    }
    const char *fileName = argv[1];
    FILE *file = fopen(fileName, "r");
    int mapDes = shm_open(BackingFile, O_RDWR, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    if (mapDes < 0) {
        perror("Can't open shared mem obj");
        exit(EXIT_FAILURE);
    }
    caddr_t memMap = mmap(NULL, mapSize, PROT_READ | PROT_WRITE, MAP_SHARED, mapDes, 0);
    if (memMap == MAP_FAILED) {
        perror("Can't open mem map");
        exit(EXIT_FAILURE);
    }
    sem_t *semaphore = sem_open(SemaphoreName, O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH, 2);
    if (semaphore == SEM_FAILED) {
        perror("Can't open a semaphore");
        exit(EXIT_FAILURE);
    }
    if (sem_wait(semaphore) != 0) {
        perror("Can't go down");
        exit(EXIT_FAILURE);
    }
    while(fscanf(file,"%lf%c", &num, &tempChar) != EOF) {
        result += num;
        if (tempChar == '\n') {
            SendResult(memMap, semaphore, result);
            result = 0;
            continue;
        }
    }
    if (result != 0) {
        SendResult(memMap, semaphore, result);
    }
    while (true) {
        if (sem_wait(semaphore) == 0) {
            if(memMap[0] != '\0') {
                if (sem_post(semaphore) != 0) {
                    perror("Cant wake up");
                    exit(EXIT_FAILURE);
                }
                continue;
            }
            memMap[0] = EOF;
            if (sem_post(semaphore) != 0) {
                perror("Cant wake up");
                exit(EXIT_FAILURE);
            }
            break;
        } else {
            perror("Can't go down");
            exit(EXIT_FAILURE);
        }
    }
    if (munmap(memMap, mapSize) != 0) {
        perror("Can't unmap");
        exit(EXIT_FAILURE);
    }
    if (sem_close(semaphore) != 0) {
        perror("Can't close sem");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}