#include "nGraph.h"

int neighboorCmp(const void * a, const void * b) {
    return ((neighboorData*)a)->vertex->id - ((neighboorData*)b)->vertex->id;
}

graph * createGraph(int vertexCount, int edgeCount, int* edges) {
    graph * ret = malloc(sizeof(graph));
    edge * cEdge;
    vertex * cVertex;
    int v1, v2, c1, c2, delta = 0, nextBreak;
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
        cVertex->neighboorMap = createBitmap(ret->vertexCount);
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
        nextBreak = 0;
        for (int j = 0; j < cVertex->degree; j++) {
            setBitTrue(cVertex->neighboorMap, cVertex->neighboors[j].vertex->id);
            if (!nextBreak && cVertex->id < cVertex->neighboors[j].vertex->id) {
                cVertex->nextNeighboorID = j;
                nextBreak = 1;
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
    graph * ret;
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
    ret = createGraph(newVertexCount, newEdgeCount, edgeList);
    free(edgeList);
    return ret;
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
    int currentCount = 2;
    iterator * it = createIterator(vertC, NULL, currentCount); // Valor máximo de Cores possíveis é delta+2
    edge * e = in->edgeList[0];
    lockValue(it, e->incidentVertex1->id, 0);   // Trivial encontrar clique de tamanho 2,
    lockValue(it, e->incidentVertex2->id, 1);   // Portanto pode-se bloquear 2 elementos.
    unsigned long long int c = 0, c2 = 0;
    clock_t t1, t2, t3, t4;
    char * p;
    FILE * f;
    t1 = clock();
    t3 = clock();
    t4 = clock();
    while (!validateColoring(in, it)) {
#ifdef STORE_BACKUP
        t2 = clock();
        if (t2 - t4 > BACKUP_TIME_SECS) {
            f = fopen("BackupIt.txt", "w");
            p = IteratorToString(it);
            fprintf(f, "Vertex Iterator:\n%s", p);
            fclose(f);
            free(p);
            t4 = clock();
        }
#endif
#ifndef NO_PRINT
        c++;
        if (c == 10000000) {
            t2 = clock();
            c2++;
            printf("%llu bilhoes de iteracoes testadas em %.3f segundos (ciclo em %.3f s).\n Iteracao atual:\n", c2, (t2 - t1 * 1.0)/CLOCKS_PER_SEC, (t2 - t3 * 1.0)/CLOCKS_PER_SEC);
            printIterator(it);
            c = 0;
            t3 = clock();
        }
#endif
        if (iterate(it) == 1) {
            destroyIterator(it);
            currentCount++;
            if (currentCount > in->delta + 2) {
                printf("Alguma coisa deu muito errado:\n Não foi possivel fazer a coloracao de vertices com ate delta + 2 cores.\n");
                return;
            } 
            it = createIterator(vertC, NULL, currentCount);
            lockValue(it, e->incidentVertex1->id, 0);   // Trivial encontrar clique de tamanho 2,
            lockValue(it, e->incidentVertex2->id, 1);   // Portanto pode-se bloquear 2 elementos.
        }
    }
    in->chroma = it->maxValue;
    setColor(in, it);
#ifndef NO_PRINT
    printf("Coloracao encontrada depois de %llu%09llu iteracoes usando %d cores.\n", c2, c, currentCount);
    printIterator(it);
    printGraph(in, 1, 0, 1);
#endif
    destroyIterator(it);
}

void vertexGraphColoringUCI(graph * in) {
    int vertC = in->vertexCount;
    UCI * uci = createUCI(in->delta + 1, vertC, 1, 1);
    bitmap * validBm = createBitmap(vertC);
    clock_t t1, t2, t3, t4;
    unsigned long long int type2 = 0, c = 0, c2 = 0;
    char * p;
    FILE * f;
    t1 = clock();
    t3 = clock();
    t4 = clock();
    while (!validateColoringUCI(in, uci)) {
#ifdef STORE_BACKUP
        t2 = clock();
        if (t2 - t4 > BACKUP_TIME_SECS) {
            f = fopen("BackupIt.txt", "w");
            p = UCIToString(uci);
            fprintf(f, "Vertex UCI:\n%s", p);
            fclose(f);
            free(p);
            t4 = clock();
        }
#endif
#ifndef NO_PRINT
        c++;
        if (c == 100000000) {
            t2 = clock();
            c2++;
            p = UCIToString(uci);
            
            free(p);
            printf("%llu centenas de milhao de iteracoes testadas em %.3f segundos (ciclo em %.3f s).\n Iteracao atual:\n", c2, (t2 - t1 * 1.0)/CLOCKS_PER_SEC, (t2 - t3 * 1.0)/CLOCKS_PER_SEC);
            printUCI(uci, 1, 0);
            printf("\n");
            c = 0;
            t3 = clock();
        }
#endif

        if (iterateUCI(uci) == 1) {
            printf("Alguma coisa deu muito errado:\n Nao foi possivel fazer a coloracao de vértices até delta + 2 cores.\n");
            return;
        }
    }
    in->chroma = uci->used;
    setColorUCI(in, uci);
#ifndef NO_PRINT
    printf("Coloracao encontrada depois de %llu%08llu iteracoes usando %d cores.\n", c2, c, uci->used);
    printUCI(uci, 1, 0);
    printGraph(in, 0, 0, 1);
#endif
    destroyUCI(uci);
}

void totalGraphColoring(graph * in) {
    transformKey * tK;
    graph * trans = transformGraph(in, &tK);
    vertex * max = findMaxDegreeVertex(in);
    vertex * transMax = trans->vertexList[max->id];
    int *startValue = NULL;
    iterator * it = createIterator(trans->vertexCount, startValue, in->delta + 1);
    unsigned long long int type2 = 0, c = 0, c2 = 1;
    clock_t t1, t2, t3, t4;
    char * p;
    FILE * f;
    lockValue(it, transMax->id, c++);
    for (int i = 0; i < transMax->degree; i++) {
        if (transMax->neighboors[i].vertex->id >= in->vertexCount)
            lockValue(it, transMax->neighboors[i].vertex->id, c++);
    }
    c = 0;
    t1 = clock();
    t3 = clock();
    t4 = clock();
    while (!validateColoring(trans, it)) {
#ifdef STORE_BACKUP
        t2 = clock();
        if (t2 - t4 > BACKUP_TIME_SECS) {
            f = fopen("BackupIt.txt", "w");
            p = IteratorToString(it);
            fprintf(f, "Total Iterator:\n%s", p);
            fclose(f);
            free(p);
            t4 = clock();
        }
#endif
#ifndef NO_PRINT
        c++;
        if (c == 1000000000) {
            t2 = clock();
            printf("%llu bilhoes de iteracoes testadas em %.3f segundos (ciclo em %.3f s).\n Iteracao atual:\n", c2, (t2 - t1 * 1.0)/CLOCKS_PER_SEC, (t2 - t3 * 1.0)/CLOCKS_PER_SEC);
            printIterator(it);
            printf("\n");
            c2++;
            c = 0;
            t3 = clock();
        }
#endif
        if (iterate(it) == 1) {
            it->maxValue++;
            if (!type2) type2 = 1;
            else {
                printf("Alguma coisa deu muito errado:\n Não foi possivel fazer a coloracao total com delta + 1 ou delta + 2 cores.\n");
                return;
            } 
        }
    }
    in->chroma = it->maxValue;
    setColor(trans, it);
    translateColorToOriginalGraph(in, trans);
#ifndef NO_PRINT
    printf("Coloracao encontrada depois de %llu%09llu iteracoes.\nO grafo e tipo %d\n", c2, c, (type2) ? 2 : 1);
    printIterator(it);
    printGraph(trans, 1, 0, 1);
    printGraph(in, 1, 1, 1);
#endif
    destroyIterator(it);
    destroyTransformKey(tK);
    destroyGraph(trans);
}

void totalGraphColoringUCI(graph * in) {
    transformKey * tK;
    graph * trans = transformGraph(in, &tK);
    vertex * max = findMaxDegreeVertex(in);
    vertex * transMax = trans->vertexList[max->id];
    bitmap * validBm = createBitmap(trans->vertexCount);
    UCI * uci = createUCI(in->delta + 1, trans->vertexCount, 0, 1);
    /*/
    uci->configuration[0] = 3;
    uci->configuration[1] = 3;
    uci->configuration[2] = 3;
    uci->configuration[3] = 3;
    uci->configuration[4] = 3;
    uci->configuration[5] = 0;
    UCIResetBase(uci);
    int config[] = {0, 10, 11, 0, 7, 10, 0, 3, 8, 0, 2, 5, 0, 1, 2};
    for (int i = 0; i < uci->size; i++) {
        uci->valueConfig[i] = config[i];
    }
    //*/
    unsigned long long int type2 = 0, c = 0, c2 = 0;
    clock_t t1, t2, t3, t4;
    FILE * f;
    char * p;
    t1 = clock();
    t3 = clock();
    t4 = clock();
    while (!validateColoringUCI(trans, uci)) {
#ifdef STORE_BACKUP
        t2 = clock();
        if (t2 - t4 > BACKUP_TIME_SECS) {
            f = fopen("BackupIt.txt", "w");
            p = UCIToString(uci);
            fprintf(f, "Total UCI:\n%s", p);
            fclose(f);
            free(p);
            t4 = clock();
        }
#endif     
#ifndef NO_PRINT 
        c++;
        if (c == 100000000) {
            c2++;

            t2 = clock();
            printf("%llu centenas de milhao de iteracoes testadas em %.3f segundos (ciclo em %.3f s).\n Iteracao atual:\n", c2, (t2 - t1 * 1.0)/CLOCKS_PER_SEC, (t2 - t3 * 1.0)/CLOCKS_PER_SEC);
            printUCI(uci, 1, 0);
            printf("\n");
            t3 = clock();
            c = 0;
        }
#endif
        if (iterateUCI(uci) == 1) {
            printf("Alguma coisa deu muito errado:\n Nao foi possivel fazer a coloracao total com delta + 1 ou delta + 2 cores.\n");
            return;
        }
    }
    if (uci->configuration[uci->limit - 1] != 0) type2 = 1;
    in->chroma = uci->used;
    //printUCI(uci, 1, 0);
    //printGraph(in, 1, 1, 1);
    destroyBitmap(validBm);
    setColorUCI(trans, uci);
    translateColorToOriginalGraph(in, trans);
#ifndef NO_PRINT
    printf("Coloracao encontrada em %llu%08llu iteracoes.\nO grafo e tipo %d\n", c2, c, (type2) ? 2 : 1);
    printUCI(uci, 1, 0);
    printGraph(trans, 1, 0, 1);
    printGraph(in, 1, 1, 1);
#endif
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

int validateColoringUCI2(graph * in, UCI * uci, bitmap * bm) {
    UCIGetValues2(uci);

    for (int i = 0; i < in->vertexCount; i++) {
        bitmapAnd(uci->valueMap[uci->values[i]], in->vertexList[i]->neighboorMap, &bm);
        if (!isBitmapZero(bm)) return 0;
    }

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
    destroyBitmap(v->neighboorMap);
    free(v);
}

void destroyTransformKey(transformKey * k) {
    free(k->vertexMeaning);
    free(k);
    return;
}
