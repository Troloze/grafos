#ifndef TRO_GRAPH
#define TRO_GRAPH

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "misc.h"

#define PRINT_GRAPH_HORIZONTAL_EDGE_N 6
#define PRINT_GRAPH_HORIZONTAL_COLOUR_N 6
#define MAX_UINT 0xFFFFFFFF


typedef struct graph graph;

/**
 * @brief Um vértice
 * @param id Número identificador do vértice
 * @param degree Grau do vértice, ou seja, o número de arestas incidentes ao vértice.
 * @param edges Array com os IDs de todas as incidências de arestas (laços terão dois valores)
 * @param colour Cor associada ao vértice.
*/
typedef struct vertex {
    int id;
    int degree;
    int * edges;
    int colour;
    bitList aux;
    intList posCol;
    intList neigh;
    graph * g;
    int colN;
} vertex;

/**
 * @brief Uma aresta
 * @param id Número identificador da aresta
 * @param verts Dois vértices incidentes à aresta (laços terão os dois valores iguais)
 * @param colour Cor associada à aresta.
*/
typedef struct edge {
    int id;
    int verts[2];
    int colour;
    bitList aux;
    graph * g;
} edge;
 
/**
 * @brief Um grafo não orientado 
 * @param vertN número de vértices
 * @param edgeN número de arestas
 * @param v array com todos os vértices
 * @param e array com todas as arestas
*/
struct graph {
    int vertN;
    int edgeN;
    vertex * v;
    edge * e;
    int delta;
};

/**
 * @brief Um pseudovértice, ou seja, um vértice que não pode ser colorido e não tem informação sobre arestas.
 * @param id Número identificador do pseudovértice
 * @param neigh Array com identificadores dos pseudovértices vizinhos.
*/
typedef struct pseudoVert {
    int id;
    int deg;
    int * neigh;
} pseudoVert;

/**
 * @brief Um pseudografo, ou seja, um grafo que possui só informações sobre vértices.
 * @param vertN número de pseudovértices no pseudografo.
 * @param vertices Array com todos os pseudovértices. 
*/
typedef struct pseudoGraph {
    int vertN;
    pseudoVert * vertices;
} pseudoGraph;

/**
 * @brief Estrutura de um conjunto de arestas
 * @param size Quantidade de conjuntos
 * @param max usado para alocar mais espaço
 * @param sets Conjuntos de bitlists dos conjuntos independentes.
*/
typedef struct edgeSets {
    int size;
    int max;
    bitList * sets;
} edgeSets;


/**
 * @brief Gera um grafo não orientado onde vértices e arestas podem ser coloridos.
 * @param vertCount Número de vertices
 * @param edgeCount Número de arestas
 * @param edges array com todas as arestas
 * @returns Um grafo não orientado com todos os vértices e arestas coloridos com -1
*/
graph * generateGraph(int vertCount, int edgeCount, int* edges);

/**
 * @brief Gera um pseudografo, que só considera os vértices. Não é possível realizar coloração 
 * @param vertCount Número de vértices
 * @param edgeCount Número de arestas
 * @param edges array com todas as arestas;
 * @returns O Pseudografo
*/
pseudoGraph * generatePseudo(int vertCount, int edgeCount, int* edges);

/**
 * @brief Função que irá imprimir os dados do grafo no console
 * @param g Grafo a ser impresso
 * @param showVertColours Caso maior que 0, irá imprimir também as cores dos vértices
 * @param showEdgeColours Caso maior que 0, irá imprimir também as cores das arestas
*/
void printGraph(graph * g, int showVertColours, int showEdgeColours);

/**
 * @brief Libera a memória de um grafo
 * @param g Grafo a ser liberado
*/
void freeGraph(graph * g);

/**
 * @brief Libera a memória de um pseudografo
 * @param p Pseudografo a ser liberado
*/
void freePseudo(pseudoGraph * p);

/**
 * @brief Retorna um vetor com os IDs das arestas incidentes ao vértice
 * @param v Vértice a ser analizado
 * @return Retorna um vetor com os IDs das arestas incidentes ao vértice
*/
intList edgeList(vertex v);

/**
 * @brief Retorna uma lista com os IDs dos vértices incidentes à entrada
 * @param v Vértice a ter os vizinhos retornados
 * @return Retorna um vetor com os IDs dos vértices incidentes à entraad
*/
intList neighboorList(vertex v);



/**
 * @brief Função para identificar se um vértice é incidente à uma aresta
 * @param v Vértice a ser verificado
 * @param edgeID ID da aresta
 * @return 1 caso verdadeiro, 0 se falso
*/
int hasEdge(vertex v, int edgeID);

/**
 * @brief Função que retorna todos os IDs das arestas incidentes à aresta passada no argumento
 * @param e Aresta a ter suas arestas incidentes retornadas
 * @return Uma lista com IDs das arestas incidentes à aresta passada no argumento
*/
intList getEdgeIncidence(edge e);

/**
 * @brief Função que retorna uma bitlist onde todas as posições dos IDs incidentes são marcadas com 1
 * @param e Aresta a ter suas arestas incidentes retornadas
 * @return Bitlist representando as incidências das arestas
*/
bitList getEdgeIncidenceBit(edge e);


edgeSets getIndependentEdgeSets(graph * g);

void freeEdgeSets(edgeSets e);

edgeSets newEdgeSets();

void addToSets(edgeSets * e, bitList b);

void viewSets(edgeSets e);

bitList findEdgeCovering(edgeSets eS, graph * g, bitList * start);

void colorGraphGeneric(graph * g);

void colorBipGraph(graph * g) ;

#endif