#include "nMisc.h"

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

static void UCIResetBaseLine(UCI * in, int line, int index) {
    if (index >= in->maxValues[line]) return;
    for (int i = index + 1; i < in->configuration[line]; i++) {
        in->valueConfig[in->pointers[line] + i] = in->valueConfig[in->pointers[line] + i - 1] + 1;
    }
    return;
}


static int UCIIterateBase(UCI * in) {
    int index, pointer, upperBound, stop = 0, next_val = 0;
    if (in->used == 1) return 1;

    for (int line = 0; line < in->used; line++) {
        if (in->configuration[line] <= 1) {
            return 1;
        }
        upperBound = -1;
        if (line < in->used - 1) {
            if (in->configuration[line + 1] == in->configuration[line])
                upperBound = in->valueConfig[in->pointers[line + 1]];
        }
        for (int i = 0; i < in->configuration[line]; i++) {
            index = (in->configuration[line] - 1 - i);
            pointer = index + in->pointers[line];
            if (in->valueConfig[pointer] == in->maxValues[line] - 1 - i) {
                continue;
            } // Procurar o primeiro valor que não esteja na posição máxima
            if (index == 0) { //Se a posição do valor for 0, ver se está no limite máximo)
                if (in->valueConfig[pointer] == upperBound) {
                    break;
                }
            }
            next_val = in->valueConfig[pointer] + 1;
            stop = 1;
        }
        in->valueConfig[in->pointers[line]] = next_val;
        for (int i = index + 1; i < in->configuration[line]; i++) {
            in->valueConfig[in->pointers[line] + i] = in->valueConfig[in->pointers[line] + i - 1] + 1;
        }
        if (stop) return 0;
    }

    return 1;
}

static int UCIIterateBaseLine(UCI * in, int line) {
    int index, pointer, upperBound = -1;
    
    

    for (int i = 0; i < in->configuration[line]; i++) {
        index = (in->configuration[line] - 1 - i);
        pointer = index + in->pointers[line];
        if (in->valueConfig[pointer] == in->maxValues[line] - 1 - i) {
            continue;
        } // Procurar o primeiro valor que não esteja na posição máxima
        if (index == 0) { //Se a posição do valor for 0, ver se está no limite máximo)
            if (in->valueConfig[pointer] == upperBound) {
                break;
            }
        }
        in->valueConfig[pointer]++;
        UCIResetBaseLine(in, line, index);
        return 0;
    }
   
    in->valueConfig[in->pointers[line]] = 0;
    UCIResetBaseLine(in, line, 0);
    
    return 1;
}

static int _UCIIterateBase(UCI * in) {
    if (in->used == 1) return 1;

    for (int i = 0; i < in->used; i++) {
        if (in->configuration[i] <= 1) {
            return 1;
        }
        if (!UCIIterateBaseLine(in, i)) {
            return 0;
        }
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

iterator * cloneIterator(iterator * it) {
    iterator * ret = createIterator(it->size, it->values, it->maxValue);
    for (int i = 0; i < it->lockedSize; i++) {
        lockValue(ret, it->lockedValues[i], it->values[it->lockedValues[i]]);
    }
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
    int counter = n;

    ret->used = k;
    if (extra) k++;
    ret->size = n;
    ret->limit = k;
    ret->configuration = malloc(sizeof(int) * k);
    ret->pointers = malloc(sizeof(int) * k + 1);
    ret->maxValues = malloc(sizeof(int) *  k);
    ret->valueConfig = malloc(sizeof(int) * n);
    ret->values = malloc(sizeof(int) * n);
    ret->valueMap = NULL;
    ret->list = malloc(sizeof(linkedList) * n);
    ret->auxList = malloc(sizeof(linkedList*) * n);
    if (zero) ret->used = 1;


    if (extra) k--;
   
    for (int i = 0; i < k; i++) {

        if (!zero) {
            ret->configuration[i] = 1;
            if (--counter == 0) zero = 1;
            continue;
        }
        ret->configuration[i] = 0;
    }

    if (extra) ret->configuration[k] = 0;

    ret->configuration[0] += counter;

    UCIResetBase(ret);
    return ret;
}

UCI * cloneUCI(UCI * in) {
    UCI * ret = createUCI(in->limit, in->size, 1, 0);

    for (int i = 0; i < in->limit; i++) {
        ret->configuration[i] = in->configuration[i];
        ret->maxValues[i] = in->maxValues[i];
        ret->pointers[i] = in->pointers[i];
    }

    for (int i = 0; i < in->size; i++) {
        ret->valueConfig = in->valueConfig;
    }

    ret->used = in->used;

    return ret;
}

void UCIResetBase(UCI * in) {
    if (in->valueMap != NULL) {
        for (int i = 0; i < in->used; i++) {
            destroyBitmap(in->valueMap[i]);
        }
        free(in->valueMap);
    }
    in->maxValues[0] = in->size;
    in->pointers[0] = 0;
    in->pointers[in->limit] = in->size;
    in->used = 0;
    for (int i = 0; i < in->limit - 1; i++) {
        in->maxValues[i + 1] = in->maxValues[i] - in->configuration[i];
        in->pointers[i + 1] = in->pointers[i] + in->configuration[i];
    }
    for (int i = 0; i < in->limit; i++) {
        if (in->configuration[i] != 0) in->used++;
        for (int j = 0; j < in->configuration[i]; j++) {
            in->valueConfig[j + in->pointers[i]] = j;
        }
    }

    in->valueMap = malloc(sizeof(bitmap *) * in->used);
    for (int i = 0; i < in->used; i++) {
        in->valueMap[i] = createBitmap(in->size);
    }
}

void destroyUCI(UCI * in) {
    free(in->valueConfig);
    free(in->pointers);
    free(in->maxValues);
    free(in->configuration);
    free(in);
}

int * UCIGetValues(UCI * in) { 
    int * ret, * aux, * swap, c, cAux;
    ret = malloc(sizeof(int) * in->size);
    aux = malloc(sizeof(int) * in->size);
    for (int i = 0; i < in->size; i++) {
        ret[i] = -1;
        aux[i] = -1;
    }
    for (int i = in->limit - 1; i >= 0; i--) {
        c = 0;
        cAux = 0;
        swap = ret;
        ret = aux;
        aux = swap;
        for (int j = 0; j < in->maxValues[i]; j++) {
            if (c < in->configuration[i]) {
                if (in->valueConfig[c + in->pointers[i]] == j) {
                    ret[j] = i;    
                    c++;
                    continue;
                }
            }
            ret[j] = aux[cAux];
            cAux++;
        }
    }
    free(aux);
    return ret;
}

void UCIGetValues2(UCI * in) {
    if (in->size == 0) return;
    int auxInt;
    int * values = in->values;
    linkedList base;
    linkedList * list = in->list;
    linkedList ** auxList = in->auxList;
    base.id = -1;

    

    for (int i = 0; i < in->size; i++) {
        list[i].id = i;
        list[i].value = -1;
        if (i == 0) list[i].prev = &base;
        else list[i].prev = &list[i-1];
        if (i == in->size - 1) list[i].next = NULL;
        else list[i].next = &list[i+1];
        auxList[i] = &list[i];
    }
    base.next = &list[0];

    for (int i = 0; i < in->used; i++) {
        resetBit(in->valueMap[i]);
        for (int j = 0; j < in->configuration[i]; j++) {
            auxInt = in->valueConfig[j + in->pointers[i]];
            setBitTrue(in->valueMap[i], auxList[auxInt]->id);
            auxList[auxInt]->value = i;
            listRemove(auxList[auxInt]);
        }
        auxList[0] = base.next;
        if (auxList[0] == NULL) break;

        for (int j = 1; j < in->maxValues[i + 1]; j++) auxList[j] = auxList[j-1]->next;
    }
    
    for (int i = 0; i < in->size; i++) values[i] = list[i].value;

    return;
}

void listRemove(linkedList * in) {
    if (in->prev != NULL) {
        in->prev->next = in->next;
    }   
    if (in->next != NULL) {
        in->next->prev = in->prev;
    }
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

char * IteratorToString(iterator * it) {
    char * ret = malloc(sizeof(char) * 1024);
    ret = "";
    char buffer[128] = "";
    sprintf(buffer, "size: %d, lockedSize: %d, maxValue: %d\nLocked: ", it->size, it->lockedSize, it->maxValue);
    strcat(ret, buffer);
    for (int i = 0; i < it->lockedSize; i++) {
        if (i != 0) {
            sprintf(buffer, " ");
            strcat(ret, buffer);
        }
        sprintf(buffer, "%d", it->lockedValues[i]);
        strcat(ret, buffer);
    }
    sprintf(buffer, "\nValues: ");
    strcat(ret, buffer);
    for (int i = 0; i < it->size; i++) {
        if (i != 0) {
            sprintf(buffer, " ");
            strcat(ret, buffer);
        }
        sprintf(buffer, "%d", it->values[i]);
        strcat(ret, buffer);
    }
    sprintf(buffer, "\n");
    strcat(ret, buffer);
    return ret;
}

void printUCI(UCI * in, int printDetail, int shorten) {
    int pointer, validator, counter;
    printf("UCI\nSize: %d, Limit: %d, Used: %d\n", in->size, in->limit, in->used);
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
 
    printf("VConfig: [");
    for (int i = 0; i < in->size; i++) {
        printf("%d", in->valueConfig[i]);
        if (i == in->size - 1) printf("]\n");
        else {
            printf(", ");
            if (i%20 == 19) printf("\n         ");
        }
    }
    
    if (printDetail) {
        printf("VConfg:\n");
        for (int i = 0; i < in->limit; i++) {
            if (in->configuration[i] == 0) {
                break;
            }
            printf("(%d) [", i);
            pointer = in->pointers[i];
            counter = 0;
            for (int j = 0; j < in->maxValues[i]; j++) {
                if (counter >= in->configuration[i]) {
                    printf("0");
                }
                else {
                    if (j != in->valueConfig[pointer]) {
                        printf("0");
                    } 
                    else {
                        printf("1");
                        pointer++;
                        counter++;
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
        if (in->used < in->size) {
            if (in->used + 1 != in->size) printf("(...)\n");
            printf("(%d)", in->limit - 1);
        }
    }
}

char * UCIToString(UCI * in) {
    char ret[1024] = "";
    char * ret2;
    char buffer[128] = "";
    sprintf(buffer, "size: %d, limit: %d, used: %d\nConfig: ", in->size, in->limit, in->used);
    strcat(ret, buffer);
    for (int i = 0; i < in->limit; i++) {
        if (i != 0) {
            sprintf(buffer, " ");
            strcat(ret, buffer);
        }
        sprintf(buffer, "%d", in->configuration[i]);
        strcat(ret, buffer);
    }
    sprintf(buffer, "\nValues: ");
    strcat(ret, buffer);
    for (int i = 0; i < in->size; i++) {
        if (i != 0) {
            sprintf(buffer, " ");
            strcat(ret, buffer);
        }
        sprintf(buffer, "%d", in->valueConfig[i]);
        strcat(ret, buffer);
    }
    sprintf(buffer, "\n");
    strcat(ret, buffer);
    ret2 = malloc(sizeof(char) * (1 + strlen(ret)));
    sprintf(ret2, "%s", ret);
    return ret2;
}

bitmap * createBitmap(int size) {
    bitmap * ret = malloc(sizeof(bitmap));
    ret->bits = malloc(sizeof(int) * (size/32));
    ret->size = size;
    resetBit(ret);
    return ret;
}

void destroyBitmap(bitmap * bm) {
    free(bm->bits);
    free(bm);
}

void setBit(bitmap * bm, int pos, int v) {
    if (v) setBitTrue(bm, pos);
    if (!v) setBitFalse(bm, pos);
}

void setBitTrue(bitmap * bm, int pos) {
    if (pos >= bm->size) return;
    bm->bits[pos/32] |= 1 << (pos%32);
}

void setBitFalse(bitmap * bm, int pos) {
    if (pos >= bm->size) return;
    bm->bits[pos/32] &= ~(1 << (pos%32));
}

int getBit(bitmap * bm, int pos) {
    return (bm->bits[pos/32] & 1 << (pos%32)) ? 1 : 0;
}

void bitmapXor(bitmap * b1, bitmap * b2, bitmap ** out) {
    int aux;
    void * swap;
    if (b2->size > b1->size) {
        swap = b2;
        b2 = b1;
        b1 = swap;
    }
    int a, b;
    aux = (b1->size - 1)/32;
    for (int i = 0; i <= aux; i++) {
        a = b1->bits[i];
        if (i > ((b2->size) - 1)/32) b = 0;
        else b = b2->bits[i];
        (*out)->bits[i] = a^b;
    }
    (*out)->bits[(b1->size - 1)/32] &= ~((~0) << (b1->size - 1)%32);
}

void bitmapOr(bitmap * b1, bitmap * b2, bitmap ** out) {
    int aux;
    void * swap;
    if (b2->size > b1->size) {
        swap = b2;
        b2 = b1;
        b1 = swap;
    }
    int a, b;
    aux = (b1->size - 1)/32;
    for (int i = 0; i <= aux; i++) {
        a = b1->bits[i];
        if (i > ((b2->size) - 1)/32) b = 0;
        else b = b2->bits[i];
        (*out)->bits[i] = a|b;
    }
    (*out)->bits[(b1->size - 1)/32] &= ~((~0) << (b1->size - 1)%32);
}

void bitmapAnd(bitmap * b1, bitmap * b2, bitmap ** out) {
    int aux;
    void * swap;
    if (b2->size > b1->size) {
        swap = b2;
        b2 = b1;
        b1 = swap;
    }
    int a, b;
    aux = (b1->size - 1)/32;
    for (int i = 0; i <= aux; i++) {
        a = b1->bits[i];
        if (i > ((b2->size) - 1)/32) b = 0;
        else b = b2->bits[i];
        (*out)->bits[i] = a&b;
    }
    (*out)->bits[(b1->size - 1)/32] &= ~((~0) << (b1->size - 1)%32);
}

void bitmapNXor(bitmap * b1, bitmap * b2, bitmap ** out) {
    int aux;
    void * swap;
    if (b2->size > b1->size) {
        swap = b2;
        b2 = b1;
        b1 = swap;
    }
    int a, b;
    aux = (b1->size - 1)/32;
    for (int i = 0; i <= aux; i++) {
        a = b1->bits[i];
        if (i > ((b2->size) - 1)/32) b = 0;
        else b = b2->bits[i];
        (*out)->bits[i] = ~(a^b);
    }
    (*out)->bits[(b1->size - 1)/32] &= ~((~0) << (b1->size - 1)%32);
}

void bitmapNOr(bitmap * b1, bitmap * b2, bitmap ** out) {
    int aux;
    void * swap;
    if (b2->size > b1->size) {
        swap = b2;
        b2 = b1;
        b1 = swap;
    }
    int a, b;
    aux = (b1->size - 1)/32;
    for (int i = 0; i <= aux; i++) {
        a = b1->bits[i];
        if (i > ((b2->size) - 1)/32) b = 0;
        else b = b2->bits[i];
        (*out)->bits[i] = ~(a|b);
    }
    (*out)->bits[(b1->size - 1)/32] &= ~((~0) << (b1->size - 1)%32);
}

void bitmapNAnd(bitmap * b1, bitmap * b2, bitmap ** out) {
    int aux;
    void * swap;
    if (b2->size > b1->size) {
        swap = b2;
        b2 = b1;
        b1 = swap;
    }
    int a, b;
    aux = (b1->size - 1)/32;
    for (int i = 0; i <= aux; i++) {
        a = b1->bits[i];
        if (i > ((b2->size) - 1)/32) b = 0;
        else b = b2->bits[i];
        (*out)->bits[i] = ~(a&b);
    }
    (*out)->bits[(b1->size - 1)/32] &= ~((~0) << (b1->size - 1)%32);
}

void bitmapNot(bitmap * bm) {
    for (int i = 0; i <= (bm->size - 1)/32; i++) bm->bits[i] = ~bm->bits[i];
    bm->bits[(bm->size - 1)/32] &= ~((~0) << (bm->size - 1)%32);
}

int isBitmapZero(bitmap * bm) {
    bm->bits[(bm->size - 1)/32] &= ~((~0) << (bm->size - 1)%32);
    for (int i = 0; i <= (bm->size - 1)/32; i++) {
        if (bm->bits[i]) return 0;
    }
    return 1;
}

void resetBit(bitmap * bm) {
    int m = (bm->size - 1)/32;
    for (int i = 0; i <= m; i++) {
        bm->bits[i] = 0;
    }
}

void printBitmap(bitmap * bm) {
    int aux;
    printf("Bitmap, size: %d\n", bm->size);
    for (int i = 0; i <= ((bm->size - 1)/32); i++) {
        aux = bm->bits[i];
        printf("[");
        for (int j = 0; j < 32; j++) {
            printf("%d", (aux >> j)&1);
            if (((i == (bm->size - 1)/32) && (j == bm->size%32 - 1)) || (j == 31)) {
                break;
            } 
            printf(", ");
        }
        printf("]\n");
    }
}