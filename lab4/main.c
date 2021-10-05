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

int main() {
    char *fileName = NULL;
    size_t temp;
    printf("Enter the fileName: ");
    if(getline(&fileName, &temp, stdin) == -1) {
        perror("Can't read the fileName");
        exit(EXIT_FAILURE);
    }
    fileName[strlen(fileName) - 1] = '\0';
    int fileDes = shm_open(BackingFile, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    int testFile = open(fileName, O_RDONLY);
    if (fileDes == -1 || testFile == -1) {
        perror("Can't open file or shared mem object");
        exit(EXIT_FAILURE);
    }
    close(testFile);
    sem_t *semaphore = sem_open(SemaphoreName, O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH, 2);
    if (semaphore == SEM_FAILED) {
        perror("Can't create a semaphore");
        exit(EXIT_FAILURE);
    }
    ftruncate(fileDes, mapSize);
    caddr_t memMap = mmap(NULL, mapSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileDes, 0);
    if (memMap == MAP_FAILED) {
        perror("Can't create mem map");
        exit(EXIT_FAILURE);
    }
    memset(memMap, '\0', mapSize);
    int value;
    if (sem_getvalue(semaphore, &value) != 0) {
        perror("Can't get sem value");
        exit(EXIT_FAILURE);
    }
    while(value++ < 2) {
        sem_post(semaphore);
    }
    pid_t pid = fork();
    if (pid == 0) {
        munmap(memMap, mapSize);
        close(fileDes);
        sem_close(semaphore);
        if (execl("child", "child", fileName, NULL) == -1) {
            perror("Can't execute child proc");
            exit(EXIT_FAILURE);
        }
    } else if (pid == -1) {
        perror("Can't create child proc");
        exit(EXIT_FAILURE);
    }

    while (true) {
        if (sem_getvalue(semaphore, &value) != 0) {
            perror("Can't get sem value");
            exit(EXIT_FAILURE);
        }
        if (value == 2) continue;
        if (sem_wait(semaphore) != 0) {
            perror("Can't go down");
            exit(EXIT_FAILURE);
        }
        if (memMap[0] == EOF) {
            break;
        }
        if (memMap[0] == '\0') {
            if (sem_post(semaphore) != 0) {
                perror("Can't wake up");
                exit(EXIT_FAILURE);
            }
            continue;
        }
        char *buffer = (char*)malloc((strlen(memMap)) * sizeof(char));
        strcpy(buffer, memMap);
        memset(memMap, '\0', mapSize);
        if (sem_post(semaphore) != 0) {
            perror("Can't wake up");
            exit(EXIT_FAILURE);
        }
        printf("%s", buffer);
        free(buffer);
    }
    if (munmap(memMap, mapSize) != 0) {
        perror("Can't unmap");
        exit(EXIT_FAILURE);
    }
    if (sem_close(semaphore) != 0) {
        perror("Can't close sem");
        exit(EXIT_FAILURE);
    }
    close(fileDes);
}