#ifndef TRO_GRAPH
#define TRO_GRAPH

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "nMisc.h"

typedef struct _graph graph;
typedef struct _edge edge;
typedef struct _vertex vertex;

typedef struct neighboorData {
    vertex * vertex;
    edge * edge;
} neighboorData;

struct _graph {
    vertex ** vertexList;
    edge ** edgeList;
    int vertexCount;
    int edgeCount;
    int delta;
};

struct _edge {
    vertex * incidentVertex1;
    vertex * incidentVertex2;
    edge ** incidentEdges;
    graph * parent;
    int incidentCount;
    int degree;
    int color;
    int id;
};

struct _vertex {
    neighboorData * neighboors;
    int nextNeighboorID;
    graph * parent;
    int degree;
    int color;
    int id;
    int typeFlags;
};

typedef struct transformKey {
    int vertexNumber;
    int * vertexMeaning;
} transformKey;

graph * createGraph(int vertexCount, int edgeCount, int* edges);
graph * transformGraph(graph * in, transformKey ** out);
edge * createEdge();
vertex * createVertex();

void vertexGraphColoring(graph * in);
void totalGraphColoring(graph * in);

int validateColoring(graph * in, iterator * it);
void setColor(graph * in, iterator * it);
void translateColorToOriginalGraph(graph * original, graph * transformed);

vertex * findMaxDegreeVertex(graph * in);

int getVertexFromTransformed(int ID, transformKey * key);
int getEdgeFromTransformed(int ID, transformKey * key);

void printGraph(graph * g, int printDetail, int printColourEdge, int printColourVertex);

void destroyGraph(graph * g);
void destroyEdge(edge * e);
void destroyVertex(vertex * v);
void destroyTransformKey(transformKey * k);

#endif