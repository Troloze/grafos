#include "nGraph.h"

int main (int argc, char** argv) {

    int _bD[] = {4, 5};
    int _b[] = {
        0, 1,
        0, 2,
        0, 3,
        1, 2,
        2, 3
    };

    int _eD[] = {5, 6};
    int _e[] = {
        0, 1, 
        0, 2, 
        0, 4, 
        1, 2, 
        2, 3, 
        3, 4
    };

    int _k5D[] = {5, 10};
    int _k5[] = {
        0, 1,
        0, 2,
        0, 3,
        0, 4,
        1, 2,
        1, 3,
        1, 4,
        2, 3,
        2, 4,
        3, 4
    };

    int _bipD[] = {9, 9};
    int _bip[] = {
        0, 5,
        0, 7,
        1, 5,
        1, 6,
        2, 7,
        2, 8,
        3, 6,
        4, 5,
        4, 8    
    };

    int _k44D[] = {8, 16};
    int _k44[] = {
        0, 4,
        1, 4,
        2, 4,
        3, 4,
        0, 5,
        1, 5,
        2, 5,
        3, 5,
        0, 6,
        1, 6,
        2, 6,
        3, 6,
        0, 7,
        1, 7,
        2, 7,
        3, 7
    };


/*/
    iterator * it = createIterator(10, NULL, 10);
    printIterator(it);
    for (int i = 0; i < 10; i++) {
        lockValue(it, i, i);
        printIterator(it);
    }
//*/
//*/
    graph * b = createGraph(_k44D[0], _k44D[1], _k44);
//*/
    vertexGraphColoring(b);
    totalGraphColoring(b);
    destroyGraph(b);
/*/
    printGraph(b, 1, 0, 0);
    transformKey * tK;  
    graph * nE = transformGraph(b, &tK);
    printGraph(nE, 1, 0 , 0);
    destroyGraph(b);
    destroyGraph(nE);
    destroyTransformKey(tK);
//*/
    return 0;
}