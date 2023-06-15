#include "nMisc.h"

int intCmp(const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int iterate(iterator * it) {
    if (it->maxValue < 2) return -1;
    if (!it) return -1;
    if (it->size <= 0) return -1;
    int pointer = 0;
    int lockPointer = 0;
    while (1) {
        if (!it->unlocked){
            if (lockPointer < it->lockedSize) {
                if (pointer == it->lockedValues[lockPointer]) {
                    if (++pointer == it->size) return 1;
                    lockPointer++;
                    continue;
                }
            }
        }

        if (++it->values[pointer] == it->maxValue) {
            it->values[pointer] = 0;
            if (++pointer == it->size) return 1;
            continue;
        }

        return 0;
    }
}

iterator * createIterator(int size, int * startingValues, int maxValue) {
    iterator * ret = malloc(sizeof(iterator));
    ret->unlocked = 1;
    ret->values = malloc(sizeof(int) * size);
    ret->size = size;
    ret->lockedSize = 0;
    ret->lockedValues = malloc(0);
    ret->maxValue = maxValue;

    if (startingValues == NULL) 
        for (int i = 0; i < size; i++) ret->values[i] = 0;
    else 
        for (int i = 0; i < size; i++) ret->values[i] = startingValues[i];

    return ret;
}

int lockValue(iterator * it, int valuePos, int value) {
    if (valuePos > it->size || valuePos < 0 || !it) return -1;
    int * b = (it->lockedSize > 0) ? bsearch(&valuePos, it->lockedValues, it->lockedSize, sizeof(int), intCmp) : NULL;
    int * n, c;
    
    if (b != NULL) {
        it->values[valuePos] = value;
        return 1;
    }

    if (it->unlocked) {
        it->lockedValues = realloc(it->lockedValues, sizeof(int) * 1);
        it->lockedValues[0] = valuePos;
        it->values[valuePos] = value;
        it->lockedSize++;
        it->unlocked = 0;
        return 0;
    }

    c = 0;
    for (int i = 0; i < it->lockedSize; i++) {
        if (it->lockedValues[i] > valuePos) {
            n = malloc(sizeof(int) * it->lockedSize + 1);
            for (int j = 0; j < it->lockedSize; j++) {
                if (j == i) c++;
                

                n[j + c] = it->lockedValues[j];
            }
            n[i] = valuePos;
            it->lockedSize++;
            it->values[valuePos] = value;
            free(it->lockedValues);
            it->lockedValues = n;
            return 0;
        }
    }
    
    it->lockedValues = realloc(it->lockedValues, sizeof(int) * ++it->lockedSize);
    it->lockedValues[it->lockedSize-1] = valuePos;
    it->values[valuePos] = value;

    return 0;
}   

int unlockValue(iterator * it, int valuePos) {
    if (valuePos > it->size || valuePos < 0 || !it) return -1;
    if (it->unlocked) return 1;
    int * b = (it->lockedSize > 0) ? bsearch(&valuePos, it->lockedValues, it->lockedSize, sizeof(int), intCmp) : NULL;
    int * n, c = 0;
    if (!b) return 1;
    for (int i = 0; i < it->lockedSize; i++) {
        if (it->lockedValues[i] = valuePos) {
            n = malloc(sizeof(int) * --it->lockedSize);

            for (int j = 0; j < it->lockedSize; j++) {
                if (j == i) c++;
                n[j] = it->lockedValues[j + c];
            }

            free(it->lockedValues);
            it->lockedValues = n;
            if (it->lockedSize == 0) it->unlocked = 1;
            return 0;
        }
    }
    return -1;
}

void destroyIterator(iterator * it) {
    if (!it) return;
    free(it->lockedValues);
    free(it->values);
    free(it);
}

void printIterator(iterator * it) {
    printf((it->unlocked) ? "Iterator Unlocked\n" : "Iterator Locked\n");
    printf("Size: %d, LockedSize: %d, MaxValue: %d\n", it->size, it->lockedSize, it->maxValue);
    printf("Values: [");
    if (it->size == 0) printf("]\n");
    for (int i = 0; i < it->size; i++) {
        printf("%d", it->values[i]);
        if (i == it->size - 1) printf("]\n");
        else {
            printf(", ");
            if (i%10 == 9) printf("\n         ");
        }
    }

    printf("Locked: [");
    if (it->lockedSize == 0) printf("]\n");
    for (int i = 0; i < it->lockedSize; i++) {
        printf("%d", it->lockedValues[i]);
        if (i == it->lockedSize - 1) printf("]\n");
        else {
            printf(", ");
            if (i%10 == 9) printf("\n         ");
        }
    }
    return;
}



