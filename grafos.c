#define NO_PRINT
#define STORE_BACKUP
#include "nGraph.h"
#include <windows.h>

typedef struct precisionClock {
    double frequency;
    __int64 start;
} pC;

void runThroughGraphs(int order, int startingGraph, int startAtType, int stopAfterOne);

void runCompleteGraphs(int orderStart, int orderEnd);

pC startCounter() {
    pC ret;
    LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li)) printf("Algo deu errado pra caramba\n");
    ret.frequency = (double) li.QuadPart;

    QueryPerformanceCounter(&li);
    ret.start = li.QuadPart;
    return ret;
}

double getCounter(pC in) {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double) (li.QuadPart - in.start)/in.frequency;
}

int main (int argc, char** argv) {
    


    //for (int i = 2; i <= 8; i++)
        runCompleteGraphs(3, 20);

    return 0;
}

void runThroughGraphs(int order, int startingGraph, int startAtType, int stopAfterOne) {
    int graph_n = 0, currentGraphN = 0, vertexCount, edgeCount, * edges, lineCount;
    pC clk;
    double counterTime;
    char buffer[256] = "", fileName[256] = "";
    graph * currentGraph;
    sprintf(fileName, "Graphs\\graphs%dc.txt", order);
    FILE * f = fopen(fileName, "r");
    if (f == NULL) {
        fclose(fopen(fileName, "w"));
        f = fopen(fileName, "r");
    }
    fgets(buffer, 256, f);
    sscanf(buffer, "%d", &graph_n);
    printf("%d\n", graph_n);
    if (startingGraph > graph_n) {
        fclose(f);
        return;
    }

    if (startingGraph > 0) {
        for (int i = 0; i < startingGraph; i++) {
            fgets(buffer, 256, f);
            fgets(buffer, 256, f);
            sscanf(buffer, "%d %d", &vertexCount, &edgeCount);
            for (int k = 0; k <= (edgeCount - 1)/15; k++) {
                fgets(buffer, 256, f);
            }
            fgets(buffer, 256, f);
            }
    }
    
    while (currentGraphN != graph_n) {
        fgets(buffer, 256, f);
        sscanf(buffer, "Graph %d, order 7.", &currentGraphN);
        fgets(buffer, 256, f);
        sscanf(buffer, "%d %d", &vertexCount, &edgeCount);
        printf("Currently working on order %d graph %d with %d edges.\n", vertexCount, currentGraphN, edgeCount);
        edges = malloc(sizeof(int) * 2 * edgeCount);
        lineCount = 14;
        for (int k = 0; k <= (edgeCount - 1)/15; k++) {
            fgets(buffer, 256, f);
            if (k == (edgeCount - 1)/15) lineCount = (edgeCount - 1)%15;
            for (int i = 0; i <= lineCount; i++) {
                sscanf(&buffer[i * 5], "%d %d", &edges[(i + k * 15) * 2], &edges[(i + k * 15) * 2 + 1]);
                //printf("[%d, %d]\n", edges[(i + k * 15)  * 2], edges[(i + k * 15) * 2 + 1]);
            }
        }

        fgets(buffer, 256, f);
        fclose(f);
        currentGraph = createGraph(vertexCount, edgeCount, edges);
        free(edges);
        //printf("Currently working on order %d graph %d.\n", order, currentGraphN);
        switch (startAtType)
        {
        case 0:
            goto TYPE_VERTEX_IT;
        case 1:
            goto TYPE_VERTEX_UCI;
        case 2:
            goto TYPE_TOTAL_IT;
        case 3:
            goto TYPE_TOTAL_UCI;
        }

TYPE_VERTEX_IT:
        printf("Working on Vertex Iterator.\n");
        f = fopen("Backup.txt", "w");
        fprintf(f, "Order: %d, Graph: %d.\nVertex Iterator.\n", order, currentGraphN);
        fclose(f);
        
        clk = startCounter();
        vertexGraphColoring(currentGraph);
        counterTime = getCounter(clk);
        printf("Done: %.6f\n", counterTime);
        fclose(fopen("BackupIt.txt", "w"));
        f = fopen("Results.txt", "a+");
        fprintf(f, "<order:%d, edges:%d, delta:%d, colors: %d, graph:%d, type:0, time:%.6f>\n", order, currentGraph->edgeCount, currentGraph->delta, currentGraph->chroma, currentGraphN, counterTime);
        fclose(f);
TYPE_VERTEX_UCI:
        printf("Working on Vertex UCI.\n");
        f = fopen("Backup.txt", "w");
        fprintf(f, "Order: %d, Graph: %d.\nVertex UCI.\n", order, currentGraphN);
        fclose(f);
        
        clk = startCounter();
        vertexGraphColoringUCI(currentGraph);
        counterTime = getCounter(clk);
        printf("Done: %.6f\n", counterTime);
        fclose(fopen("BackupIt.txt", "w"));
        f = fopen("Results.txt", "a+");
        fprintf(f, "<order:%d, edges:%d, delta:%d, colors: %d, graph:%d, type:1, time:%.6f>\n", order, currentGraph->edgeCount, currentGraph->delta, currentGraph->chroma, currentGraphN, counterTime);
        fclose(f);


        if (vertexCount + edgeCount > 15) {
            destroyGraph(currentGraph);
            currentGraph = NULL;
            if (stopAfterOne || currentGraphN == graph_n) {
                break;
            }
            f = fopen(fileName, "r");
            fgets(buffer, 256, f);
            for (int i = 0; i < currentGraphN; i++) {
                fgets(buffer, 256, f);
                fgets(buffer, 256, f);
                sscanf(buffer, "%d %d", &vertexCount, &edgeCount);
                for (int k = 0; k <= (edgeCount - 1)/15; k++) {
                    fgets(buffer, 256, f);
                }
                fgets(buffer, 256, f);
            }
            continue;
        }
TYPE_TOTAL_IT:
        printf("Working on Total Iterator.\n");
        f = fopen("Backup.txt", "w");
        fprintf(f, "Order: %d, Graph: %d.\nTotal Iterator.\n", order, currentGraphN);
        fclose(f);
        
        clk = startCounter();
        totalGraphColoring(currentGraph);
        counterTime = getCounter(clk);
        printf("Done: %.6f\n", counterTime);
        fclose(fopen("BackupIt.txt", "w"));
        f = fopen("Results.txt", "a+");
        fprintf(f, "<order:%d, edges:%d, delta:%d, colors: %d, graph:%d, type:2, time:%.6f>\n", order, currentGraph->edgeCount, currentGraph->delta, currentGraph->chroma, currentGraphN, counterTime);
        fclose(f);


TYPE_TOTAL_UCI:
        printf("Working on Total UCI.\n");
        f = fopen("Backup.txt", "w");
        fprintf(f, "Order: %d, Graph: %d.\nTotal UCI.\n", order, currentGraphN);
        fclose(f);
        
        clk = startCounter();
        totalGraphColoringUCI(currentGraph);
        counterTime = getCounter(clk);
        printf("Done: %.6f\n", counterTime);
        fclose(fopen("BackupIt.txt", "w"));
        f = fopen("Results.txt", "a+");
        fprintf(f, "<order:%d, edges:%d, delta:%d, colors: %d, graph:%d, type:3, time:%.6f>\n", order, currentGraph->edgeCount, currentGraph->delta, currentGraph->chroma, currentGraphN, counterTime);
        fclose(f);
        destroyGraph(currentGraph);
        currentGraph = NULL;
        if (stopAfterOne || currentGraphN == graph_n) {
            break;
        }
        f = fopen(fileName, "r");
        fgets(buffer, 256, f);
        for (int i = 0; i < currentGraphN; i++) {
            fgets(buffer, 256, f);
            fgets(buffer, 256, f);
            sscanf(buffer, "%d %d", &vertexCount, &edgeCount);
            for (int k = 0; k <= (edgeCount - 1)/15; k++) {
                fgets(buffer, 256, f);
            }
            fgets(buffer, 256, f);
        }
    }
}

void runCompleteGraphs(int orderStart, int orderEnd) {
    int * edges = malloc(sizeof(int) * (orderEnd * (orderEnd - 1)));
    int c;
    FILE * f;
    graph * currentGraph;
    pC clk;
    double counterTime;
    for (int i = orderStart; i <= orderEnd; i++) {
        c = 0;
        for (int j = 0; j < i; j++) {
            for (int k = j + 1; k < i; k++) {
                edges[c++] = j;
                edges[c++] = k;
            }
        }
        
        currentGraph = createGraph(i, c/2, edges);
        //printGraph(currentGraph, 1, 0, 0);
        /*/
        printf("Working on Vertex Iterator.\n");
        clk = startCounter();
        vertexGraphColoring(currentGraph);
        counterTime = getCounter(clk);
        printf("Done: %.6f\n", counterTime);
        f = fopen("Results2.txt", "a+");
        fprintf(f, "<order:%d, edges:%d, delta:%d, colors: %d, graph:%d, type:0, time:%.6f>\n", i, currentGraph->edgeCount, currentGraph->delta, currentGraph->chroma, 1, counterTime);
        fclose(f);
        //*/
        printf("Working on Vertex UCI.\n");
        
        clk = startCounter();
        vertexGraphColoringUCI(currentGraph);
        counterTime = getCounter(clk);
        printf("Done: %.6f\n", counterTime);
        f = fopen("Results2.txt", "a+");
        fprintf(f, "<order:%d, edges:%d, delta:%d, colors: %d, graph:%d, type:1, time:%.6f>\n", i, currentGraph->edgeCount, currentGraph->delta, currentGraph->chroma, 1, counterTime);
        fclose(f);
        destroyGraph(currentGraph);
    }

}