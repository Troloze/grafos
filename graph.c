#include "graph.h"


graph * generateGraph(int vertCount, int edgeCount, int* edges) {
    graph * ret;
    int v1, v2;
    ret = malloc(sizeof(graph));
    ret->v = malloc(sizeof(vertex) * vertCount);
    ret->e = malloc(sizeof(edge) * edgeCount);
    ret->edgeN = edgeCount;
    ret->vertN = vertCount;
    ret->delta = 0;

    for (int i = 0; i < vertCount; i++) {   // Fazendo a configuração inicial dos vértices
        ret->v[i].id = i;
        ret->v[i].degree = 0;
        ret->v[i].colour = 0;
        ret->v[i].g = ret;
    }
    for (int i = 0; i < edgeCount; i++) {   // Encontrando o grau de cada vértice, e fazendo a configuração inicial das arestas
        ret->e[i].colour = -1;
        ret->e[i].id = i;
        ret->e[i].g = ret;

        v1 = edges[i*2];
        v2 = edges[i*2 + 1];    
        ret->v[v1].degree++;
        ret->v[v2].degree++;
    }
    
    for (int i = 0; i < vertCount; i++) {   // Alocando memória em cada vértice para guardar as arestas e encontrando o maior grau.
        ret->v[i].edges = malloc(sizeof(int) * ret->v[i].degree);
        if (ret->delta < ret->v[i].degree) ret->delta = ret->v[i].degree;
    }

    for (int i = 0; i < edgeCount; i++) {   // Colocando as arestas nos vértices, usando as cores como ponteiro para a posição da aresta;
        v1 = edges[i*2];
        v2 = edges[i*2 + 1];   
        ret->v[v1].edges[ret->v[v1].colour] = i;
        ret->v[v2].edges[ret->v[v2].colour] = i;
        ret->e[i].verts[0] = v1;
        ret->e[i].verts[1] = v2;
        ret->v[v1].colour++;
        ret->v[v2].colour++;
    }
    
    for (int i = 0; i < vertCount; i++) {   // Colorindo os vértices com -1.
        ret->v[i].colour = -1;
        ret->v[i].aux = makeEmptyBitList(ret->delta + 2);
        ret->v[i].neigh = neighboorList(ret->v[i]);
        ret->v[i].colN = ret->delta + 2;
    }

    for (int i = 0; i < edgeCount; i++) {
        ret->e[i].aux = getEdgeIncidenceBit(ret->e[i]);
    }
    
    return ret;
}

pseudoGraph * generatePseudo(int vertCount, int edgeCount, int* edges) {
    int v1, v2;
    pseudoGraph * ret;
    ret = malloc(sizeof(pseudoGraph));
    
    ret->vertN = vertCount;
    ret->vertices = malloc(sizeof(pseudoVert) * vertCount);
    for (int i = 0; i < vertCount; i++) {   // Config Inicial
        ret->vertices[i].id = 0;
        ret->vertices[i].deg = 0;
    }

    for (int i = 0; i < edgeCount; i++) { // Contagem do grau
        v1 = edges[i*2];
        v2 = edges[i*2 + 1];   
        ret->vertices[v1].deg++;
        ret->vertices[v2].deg++;
    }

    for (int i = 0; i < vertCount; i++) { // Alocagem;
        ret->vertices[i].neigh = malloc(sizeof(int) * ret->vertices[i].deg);
    }

    for (int i = 0; i < edgeCount; i++) {   // fazendo 
        v1 = edges[i*2];
        v2 = edges[i*2 + 1];   

        ret->vertices[v1].neigh[ret->vertices[v1].id] = v2;
        ret->vertices[v2].neigh[ret->vertices[v2].id] = v1;
        ret->vertices[v1].id++;
        ret->vertices[v2].id++;
    }

    for (int i = 0; i < vertCount; i++) { // Setagem final;
        ret->vertices[i].id = i;
        qsort(ret->vertices[i].neigh, ret->vertices[i].deg, sizeof(int), intSorter);
    }

    return ret;
}

void printGraph(graph * g, int showVertColours, int showEdgeColours) {
    printf("Arestas:");
    for (int i = 0 ; i < g->edgeN; i++) {
        if (i%PRINT_GRAPH_HORIZONTAL_EDGE_N == 0) printf("\n");
        else printf(" || ");
        printf("%02d: %02d - %02d", i, g->e[i].verts[0], g->e[i].verts[1]);
    }
    printf("\n\n");

    if (showVertColours > 0) {
        printf("Cores dos vertices:");
        for (int i = 0; i < g->vertN; i++) {
            if (i%PRINT_GRAPH_HORIZONTAL_COLOUR_N == 0) printf("\n");
            else printf(" || ");
            printf("V_%02d : %d",i, g->v[i].colour);
        }
        printf("\n\n");
    }
    if (showEdgeColours > 0) {
        printf("Cores das arestas:");
        for (int i = 0; i < g->edgeN; i++) {
            if (i%PRINT_GRAPH_HORIZONTAL_COLOUR_N == 0) printf("\n");
            else printf(" || ");
            printf("E_%02d : %d", i, g->e[i].colour);
        }
        printf("\n\n");

    }
    printf("O grafo possui %d vertices e %d arestas.\n", g->vertN, g->edgeN);
    return;
}

void freeGraph(graph * g) {
    for (int i = 0; i < g->vertN; i++) {
        free(g->v[i].edges);
        freeBitList(g->v[i].aux);
    }
    for (int i = 0; i < g->edgeN; i++) {
        freeBitList(g->e[i].aux);
    }
    free(g->v);
    free(g->e);
    free(g);
    return;
}

void freePseudo(pseudoGraph * p) {
    for (int i = 0; i < p->vertN; i++) free(p->vertices[i].neigh);
    free(p->vertices);
    free(p);
}

intList edgeList(vertex v) {
    intList retList;
    retList.size = v.degree;
    int * ret = malloc(sizeof(int) * v.degree);
    for (int i = 0; i < v.degree; i++) {
        ret[i] = v.edges[i];
    }
    retList.data = ret;
    return retList;
}

intList neighboorList(vertex v) {
    intList retList;
    int id = v.id;
    intList edges = edgeList(v);
    int * ret = malloc(sizeof(int) * edges.size);
    int aux;
    retList.size = edges.size;
    for (int i = 0; i < edges.size; i++) {
        edge e = v.g->e[edges.data[i]];
        ret[i] = (e.verts[0] == id) ? e.verts[1] : e.verts[0];
    }
    retList.data = ret;
    qsort(retList.data, retList.size, sizeof(int), intSorter);
    return retList;
}

int hasEdge(vertex v, int edgeID) {
    for (int i = 0; i < v.degree; i++) {
        if (edgeID == v.edges[i]) {
            return 1;
        }
    }
    return 0;
}

intList getEdgeIncidence(edge e) {
    intList retList, v1, v2;
    retList.size = e.g->v[e.verts[0]].degree + e.g->v[e.verts[1]].degree - 2;
    if (retList.size == 0) {
        retList.data = malloc(0);
        return retList;
    }
    int k = 0;
    int * ret = malloc(sizeof(int) * retList.size);
    v1 = edgeList(e.g->v[e.verts[0]]);
    v2 = edgeList(e.g->v[e.verts[1]]);
    for (int i = 0; i < v1.size; i++) {
        if (v1.data[i] == e.id) continue;
        ret[k] = v1.data[i];
        k++;
    }
    for (int i = 0; i < v2.size; i++) {
        if (v2.data[i] == e.id) continue;
        ret[k] = v2.data[i];
        k++;
    }
    freeIntList(v1);
    freeIntList(v2);
    qsort(ret, retList.size, sizeof(int), intSorter);
    retList.data = ret;
    return retList;
}

bitList getEdgeIncidenceBit(edge e) {
    intList a = getEdgeIncidence(e);
    return makeBitList(a);
}

edgeSets getIndependentEdgeSets(graph * g) {
    edgeSets ret = newEdgeSets();
    if (g->edgeN == 0) return ret;
    bitList config = makeEmptyBitList(g->edgeN),
        temp,
        * cluster;
    intList set;
    
    while(!addBit(&config)) {
        //printBitList(config);
        set = extractList(config);
        

        cluster = malloc(sizeof(bitList) * set.size);
        for (int i = 0; i < set.size; i++) 
            cluster[i] = g->e[set.data[i]].aux;
        
        temp = mergeBitList(cluster, set.size);
        
        if (!(compareBitList(temp, config) & BITLIST_1_OVERLAP)) {
            addToSets(&ret, config);
        }

        free(cluster);
        freeBitList(temp);
    }

    qsort(ret.sets, ret.size, sizeof(bitList), bitListSorter);
    return ret;
}

void viewSets(edgeSets e) {
    printf("EdgeSets size:%d\n", e.size);
    intList temp;
    for (int i = 0; i < e.size; i++) {
        printf("E_%d ", i);
        temp = extractList(e.sets[i]);
        printIntList(temp);
        freeIntList(temp);
    }
}

void freeEdgeSets(edgeSets e) {
    for (int i = 0; i < e.size; i++) {  
        freeBitList(e.sets[i]);
    }
    free(e.sets);
}

edgeSets newEdgeSets() {
    edgeSets ret;
    ret.max = 10;
    ret.size = 0;
    ret.sets = malloc(sizeof(bitList) * 10);
    return ret;
}

void addToSets(edgeSets * e, bitList b) {
    if (e->size == e->max - 1) {
        if (e->max >= 10240) e->max += 10000;
        else e->max *= 2;
        e->sets = realloc(e->sets, sizeof(bitList) * e->max);
    }

    e->sets[e->size] = duplicateBitList(b);
    e->size++;
}

bitList findEdgeCovering(edgeSets eS, graph * g, bitList * start) {
    int maxV = g->delta + 2, c, safety;
    unsigned long long k = 1; //(delta + 2 número cromático máximo)
    bitList * temp;
    intList conv;
    printf("    Procurando cobertura de arestas.(cada ponto representa 16.777.215 permutacoes testadas)\n    ");
    while(!addBit(start)) {
        if (k%0xFFFFFF == 0) printf(".");
        
        if((k++)%0x1FFFFFFF == 0) {
            printf("\n    ");
            printBitList(*start);
            printf("\n    ");
            k = 1;
        }
        safety = 1;
        c = countBits(*start);
        if (c > maxV) continue; // Pois não queremos coberturas de mais que delta + 2 conjuntos independentes.
        temp = malloc(sizeof(bitList) * c);
        conv = extractList(*start);
        
        for (int i = 0; i < c; i++) temp[i] = eS.sets[conv.data[i]];
        if (!(checkBitCollision(temp, c) & COLLISION_COVER)) {
                free(temp);
                continue;
        }


        free(temp);
        safety = 0;
        break;
    }
    if (safety == 1) {
        printf("Nao foi possivel encontrar outra cobertura com ate %d conjuntos independentes\n", maxV);
        start->size = -1;
        free(start->data);
        start->data = NULL;
    }

    return *start;    
}

void colorEdges(graph * g, edgeSets sets, intList cover) {
    intList currentSet;
    edge * current;
    vertex * currentVertex;
    for (int i = 0; i < cover.size; i++) {
        currentSet = extractList(sets.sets[cover.data[i]]);
        for (int j = 0; j < currentSet.size; j++) {
            current = &(g->e[currentSet.data[j]]);
            current->colour = i;
            currentVertex = &(g->v[current->verts[0]]);
            setBit(&(currentVertex->aux), i, 1);
            currentVertex->colN--;
            currentVertex = &(g->v[current->verts[1]]);
            setBit(&(currentVertex->aux), i, 1);
            currentVertex->colN--;
        }
    }
}

int colorVertices(graph * g) {
    int * vertK = malloc(sizeof(int) * g->vertN),
        currentN;
    for (int i = 0; i < g->vertN; i++) {
        vertK[i] = g->v[i].colN;
    }
    intList l = makeIntList(g->vertN, vertK);
    perCounter counter = createPerCounter(l);
    freeIntList(l);
    while(!countPer(counter)) {
        for (int i = 0; i < counter.size; i++) {
        }
        for (int i = 0; i < g->vertN; i++) {
            g->v[i].colour = g->v[i].posCol.data[counter.counter[i]];
            for (int j = 0; j < g->v[i].degree; j++) {
                currentN = g->v[i].neigh.data[j];
                if (currentN > i) break;
                if (g->v[i].colour == g->v[currentN].colour) {
                    goto GRAPH_CV_FB_1;
                }
            }
        }
        return 1;
GRAPH_CV_FB_1:;
    }
    return 0;
}

void colorGraphGeneric(graph * g) {
    edgeSets sets = getIndependentEdgeSets(g);
    bitList edgeCover = makeEmptyBitList(sets.size);
    intList view;
    int result = 1;
    unsigned long long k = 0;
    while (1) {
        printf("Iteracao No %u...\n", k++);
        for (int i = 0; i < g->vertN; i++) {
            bitReset(g->v[i].aux);
        }
        edgeCover = findEdgeCovering(sets, g, &edgeCover);
        if (edgeCover.size == -1) {
            freeEdgeSets(sets);
            return;
        }
        view = extractList(edgeCover);
        printf("    Cobertura encontrada: ");
        printIntList(view);
        colorEdges(g, sets, view);
        freeIntList(view);

        for (int i = 0; i < g->vertN; i++) {
            invertBitList(g->v[i].aux, 0);
            g->v[i].posCol = extractList(g->v[i].aux);
        }

        result = colorVertices(g);
        if (!result) {
            printf("    Nao foi possivel colorir os vertices com a cobertura.\n");
            continue;
        }
        printGraph(g, 1 ,1);
        printf("\n");
        break;
    }

    freeEdgeSets(sets);
    freeBitList(edgeCover);
}

int getNeighboorAtEdge(vertex v, edge n) {
    if (n.verts[0] != v.id && n.verts[1] != v.id) {
        printf("Vertice %d nao e incidente a aresta %d - %d\n");
        return -1;
    }
    return (n.verts[0] == v.id) ? n.verts[1] : n.verts[0];
}

void colorBipGraph(graph * g) {
    int * buffer = malloc(sizeof(int) * g->vertN);
    int bSize = 1, cColor = 0, maxDegreeID = 0, vert, k;
    buffer[0] = 0;
    bitReset(g->v[0].aux);
    intList posCol;
    vertex c;
    edge cE;

    for (int i = 0; i < bSize; i++) {
        c = g->v[buffer[i]];
        maxDegreeID = (g->v[maxDegreeID].degree > c.degree) ? maxDegreeID : c.id;
        for (int j = 0; j < c.degree; j++) {
            if (g->v[c.neigh.data[j]].colour != -1) {
                if (g->v[c.neigh.data[j]].colour == c.colour) {
                    printf("Não é grafo bipartido\n");
                    free(buffer);
                    return;
                }
                continue;
            }
            buffer[bSize++] = c.neigh.data[j];
            g->v[c.neigh.data[j]].colour = (c.colour == 0) ? 1 : 0;
        }
    }

    bSize = 1;
    maxDegreeID = 0;
    buffer[0] = maxDegreeID;
    g->v[buffer[0]].colN = -1;

    for (int i = 0; i < bSize; i++) {
        c = g->v[buffer[i]];
        c.aux.data[0] |= 0b11;
        invertBitList(c.aux, 0);
        posCol = extractList(c.aux);
        k = 0;
        for (int j = 0; j < c.degree; j++) {
            cE = g->e[c.edges[j]];
            vert = getNeighboorAtEdge(c, cE);
            if (g->v[vert].colN == -1) {
                if (cE.colour == -1) {
                    g->e[cE.id].colour = posCol.data[k];
                    setBit(&g->v[vert].aux, posCol.data[k++], 1);
                }
                continue;
            };
            g->e[cE.id].colour = posCol.data[k];
            buffer[bSize++] = vert;
            g->v[vert].colN = -1;
            setBit(&g->v[vert].aux, posCol.data[k++], 1);
        }
        freeIntList(posCol);
    }
    printGraph(g, 1, 1);
    for (int i = 0; i < g->vertN; i++) {
        g->v[i].colN = g->delta + 2;
    }
    return;
} 