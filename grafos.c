#include "graph.h"

int main (int argc, char** argv) {
    
    int e[] = {
        0, 1, 
        0, 2, 
        0, 4, 
        1, 2, 
        2, 3, 
        3, 4
    };

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

    int bip[] = {
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


    graph * nG = generateGraph(5, 6, e);
    //graph * K5 = generateGraph(5, 10, _k5);
    graph * Bip = generateGraph(9, 9, bip);
    graph * K44 = generateGraph(8, 16, _k44);

    colorBipGraph(K44);
    
    freeGraph(Bip);
    freeGraph(K44);
    freeGraph(nG);
    //freeGraph(K5);

    return 0;
}