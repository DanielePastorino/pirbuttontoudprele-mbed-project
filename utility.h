
#include "mbed.h"
#include "string"
#include <cstdio>
#include <cstring>

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

void allLedOff() { led1 = led2 = led3 = 1; }

void allLedOn() { led1 = led2 = led3 = 0; }

void allLedToggle() { led1 = led2 = led3 = !(led1 || led2 || led3); }

/*
char **parseCommandString(char *line){
    char **out = (char **)malloc(sizeof(char *)*2);
    int init_size = strlen(line);
    char delim[] = ";";
    int i = 0;

    char *ptr = strtok(line, delim);
    out[i] = (char *)malloc(sizeof(char)*strlen(ptr));
    strcpy(out[i], ptr);
    while(ptr != NULL){
        //debug("'%s'\n\r", ptr);
        out[i] = (char *)malloc(sizeof(char)*strlen(ptr));
        strcpy(out[i], ptr);
        ptr = strtok(NULL, delim);
        i++;
    }
    return out; 
}

int arrayLength(char **arr){
    int i = 0;
    if (arr != NULL){
        while(arr[i] != NULL){
            i++;
        }
    } 
    return i;
}*/

int charToInt(char c){
    return (int)c - '0';
}

extern "C" void mbed_reset();


