#include "nGraph.h"

int neighboorCmp(const void * a, const void * b) {
    return ((neighboorData*)a)->vertex->id - ((neighboorData*)b)->vertex->id;
}

graph * createGraph(int vertexCount, int edgeCount, int* edges) {
    graph * ret = malloc(sizeof(graph));
    edge * cEdge;
    vertex * cVertex;
    int v1, v2, c1, c2, delta = 0;
    ret->edgeList = malloc(sizeof(edge *) * edgeCount);
    ret->vertexList = malloc(sizeof(vertex *) * vertexCount);
    ret->edgeCount = edgeCount;
    ret->vertexCount = vertexCount;

    for (int i = 0; i < vertexCount; i++) {
        cVertex = createVertex();
        cVertex->id = i;
        cVertex->color = 0;
        cVertex->parent = ret;
        cVertex->degree = 0;
        ret->vertexList[i] = cVertex;
    }

    for (int i = 0; i < edgeCount; i++) {
        cEdge = createEdge();
        cEdge->id = i;
        cEdge->color = -1;
        cEdge->parent = ret;
        ret->edgeList[i] = cEdge;

        v1 = edges[i*2];
        v2 = edges[i*2 + 1];
        ret->vertexList[v1]->degree++;
        ret->vertexList[v2]->degree++;

        cEdge->incidentVertex1 = ret->vertexList[v1];
        cEdge->incidentVertex2 = ret->vertexList[v2];

        ret->edgeList[i] = cEdge;
    }

    for (int i = 0; i < vertexCount; i++) {
        cVertex = ret->vertexList[i];
        v1 = cVertex->degree;
        cVertex->neighboors = realloc(cVertex->neighboors, sizeof(neighboorData) * v1);
        if (v1 > delta) delta = v1;
    }
    
    ret->delta = delta;

    for (int i = 0; i < edgeCount; i++) {
        cEdge = ret->edgeList[i];
        cEdge->degree = cEdge->incidentVertex1->degree + cEdge->incidentVertex2->degree - 2;
        v1 = edges[i*2];
        v2 = edges[i*2 + 1];
        c1 = ret->vertexList[v1]->color;
        c2 = ret->vertexList[v2]->color;
        ret->vertexList[v1]->neighboors[c1].vertex = ret->vertexList[v2];
        ret->vertexList[v2]->neighboors[c2].vertex = ret->vertexList[v1];
        ret->vertexList[v1]->neighboors[c1].edge = cEdge;
        ret->vertexList[v2]->neighboors[c2].edge = cEdge;
        ret->vertexList[v1]->color++;
        ret->vertexList[v2]->color++;
    }

    for (int i = 0; i < vertexCount; i++) {
        cVertex = ret->vertexList[i];
        cVertex->color = -1;
        qsort(cVertex->neighboors, cVertex->degree, sizeof(neighboorData), neighboorCmp);
        cVertex->nextNeighboorID = -1;
        for (int j = 0; j < cVertex->degree; j++) {
            if (cVertex->id < cVertex->neighboors[j].vertex->id) {
                cVertex->nextNeighboorID = j;
                break;
            }
        }
        if (cVertex->nextNeighboorID == -1) cVertex->nextNeighboorID = cVertex->degree;
    }

    return ret;
}

graph * transformGraph(graph * in, transformKey ** out) {
    int newVertexCount = in->edgeCount + in->vertexCount, 
        newEdgeCount = 0, 
       *edgeList, 
        current,
        c = 0;
    transformKey * outKey;
    edge * currentEdge;
    vertex * currentVertex;
    for (int i = 0; i < in->edgeCount; i++) {
        newEdgeCount += in->edgeList[i]->degree;
    }
    newEdgeCount = newEdgeCount / 2 + 3 * in->edgeCount;
    outKey = malloc(sizeof(transformKey));
    outKey->vertexNumber = newVertexCount;
    outKey->vertexMeaning = malloc(sizeof(int) * newVertexCount);

    for (int i = 0; i < in->vertexCount; i++) 
        outKey->vertexMeaning[i] = (i + 1);
    
    for (int i = 0; i < in->edgeCount; i++) 
        outKey->vertexMeaning[i + in->vertexCount] = -(i + 1);
    

    edgeList = malloc(sizeof(int) * newEdgeCount * 2);
    for (int i = 0; i < newVertexCount; i++) {
        if (outKey->vertexMeaning[i] > 0) { // Vértice
            current = outKey->vertexMeaning[i] - 1;
            currentVertex = in->vertexList[current];
            for (int j = 0; j < currentVertex->degree; j++) {
                if (currentVertex->id < currentVertex->neighboors[j].vertex->id) {
                    edgeList[c * 2] = i;
                    edgeList[c * 2 + 1] = currentVertex->neighboors[j].vertex->id;
                    c++;
                }
                edgeList[c * 2] = i;
                edgeList[c * 2 + 1] = in->vertexCount + currentVertex->neighboors[j].edge->id;
                c++;
            }
        }   

        if (outKey->vertexMeaning[i] < 0) { // Aresta
            current = -outKey->vertexMeaning[i] - 1;
            currentEdge = in->edgeList[current];
            currentVertex = currentEdge->incidentVertex1;
            for (int j = 0; j < currentVertex->degree; j++) {
                if (currentEdge->id < currentVertex->neighboors[j].edge->id) {
                    edgeList[c * 2] = i;
                    edgeList[c * 2 + 1] = in->vertexCount + currentVertex->neighboors[j].edge->id;
                    c++;
                }
                
            }

            currentVertex = currentEdge->incidentVertex2;
            for (int j = 0; j < currentVertex->degree; j++) {
                if (currentEdge->id < currentVertex->neighboors[j].edge->id) {
                    edgeList[c * 2] = i;
                    edgeList[c * 2 + 1] = in->vertexCount + currentVertex->neighboors[j].edge->id;
                    c++;
                }
            }
        }       
    }
    *out = outKey;
    return createGraph(newVertexCount, newEdgeCount, edgeList);
}

edge * createEdge() {
    edge * ret = malloc(sizeof(edge));
    ret->incidentEdges = malloc(0);
    return ret;
}

vertex * createVertex() {
    vertex * ret = malloc(sizeof(vertex));
    ret->neighboors = malloc(0);
    return ret;
}

void vertexGraphColoring(graph * in) {
    int vertC = in->vertexCount;
    iterator * it = createIterator(vertC, NULL, vertC); // Valor máximo de Cores possíveis é o número de vértices
    unsigned long long int type2 = 0, c = 0, c2 = 0;

    while (!validateColoring(in, it)) {
        c++;
        if (c == 1000000000) {
            printf("%llu bilhoes de iteracoes testadas. Iteracao atual:\n", c2);
            printIterator(it);
            c2++;
            c = 0;
        }
        if (iterate(it) == 1) {
            it->maxValue++;
            if (!type2) type2 = 1;
            else {
                printf("Alguma coisa deu muito errado:\n Não foi possivel fazer a coloracao total com delta + 1 ou delta + 2 cores.\n");
                return;
            } 
        }
    }

    printf("Coloracao encontrada depois de %llu%09llu iteracoes.\nO grafo e tipo %d\n", c2, c, (type2) ? 2 : 1);
    setColor(in, it);
    printIterator(it);
    printGraph(in, 1, 1, 1);
    destroyIterator(it);
}

void totalGraphColoring(graph * in) {
    transformKey * tK;
    graph * trans = transformGraph(in, &tK);
    vertex * max = findMaxDegreeVertex(in);
    vertex * transMax = trans->vertexList[max->id];
    int *startValue = NULL;
    iterator * it = createIterator(trans->vertexCount, startValue, in->delta + 2);
    unsigned long long int type2 = 1, c = 0, c2 = 1;
    lockValue(it, transMax->id, c++);
    for (int i = 0; i < transMax->degree; i++) {
        if (transMax->neighboors[i].vertex->id >= in->vertexCount)
            lockValue(it, transMax->neighboors[i].vertex->id, c++);
    }

    while (!validateColoring(trans, it)) {
        c++;
        if (c == 1000000000) {
            printf("%llu bilhoes de iteracoes testadas. Iteracao atual:\n", c2);
            printIterator(it);
            printf("\n");
            c2++;
            c = 0;
        }
        if (iterate(it) == 1) {
            it->maxValue++;
            if (!type2) type2 = 1;
            else {
                printf("Alguma coisa deu muito errado:\n Não foi possivel fazer a coloracao total com delta + 1 ou delta + 2 cores.\n");
                return;
            } 
        }
    }
    printf("Coloracao encontrada depois de %llu%09llu iteracoes.\nO grafo e tipo %d\n", c2, c, (type2) ? 2 : 1);
    
    setColor(trans, it);
    translateColorToOriginalGraph(in, trans);
    printIterator(it);
    printGraph(trans, 1, 0, 1);
    printGraph(in, 1, 1, 1);
    destroyIterator(it);
    destroyTransformKey(tK);
    destroyGraph(trans);
}

void totalGraphColoringUCI(graph * in) {
    transformKey * tK;
    graph * trans = transformGraph(in, &tK);
    vertex * max = findMaxDegreeVertex(in);
    vertex * transMax = trans->vertexList[max->id];
    UCI * uci = createUCI(in->delta + 1, trans->vertexCount, 0, 1);
    uci->configuration[0] = 10;
    uci->configuration[1] = 9;
    uci->configuration[2] = 3;
    uci->configuration[3] = 1;
    uci->configuration[4] = 1;
    uci->configuration[5] = 0;
    int configVal[] = {1, 3, 4, 6, 8, 10, 14, 16, 21, 23, 0, 1, 3, 4, 8, 9, 11, 12, 13, 0, 1, 2, 0, 0};
    for (int i = 0; i < uci->size; i++) {
        uci->valueConfig[i] = configVal[i];
    }
    uci->maxValues[0] = uci->size;
    uci->pointers[0] = 0;
    uci->used = 0;
    for (int i = 0; i < uci->limit - 1; i++) {
        uci->maxValues[i + 1] = uci->maxValues[i] - uci->configuration[i];
        uci->pointers[i + 1] = uci->pointers[i] + uci->configuration[i];
    }
    unsigned long long int type2 = 1, c = 9999999, c2 = 10856;


    while (!validateColoringUCI(trans, uci)) {
        c++;
        if (c == 10000000) {
            printf("%llu dezenas de milhoes de iteracoes testadas. Iteracao atual:\n", c2);
            printUCI(uci, 1, 0);
            printf("\n");
            c2++;
            c = 0;
        }
        if (iterateUCI(uci) == 1) {
            printf("Alguma coisa deu muito errado:\n Não foi possivel fazer a coloracao total com delta + 1 ou delta + 2 cores.\n");
            return;
        }
    }
    printf("Coloracao encontrada depois de %llu%07llu iteracoes.\nO grafo e tipo %d\n", c2, c, (type2) ? 2 : 1);
    
    setColorUCI(trans, uci);
    translateColorToOriginalGraph(in, trans);
    printUCI(uci, 1, 0);
    printGraph(trans, 1, 0, 1);
    printGraph(in, 1, 1, 1);
    destroyUCI(uci);
    destroyTransformKey(tK);
    destroyGraph(trans);
}

int validateColoring(graph * in, iterator * it) {
    vertex * current, * currentN;
    for (int i = 0; i < in->vertexCount; i++) {
        current = in->vertexList[i];
        for (int j = current->nextNeighboorID; j < current->degree; j++) {
            currentN = in->vertexList[i]->neighboors[j].vertex;
            if (it->values[current->id] == it->values[currentN->id]) return 0;
        }
    }
    return 1;
}

int validateColoringUCI(graph * in, UCI * uci) {
    int * values = UCIGetValues(uci);
    vertex * current, * currentN;
    for (int i = 0; i < in->vertexCount; i++) {
        current = in->vertexList[i];
        for (int j = current->nextNeighboorID; j < current->degree; j++) {
            currentN = in->vertexList[i]->neighboors[j].vertex;
            if (values[current->id] == values[currentN->id]) {
                free(values);
                return 0;
            }
        }
    }
    free(values);
    return 1;
}

void setColor(graph * in, iterator * it) {
    for (int i = 0; i < in->vertexCount; i++) {
        in->vertexList[i]->color = it->values[i];
    }
}

void setColorUCI(graph * in, UCI * uci) {
    int * values = UCIGetValues(uci);
    for (int i = 0; i < in->vertexCount; i++) {
        in->vertexList[i]->color = values[i];
    }
    free(values);
}

void translateColorToOriginalGraph(graph * original, graph * transformed) {
    for (int i = 0; i < transformed->vertexCount; i++) {
        if (i < original->vertexCount) {
            original->vertexList[i]->color = transformed->vertexList[i]->color;
        }
        else {
            original->edgeList[i - original->vertexCount]->color = transformed->vertexList[i]->color;
        }
    }
}

vertex * findMaxDegreeVertex(graph * in) {
    vertex * ret = NULL;
    int degree = 0;
    for (int i = 0; i < in->vertexCount; i++) {
        if (degree < in->vertexList[i]->degree) {
            ret = in->vertexList[i];
            degree = ret->degree;
        }
    }
    return ret;
}

int getEdgeFromTransformed(int ID, transformKey * key) {
    if (key->vertexMeaning[ID] > 0) return -1;
    else return -key->vertexMeaning[ID] - 1;
}

int getVertexFromTransformed(int ID, transformKey * key) {
    if (key->vertexMeaning[ID] < 0) return -1;
    else return key->vertexMeaning[ID] - 1;
}

void printGraph(graph * g, int printDetail, int printColourEdge, int printColourVertex) {
    printf("Graph address: %x\nEdges: %d\nVertices: %d\n", g, g->edgeCount, g->vertexCount);
    if (!printDetail) return;
    if (!printColourVertex || printColourEdge) 
        for (int i = 0; i < g->edgeCount; i++) {
            printf("Edge N: %d, %d - %d", i, g->edgeList[i]->incidentVertex1->id, g->edgeList[i]->incidentVertex2->id);
            if (printColourEdge) printf(", C: %d", g->edgeList[i]->color);
            printf("\n");
        }
    
    if (!printColourVertex) return;
    for (int i = 0; i < g->vertexCount; i++) {
        printf("Vertex N: %d, C: %d\n", i, g->vertexList[i]->color);
    }
    return;
}

void destroyGraph(graph * g) {
    for (int i = 0; i < g->vertexCount; i++) {
        destroyVertex(g->vertexList[i]);
    }
    for (int i = 0; i < g->edgeCount; i++) {
        destroyEdge(g->edgeList[i]);
    }
    free(g->edgeList);
    free(g->vertexList);
    free(g);
}

void destroyEdge(edge * e) {
    free(e->incidentEdges);
    free(e);
}

void destroyVertex(vertex * v) {
    free(v->neighboors);
    free(v);
}

void destroyTransformKey(transformKey * k) {
    free(k->vertexMeaning);
    free(k);
    return;
}
