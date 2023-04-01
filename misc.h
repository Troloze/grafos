#ifndef TRO_MISC
#define TRO_MISC

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INTLIST_PRINT_COUNT 10

#define BITLIST_NOT_EQUALS 8
#define BITLIST_EQUALS 4
#define BITLIST_1_OVERLAP 2
#define BITLIST_0_OVERLAP 1

#define COLLISION_OCCURS 4
#define COLLISION_NOT 2
#define COLLISION_COVER 1

/**
 * @brief Função para ordenar inteiros com o qsort
*/
int intSorter(const void * a, const void * b);

int bitListSorter(const void * a, const void * b);

/**
 * @brief Lista de inteiros usado para retorno de dados;
*/
typedef struct intList {
    int size;
    int * data;
} intList;

/**
 * @brief Lista de bits, usado para comparações mais rápidas.
 * @param size quantidade de bits
 * @param data dados
*/
typedef struct bitList {
    int size;
    unsigned int * data;
} bitList;

/**
 * @brief Estrutura para contagem de permutação.
*/
typedef struct perCounter {
    int size;
    int * values;
    int * counter;
} perCounter;


/**
 * @brief Cria uma estrutura intList
 * @param n Tamanho da lista
 * @param list elementos da lista (Liberar separadamente da memória)
 * @return Estrutura intList com elementos da lista passada como argumento
*/
intList makeIntList(int n, int * list);

/**
 * @brief Libera o espaço da lista de inteiros;
 * @param eL Lista para ser liberada
*/
void freeIntList(intList eL);


/**
 * @brief Função que compara duas listas de bits, a comparação é feita entre valores verdadeiros, qualquer diferença de tamanho é compensada com 0s
 * @param a Lista de bits para ser comparada com b
 * @param b Lista de bits para ser comparada com a
 * @return Utilize as flags bitlist para determinar a saída.
*/
int compareBitList(bitList a, bitList b);

/**
 * @brief Função que gera uma lista de bits a partir de um conjunto de inteiros. A lista de bits terá valores verdadeiros caso a posição do bit esteja incluida no conjunto de entrada, e falso caso contrário.
 * @param values Conjunto de valores para montagem da lista.
 * @return Uma lista de bits.
*/
bitList makeBitList(intList values);

/**
 * @brief Gera uma bitlista vazia
 * @param size Tamanho em bits da bitlist nova
*/
bitList makeEmptyBitList(int size);


bitList duplicateBitList(bitList in);

/**
 * @brief Libera a memória consumida pela bitlist
*/
void freeBitList(bitList bL);

/**
 * @brief Função que compara duas bitlists e retorna a posição onde existe overlap verdadeiro
 * @param a bitlist a ser comparada com b
 * @param b bitlist a ser comparada com a
 * @return BitList com verdadeiro onde a e b são verdadeiros ao mesmo tempo
*/
bitList overlapBitList(bitList a, bitList b);

/**
 * @brief Função que inverte uma bitlist (NÃO USAR)
*/
void invertBitList(bitList a, int s);

/**
 * @brief Função que retorna um bit dentro de uma bitList
 * @param values bitlist a ter o valor extraído
 * @param pos Posição do bit a ser retornado
 * @returns 1 Se o bit na posição pos de values for verdadeiro, 0 caso contrário
*/
int getBit(bitList values, int pos);

/**
 * @brief Função que coloca um valor dentro de uma bitList
 * @param values bitList a ter seu valor atualizado
 * @param pos Posição do bit a ser atualizado.
 * @param type Tipo do bit (0, 1 [qualquer valor positivo]) valores negativos irão inverter o bit
*/
void setBit(bitList * values, int pos, int type);

/**
 * @brief Imprime os dados de uma bitlist no console
 * @param b Bitlist a ter os dados impressos.
*/
void printBitList(bitList b);

/**
 * @brief Imprime o conteúdo de uma lista de inteiros.
*/
void printIntList(intList l);

/**
 * @brief Adiciona 1 nos bits
 * @returns 0 normalmente, 1 se o tamanho da lista aumentar
*/
int addBit(bitList * b);

/**
 * @brief Função que conta quantos bits verdadeiros tem uma bitlist.
*/
int countBits(bitList b);

/**
 * @brief Extrai os valores da BitList
 * @param in BitList de entrada
 * @return IntList com todas as posições na BitList com valores verdadeiros.
*/
intList extractList(bitList in);

/**
 * @brief Cria um contador de permutação a partir de uma lista de inteiros
*/
perCounter createPerCounter(intList in);

/**
 * @brief Adiciona 1 no contador de permutação
*/
int countPer(perCounter in);

/**
 * @brief Libera a memória ocupada por um contador de permutação
*/
void freePerCounter(perCounter in);

/**
 * @brief Vê se existe interseção entre bitlists, e se a união de todas faz uma cobertura completa
*/
int checkBitCollision(bitList * in, int size);

bitList mergeBitList(bitList * in, int size);

void bitReset(bitList in);

#endif