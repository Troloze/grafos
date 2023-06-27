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
    int limit;
    int used;
    int * configuration;
    int * maxValues;
    int * valueConfig;
    int * pointers;
} UCI;



int iterate(iterator * it);
int iterateUCI(UCI * in);

int lockValue(iterator * it, int valuePos, int value);
int unlockValue(iterator * it, int valuePos);

void printIterator(iterator * it);
void printUCI(UCI * in, int printDetail, int shorten);

iterator * createIterator(int size, int * startingValues, int maxValue);
iterator * cloneIterator(iterator * it);
void destroyIterator(iterator * it);

UCI * createUCI(int k, int n, int zero, int extra);
UCI * cloneUCI(UCI * in);
void UCIResetBase(UCI * in);
int * UCIGetValues(UCI * in);
void destroyUCI(UCI * in);

#endif