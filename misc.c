#include "misc.h"

int bitListSorter(const void * a, const void * b) {
    bitList x = *(bitList *)b;
    bitList y = *(bitList *)a;
    return countBits(x) - countBits(y);
}

int intSorter(const void * a, const void * b) {
    return  *(int*)a - *(int*)b;
}


intList makeIntList(int n, int * list) {
    intList retList;
    retList.size = n;
    int * ret = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        ret[i] = list[i];
    }
    retList.data = ret;
    return retList;
}

void freeIntList(intList list) {
    free(list.data);
    return;
}

void freeBitList(bitList bL) {
    free(bL.data);
    return;
}

int compareBitList(bitList a, bitList b) {
    int sizeA = (a.size + 31) >> 5;
    int sizeB = (b.size + 31) >> 5;
    unsigned int check = 0;
    int status = 0;
    int max = (sizeA > sizeB) ? sizeA : sizeB;
    unsigned int x;
    unsigned int y;
    for (int i = 0; i < max; i++) {
        x = ((i < sizeA) ? a.data[i] : 0);
        y = ((i < sizeB) ? b.data[i] : 0);

        check = x ^ y;
        if (check) status |= BITLIST_NOT_EQUALS;

        check = x & y;
        if (check) status |= BITLIST_1_OVERLAP;

        check = x | y;
        if (~check) status |= BITLIST_0_OVERLAP;
    }
    if (!(status & BITLIST_NOT_EQUALS)) status |= BITLIST_EQUALS;
    return status;
}

void invertBitList(bitList a, int s) {
    int dSize = (s + 31) >> 5;
    int sizeInt = (a.size + 31) >> 5;
    int sizeDif = a.size - sizeInt * 32;
    unsigned int mask = ~(0xFFFFFFFF << sizeDif);
    for (int i = 0; i < sizeInt; i++) {
        a.data[i] = ~a.data[i];
        if (i == (sizeInt - 1)) a.data[i] &= mask;
    }
}

bitList overlapBitList(bitList a, bitList b) {
    bitList retList;
    int sizeA = (a.size + 31) >> 5;
    int sizeB = (b.size + 31) >> 5;

    int s = (sizeA > sizeB) ? sizeB : sizeA;
    int bS = (a.size > b.size) ? b.size : a.size;
    retList.size = bS;

    int * val = malloc(sizeof(int) * s);

    for (int i = 0; i < s; i++) val[i] = a.data[i] & b.data[i];
    
    retList.data = val;

    return retList;
}

void setBit(bitList * values, int pos, int type) {
    int n = pos >> 5;
    int k = pos - n*32;
    int _n = (values->size + 31) >> 5;

    if (pos >= values->size) {
        if (n > _n) {
            values->data = realloc(values->data, sizeof(unsigned int) * n);
            values->data[n - 1] = 0;
        }
        values->size = pos + 1;
    }
    unsigned int v = 0x00000001 << k;
    
    if (type == 0) {
        values->data[n] &= ~v;
        return;
    }
    if (type > 0) {
        values->data[n] |= v;
        return;
    }

    values->data[n] = values->data[n] ^ v;
}

int getBit(bitList values, int pos) {
    if (pos >= values.size) return 0;
    int n = pos >> 5;
    int k = pos - n*32;

    unsigned int v = 0x00000001 << k;

    if (values.data[n] & v) return 1;
    else return 0;
}

bitList makeBitList(intList values) {
    bitList retList;
    if (values.size == 0) {
        retList.size = 0;
        retList.data = malloc(0);
        return retList;
    }
    qsort(values.data, values.size, sizeof(int), intSorter);

    int max = values.data[values.size - 1] + 1;
    int n = (max + 31) >> 5;
    
    retList = makeEmptyBitList(max);

    for (int i = 0; i < values.size; i++) {
        setBit(&retList, values.data[i], 1);
    }
    return retList;
}

bitList makeEmptyBitList(int size) {
    bitList ret;
    ret.size = size;
    int s = (size + 31)>>5;
    ret.data = malloc(sizeof(unsigned int) * s);
    for (int i = 0; i < s; i++) {
        ret.data[i] = 0;
    }
    return ret;
}

bitList duplicateBitList(bitList in) {
    bitList ret;
    ret.size = in.size;
    int s = (in.size + 31) >> 5;
    ret.data = malloc(sizeof(unsigned int) * s);
    for (int i = 0; i < s; i++) {
        ret.data[i] = in.data[i];
    }
    return ret;
}

void printBitList(bitList b) {
    int size = (b.size + 31) >> 5;
    int k = 0;

    printf("BitList %d elements:\n", b.size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 32; j++) {
            printf("%d",(((b.data[i]>>j) & 1) == 1) ? 1 : 0);
            k++;
            if (k == b.size) {
                printf("\n");
                return;
            }
        }
        printf("\n");
    }
}

void printIntList(intList l) {
    printf("IntList %d elements: {", l.size);
    for (int i = 0; i < l.size; i++) {
        if (i != 0) if (i%INTLIST_PRINT_COUNT == 0) printf("\n");
        else printf(", ");
        printf("%d", l.data[i]);
    }
    printf("}\n");
}

static int addBitRecursion(int k, bitList * l) {
    int s = (l->size + 31)>>5;
    int d = l->size%32;
    if (l->data[k] == 0xFFFFFFFF) {
        l->data[k] = 0;
        if (k == s - 1) {
            l->data = realloc(l->data, s + 1);
            l->data[k + 1] = 1;
            l->size++;
            return 1;
        }
        
        return addBitRecursion(++k, l);
    }
    l->data[k]++;
    if (k == s - 1 && d != 0) {
        if (l->data[k] & (1 << d)) {
            l->size++;
            return 1;
        }
    }
    return 0;
}

int addBit(bitList * b) {
    return addBitRecursion(0, b);
}

int countBits(bitList b) {
    unsigned char lookUp[] = {
        0, 1, 1, 2, 1, 2, 2, 3, 
        1, 2, 2, 3, 2, 3, 3, 4, 
        1, 2, 2, 3, 2, 3, 3, 4, 
        2, 3, 3, 4, 3, 4, 4, 5, 
        1, 2, 2, 3, 2, 3, 3, 4, 
        2, 3, 3, 4, 3, 4, 4, 5, 
        2, 3, 3, 4, 3, 4, 4, 5, 
        3, 4, 4, 5, 4, 5, 5, 6, 
        1, 2, 2, 3, 2, 3, 3, 4, 
        2, 3, 3, 4, 3, 4, 4, 5, 
        2, 3, 3, 4, 3, 4, 4, 5, 
        3, 4, 4, 5, 4, 5, 5, 6, 
        2, 3, 3, 4, 3, 4, 4, 5, 
        3, 4, 4, 5, 4, 5, 5, 6, 
        3, 4, 4, 5, 4, 5, 5, 6, 
        4, 5, 5, 6, 5, 6, 6, 7, 
        1, 2, 2, 3, 2, 3, 3, 4, 
        2, 3, 3, 4, 3, 4, 4, 5, 
        2, 3, 3, 4, 3, 4, 4, 5, 
        3, 4, 4, 5, 4, 5, 5, 6, 
        2, 3, 3, 4, 3, 4, 4, 5, 
        3, 4, 4, 5, 4, 5, 5, 6, 
        3, 4, 4, 5, 4, 5, 5, 6, 
        4, 5, 5, 6, 5, 6, 6, 7, 
        2, 3, 3, 4, 3, 4, 4, 5, 
        3, 4, 4, 5, 4, 5, 5, 6, 
        3, 4, 4, 5, 4, 5, 5, 6, 
        4, 5, 5, 6, 5, 6, 6, 7, 
        3, 4, 4, 5, 4, 5, 5, 6, 
        4, 5, 5, 6, 5, 6, 6, 7, 
        4, 5, 5, 6, 5, 6, 6, 7, 
        5, 6, 6, 7, 6, 7, 7, 8},
        mask = 0xFF;
    int c = (b.size + 7) >>3,
        s = (b.size + 31) >>5,
        k = b.size - c * 8,
        count = 0,
        ret = 0;
    

    for (int i = 0; i < s; i++) {
        for (int j = 0; j < 4; j++) {
            unsigned char * values = (unsigned char*)&b.data[i];
            if (count == c - 1) mask = ~(0xFF<<k);
            ret += lookUp[values[j] & mask];
            count++;
            if (count == c) {
                return ret;
            }
        }
    }
}

intList extractList(bitList in) {
    intList retList;

    int s = (in.size + 31)>>5, 
        k = 0, 
        c = 0,
        * temp = malloc(sizeof(int) * in.size);

    for (int i = 0; i < s; i++) {
        for (int j = 0; j < 32; j++) {
            if ((in.data[i] >> j) & 1) {
                temp[c] = i * 32 + j;
                c++;
            }

            k++;
            if (k == in.size) goto MISC_EL_FB_1;
        }
    }
MISC_EL_FB_1:

    retList.size = c;
    retList.data = malloc(sizeof(int) * c);
    for (int i = 0; i < c; i++) {
        retList.data[i] = temp[i];
    }

    return retList;
}

perCounter createPerCounter(intList in) {
    perCounter ret;
    ret.size = in.size;
    ret.counter = malloc(sizeof(int) * in.size);
    ret.values = malloc(sizeof(int) * in.size);
    for (int i = 0; i < in.size; i++) {
        ret.values[i] = in.data[i];
        ret.counter[i] = 0;
    }
    return ret;
}

static int recursiveCountPer(perCounter in, int k) {
    if (k == in.size) {
        return 1;
    }
    in.counter[k]++;
    if (in.values[k] <= in.counter[k]) {
        in.counter[k] = 0;
        return recursiveCountPer(in, ++k);
    }
    return 0;
}

int countPer(perCounter in) {
    return recursiveCountPer(in, 0);
}

void freePerCounter(perCounter in) {
    free(in.counter);
    free(in.values);
}

int checkBitCollision(bitList * in, int size) {
    unsigned int aux, s, k, mask, maxS = 0, aux2 = 3, maxK;
    for (int i = 0; i < size; i++) {
        s = (in[i].size + 31)>>5;
        maxS = (maxS < s) ? s : maxS;
    }
    for (int i = 0; i < maxS; i++) {
        aux = 0;
        mask = (0xFFFFFFFF);
        maxK = 0;
        for (int j = 0; j < size; j++) {
            s = in[j].size>>5;
            if (s < i) continue;
            k = in[j].size - s*32;
            maxK = (maxK < k) ? k : maxK;
            if (s == i) mask = ~(mask<<k);
            if (aux & in[j].data[i] & mask) return 4;
            aux |= (in[j].data[i] & mask);
        }
        mask = (0xFFFFFFFF);
        if (maxS - 1 == i) mask = ~(mask<<maxK);
        if (aux != mask) aux2 &= ~1;
    }
    return aux2;
}

bitList mergeBitList(bitList * in, int size) {
    bitList ret;
    unsigned int aux, s, k, mask, maxS = 0, maxB = 0;
    for (int i = 0; i < size; i++) {
        s = (in[i].size + 31)>>5;
        maxS = (maxS < s) ? s : maxS;
        maxB = (maxB < in[i].size) ? in[i].size : maxB;
    }
    ret.size = maxB;
    ret.data = malloc(sizeof(unsigned int) * maxS);

    for (int i = 0; i < maxS; i++) {
        aux = 0;
        for (int j = 0; j < size; j++) {
            s = in[j].size>>5;
            if (s < i) continue;
            k = in[j].size - s*32;
            mask = (0xFFFFFFFF);
            aux |= in[j].data[i];
        }
        ret.data[i] = aux;
    }
    return ret;
}

void bitReset(bitList in) {
    int s = in.size>>5;
    for (int i = 0; i < s; i++) {
        in.data[i] = 0;
    }
}