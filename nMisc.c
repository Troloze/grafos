#include "nMisc.h"

static void UCIGetOrder(UCI * in) {
    int p1, p2, v, aux;
    for (int i = 0; i < in->limit; i++) {
        in->order[i] = (in->configuration[i] <= 1) ? -1 : i;
    }

    v = in->configuration[0];
    p1 = 0;
    p2 = -1;
    for (int i = 1; i < in->limit; i++) {
        
        if (v == in->configuration[i]) {
            p2 = i;
            continue;
        }
        if (p2 != -1) {
            while (p2 > p1) {
                aux = in->order[p1];
                in->order[p1] = in->order[p2];
                in->order[p2] = aux;
                p1++;
                p2--;
            }
        }
        p1 = i;
        p2 = -1;
        v = in->configuration[i];
        if (v <= 1) break;
    }

    if (p2 != -1) { // Caso o último valor faça parte de uma sequência igual.
        while (p2 > p1) {
            aux = in->order[p1];
            in->order[p1] = in->order[p2];
            in->order[p2] = aux;
            p1++;
            p2--;
        }
    }

    return;
}

static void UCIGetValues(UCI * in) {
    int c;
    for (int i = 0; i < in->size; i++) {
        in->values[i] = -1;
    }
    for (int i = 0; i < in->limit; i++) {
        for (int j = 0; j < in->configuration[i]; j++) {
            c = 0;
            while (in->values[j + in->pointers[i] + c] != -1) {
                c++;
            }
            in->values[j + in->pointers[i] + c] = i;
        }
    }
    return;
}

static void UCIResetBase(UCI * in) {
    in->maxValues[0] = in->size;
    in->pointers[0] = 0;
    for (int i = 0; i < in->limit - 1; i++) {
        in->maxValues[i + 1] = in->maxValues[i] - in->configuration[i];
        in->pointers[i + 1] = in->pointers[i] + in->configuration[i];
    }
    for (int i = 0; i < in->limit; i++) {
        for (int j = 0; j < in->configuration[i]; j++) {
            in->valueConfig[j + in->pointers[i]] = j;
        }
    }
    UCIGetOrder(in);
    UCIGetValues(in);
}

static int UCIIterateConfig(UCI * in) {
    if (!in) return -1;
    if (in->limit == 1) return 1;
    int pointer = 0, counter, value, step = 0;
    
    while (in->configuration[pointer] <= in->configuration[pointer + 1] + 1) {
        if (in->configuration[pointer] == in->configuration[pointer + 1] + 1) {
            if (step == 0) {
                step++;
                pointer++;
                continue;
            }
            break;
        }
        pointer++;
    }
    pointer++;
    if (pointer >= in->limit) return 1;
    in->configuration[pointer]++;
    value = in->configuration[pointer];
    counter = in->size;

    for (int i = pointer; i < in->limit; i++) {
        counter -= in->configuration[i];
    }
    for (int i = 0; i < pointer; i++) {
        counter -= value;
        in->configuration[i] = value;
    }
    in->configuration[0] += counter;
    UCIResetBase(in);
    return 0;
}

static int UCIIterateBaseLine(UCI * in, int line, int upper, int lower) {
    int pointer;
    for (int i = 0; i < in->configuration[line]; i++) {
        pointer = (in->configuration[line] - 1 - i) + in->pointers[line];
        if (in->valueConfig[pointer] == in->maxValues[line] - 1 - i) {
            continue;
        }
        
        return 0;
    }
}

static int UCIIterateBase(UCI * in) {
    int l = -1, v = -1, j;
    if (in->limit == 1) return 1;


    for (int i = 0; i < in->limit; i++) {
        if (in->configuration[i] <= 1) {

            return 1;
        }
        if (!UCIIterateBaseLine(in, i, 0, 0)) {

            return 0;
        }
        v = in->configuration[i];
    }

    return 1;
}

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

int iterateUCI(UCI * in) {
    //*/
    if (UCIIterateBase(in)) 
        if (UCIIterateConfig(in)) 
            return 1;
    /*/
    int i = UCIIterateConfig(in);
    if (i) UCIGetValues(in);
    return i;
    //*/
    UCIGetValues(in);
    return 0;
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

UCI * createUCI(int k, int n, int zero, int extra) {
    UCI * ret = malloc(sizeof(UCI));
    if (extra) k++;
    int counter = n;

    ret->size = n;
    ret->limit = k;
    ret->configuration = malloc(sizeof(int) * k);
    ret->pointers = malloc(sizeof(int) * k + 1);
    ret->maxValues = malloc(sizeof(int) *  k);
    ret->valueConfig = malloc(sizeof(int) * n);
    ret->order = malloc(sizeof(int) * k);
    ret->values = malloc(sizeof(int) * n);
    

    if (extra) k--;
   
    for (int i = 0; i < k; i++) {
        ret->order[i] = -1;
        if (!zero) {
            ret->configuration[i] = 1;
            if (--counter == 0) zero = 1;
            continue;
        }
        ret->configuration[i] = 0;
    }

    if (extra) ret->configuration[k] = 0;

    ret->configuration[0] += counter;
    ret->maxValues[0] = n;
    ret->pointers[0] = 0;
    
    for (int i = 0; i < ret->limit - 1; i++) {
        ret->maxValues[i + 1] = ret->maxValues[i] - ret->configuration[i];
        ret->pointers[i + 1] = ret->pointers[i] + ret->configuration[i];
    }

    ret->pointers[k] = n;
    
    for (int i = 0; i < ret->limit; i++) {
        for (int j = 0; j < ret->configuration[i]; j++) {
            ret->valueConfig[j + ret->pointers[i]] = j;
        }
    }
    UCIGetOrder(ret);
    UCIGetValues(ret);
    
    return ret;
}

void destroyUCI(UCI * in) {
    free(in->valueConfig);
    free(in->pointers);
    free(in->maxValues);
    free(in->configuration);
    free(in->order);
    free(in->values);
    free(in);
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
            if (i%20 == 19) printf("\n         ");
        }
    }

    printf("Locked: [");
    if (it->lockedSize == 0) printf("]\n");
    for (int i = 0; i < it->lockedSize; i++) {
        printf("%d", it->lockedValues[i]);
        if (i == it->lockedSize - 1) printf("]\n");
        else {
            printf(", ");
            if (i%20 == 19) printf("\n         ");
        }
    }
    return;
}

void printUCI(UCI * in, int printDetail, int shorten) {
    int pointer, validator;
    printf("UCI\nSize: %d, Limit: %d\n", in->size, in->limit);
    printf("Config: [");
    for (int i = 0; i < in->limit; i++) {
        printf("%d", in->configuration[i]);
        if (i == in->limit - 1) printf("]\n");
        else {
            printf(", ");
            if (i%20 == 19) printf("\n         ");
        }
    }
    if (shorten) return;
    printf("MaxVal: [");
    for (int i = 0; i < in->limit; i++) {
        printf("%d", in->maxValues[i]);
        if (i == in->limit - 1) printf("]\n");
        else {
            printf(", ");
            if (i%20 == 19) printf("\n         ");
        }
    }
    printf("Points: [");
    for (int i = 0; i <= in->limit; i++) {
        printf("%d", in->pointers[i]);
        if (i == in->limit) printf("]\n");
        else {
            printf(", ");
            if (i%20 == 19) printf("\n         ");
        }
    }
    printf("Order : [");
    for (int i = 0; i < in->limit; i++) {
        printf("%d", in->order[i]);
        if (i == in->limit - 1) printf("]\n");
        else {
            printf(", ");
            if (i%20 == 19) printf("\n         ");
        }
    }
    
    if (!printDetail) {
        printf("VConfig: [");
        for (int i = 0; i < in->size; i++) {
            printf("%d", in->valueConfig[i]);
            if (i == in->size - 1) printf("]\n");
            else {
                printf(", ");
                if (i%20 == 19) printf("\n         ");
            }
        }
    }
    else {
        printf("VConfg:\n");
        for (int i = 0; i < in->limit; i++) {
            if (in->configuration[i] == 0) {
                printf("(%d)\n", i);
                continue;
            }
            printf("(%d) [", i);
            pointer = 0;
            for (int j = 0; j < in->maxValues[i]; j++) {
                if (pointer >= in->configuration[i]) {
                    printf("0");
                }
                else {
                    if (j != in->valueConfig[pointer]) {
                        printf("0");
                    } 
                    else {
                        printf("1");
                        pointer++;
                    }
                }
                if (j == in->maxValues[i] - 1) printf("]\n");
                else {
                    printf(", ");
                    if (j%20 == 19) {
                        printf("\n     ");
                        if (i == 0) continue;
                        validator = (int) log10(i);
                        for (int k = 0; k < validator; k++) printf(" ");
                    }
                }
            }
        } 
    }

    printf("Values: [");
    for (int i = 0; i < in->size; i++) {
        printf("%d", in->values[i]);
        if (i == in->size - 1) printf("]\n");
        else {
            printf(", ");
            if (i%20 == 19) printf("\n         ");
        }
    }
}
