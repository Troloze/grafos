#ifndef TRO_MISC
#define TRO_MISC

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct iterator {
    int unlocked;
    int size;
    int maxValue;
    int lockedSize;
    int * values;
    int * lockedValues;
} iterator;

typedef struct uniqueConfigurationIterator {
    int size;
    int maxValue;
    int * configuration;
    int * valueSize;
    int * currentValues;
    int ** values;
} UCI;


int iterate(iterator * it);
int lockValue(iterator * it, int valuePos, int value);
int unlockValue(iterator * it, int valuePos);

void printIterator(iterator * it);

iterator * createIterator(int size, int * startingValues, int maxValue);
void destroyIterator(iterator * it);

#endif