#ifndef TRO_MISC
#define TRO_MISC

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))


typedef struct _ll linkedList;
struct _ll {
    int id;
    int value;
    linkedList * next;
    linkedList * prev;
};

typedef struct iterator {
    int unlocked;
    int size;
    int maxValue;
    int lockedSize;
    int * values;
    int * lockedValues;
} iterator;

typedef struct bitmap {
    unsigned int * bits;
    int size;
} bitmap;

typedef struct uniqueConfigurationIterator {
    int size;
    int limit;
    int used;
    int * configuration;
    int * maxValues;
    int * valueConfig;
    int * pointers;
    int * values;
    bitmap ** valueMap;
    linkedList * list;
    linkedList ** auxList;
} UCI;



int iterate(iterator * it);
int iterateUCI(UCI * in);

int lockValue(iterator * it, int valuePos, int value);
int unlockValue(iterator * it, int valuePos);

void printIterator(iterator * it);
char * IteratorToString(iterator * it);
void printUCI(UCI * in, int printDetail, int shorten);
char * UCIToString(UCI * in);

iterator * createIterator(int size, int * startingValues, int maxValue);
iterator * cloneIterator(iterator * it);
void destroyIterator(iterator * it);

UCI * createUCI(int k, int n, int zero, int extra);
UCI * cloneUCI(UCI * in);
void UCIResetBase(UCI * in);
int * UCIGetValues(UCI * in);
void UCIGetValues2(UCI * in);
void destroyUCI(UCI * in);

bitmap * createBitmap(int size);
void destroyBitmap(bitmap * bm);
void setBit(bitmap * bm, int pos, int v);
void setBitTrue(bitmap * bm, int pos);
void setBitFalse(bitmap * bm, int pos);
int getBit(bitmap * bm, int pos);
void bitmapXor(bitmap * b1, bitmap * b2, bitmap ** out);
void bitmapOr(bitmap * b1, bitmap * b2, bitmap ** out);
void bitmapAnd(bitmap * b1, bitmap * b2, bitmap ** out);
void bitmapNXor(bitmap * b1, bitmap * b2, bitmap ** out);
void bitmapNOr(bitmap * b1, bitmap * b2, bitmap ** out);
void bitmapNAnd(bitmap * b1, bitmap * b2, bitmap ** out);
void bitmapNot(bitmap * bm);
int isBitmapZero(bitmap * bm);
void resetBit(bitmap * bm);
void printBitmap(bitmap * bm);

void listRemove(linkedList * in);

#endif