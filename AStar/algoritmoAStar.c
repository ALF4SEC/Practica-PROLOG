#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_CITIES 23
#define MAX_NEIGHBORS 10
#define INF 999999

// Nombres de las ciudades
typedef enum {
    ARAD, ZERIND, ORADEA, SIBIU, TIMISOARA, LUGOJ, MEHADIA, DOBRETA,
    CRAIOVA, RIMNICU_VILCEA, FAGARAS, PITESTI, BUCHAREST, GIURGIU,
    URZICENI, HIRSOVA, EFORIE, VASLUI, IASI, NEAMT
} City;

// Estructura para representar una conexión (vecino)
typedef struct {
    City city;
    int distance;
} Neighbor;

// Estructura para representar un nodo en el grafo
typedef struct {
    char* name;
    Neighbor neighbors[MAX_NEIGHBORS];
    int numNeighbors;
} GraphNode;

// Estructura para los detalles de búsqueda de cada ciudad
typedef struct {
    City parent;
    double f, g, h;
    bool inClosedList;
} CityDetails;

// Estructura para nodos en la lista abierta (cola de prioridad)
typedef struct OpenNode {
    City city;
    double f;
    struct OpenNode* next;
} OpenNode;

// Array de nombres de ciudades
char* cityNames[] = {
    "Arad", "Zerind", "Oradea", "Sibiu", "Timisoara", "Lugoj", "Mehadia",
    "Dobreta", "Craiova", "Rimnicu_Vilcea", "Fagaras", "Pitesti",
    "Bucharest", "Giurgiu", "Urziceni", "Hirsova", "Eforie", "Vaslui",
    "Iasi", "Neamt"
};

// Heurística: distancia en línea recta a Bucharest
int heuristic[MAX_CITIES] = {
    366, 374, 380, 253, 329, 244, 241, 242, 160, 193, 176, 100, 0, 77,
    80, 151, 161, 199, 226, 234
};

// Variables globales para el grafo
GraphNode graph[MAX_CITIES];

// Inicializar el grafo con las carreteras
void initGraph() {
    for (int i = 0; i < MAX_CITIES; i++) {
        graph[i].name = cityNames[i];
        graph[i].numNeighbors = 0;
    }
    
    void addRoad(City c1, City c2, int dist) {
        graph[c1].neighbors[graph[c1].numNeighbors].city = c2;
        graph[c1].neighbors[graph[c1].numNeighbors].distance = dist;
        graph[c1].numNeighbors++;
        
        graph[c2].neighbors[graph[c2].numNeighbors].city = c1;
        graph[c2].neighbors[graph[c2].numNeighbors].distance = dist;
        graph[c2].numNeighbors++;
    }
    
    // Definir todas las carreteras
    addRoad(ARAD, ZERIND, 75);
    addRoad(ARAD, SIBIU, 140);
    addRoad(ARAD, TIMISOARA, 118);
    addRoad(ZERIND, ORADEA, 71);
    addRoad(ORADEA, SIBIU, 151);
    addRoad(SIBIU, FAGARAS, 99);
    addRoad(SIBIU, RIMNICU_VILCEA, 80);
    addRoad(RIMNICU_VILCEA, PITESTI, 97);
    addRoad(PITESTI, BUCHAREST, 101);
    addRoad(FAGARAS, BUCHAREST, 211);
    addRoad(BUCHAREST, GIURGIU, 90);
    addRoad(BUCHAREST, URZICENI, 85);
    addRoad(TIMISOARA, LUGOJ, 111);
    addRoad(LUGOJ, MEHADIA, 70);
    addRoad(MEHADIA, DOBRETA, 75);
    addRoad(DOBRETA, CRAIOVA, 120);
    addRoad(CRAIOVA, RIMNICU_VILCEA, 146);
    addRoad(CRAIOVA, PITESTI, 138);
    addRoad(URZICENI, VASLUI, 142);
    addRoad(URZICENI, HIRSOVA, 98);
    addRoad(HIRSOVA, EFORIE, 86);
    addRoad(VASLUI, IASI, 92);
    addRoad(IASI, NEAMT, 87);
}

// Insertar en la lista abierta ordenadamente
void insertSorted(OpenNode** head, City city, double f) {
    OpenNode* newNode = (OpenNode*)malloc(sizeof(OpenNode));
    newNode->city = city;
    newNode->f = f;
    newNode->next = NULL;
    
    if (*head == NULL || f < (*head)->f) {
        newNode->next = *head;
        *head = newNode;
    } else {
        OpenNode* current = *head;
        while (current->next != NULL && current->next->f < f) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// Extraer el nodo con menor f
City extractMin(OpenNode** head) {
    if (*head == NULL) return -1;
    
    OpenNode* temp = *head;
    City city = temp->city;
    *head = (*head)->next;
    free(temp);
    return city;
}

// Algoritmo A* para encontrar distancia entre dos ciudades
int aStarDistance(City src, City dest, City path[], int* pathLen) {
    if (src == dest) {
        path[0] = src;
        *pathLen = 1;
        return 0;
    }
    
    CityDetails details[MAX_CITIES];
    for (int i = 0; i < MAX_CITIES; i++) {
        details[i].f = INF;
        details[i].g = INF;
        details[i].h = heuristic[i];
        details[i].parent = i;
        details[i].inClosedList = false;
    }
    
    details[src].f = 0.0;
    details[src].g = 0.0;
    details[src].parent = src;
    
    OpenNode* openList = NULL;
    insertSorted(&openList, src, 0.0);
    
    while (openList != NULL) {
        City current = extractMin(&openList);
        if (current == -1) break;
        
        details[current].inClosedList = true;
        
        if (current == dest) {
            // Reconstruir camino
            *pathLen = 0;
            City c = dest;
            while (details[c].parent != c) {
                path[(*pathLen)++] = c;
                c = details[c].parent;
            }
            path[(*pathLen)++] = c;
            
            // Invertir el camino
            for (int i = 0; i < *pathLen / 2; i++) {
                City temp = path[i];
                path[i] = path[*pathLen - 1 - i];
                path[*pathLen - 1 - i] = temp;
            }
            
            // Liberar memoria
            while (openList != NULL) extractMin(&openList);
            
            return (int)details[dest].g;
        }
        
        for (int i = 0; i < graph[current].numNeighbors; i++) {
            City neighbor = graph[current].neighbors[i].city;
            int distance = graph[current].neighbors[i].distance;
            
            if (details[neighbor].inClosedList) continue;
            
            double gNew = details[current].g + distance;
            double hNew = heuristic[neighbor];
            double fNew = gNew + hNew;
            
            if (details[neighbor].f == INF || details[neighbor].f > fNew) {
                insertSorted(&openList, neighbor, fNew);
                details[neighbor].f = fNew;
                details[neighbor].g = gNew;
                details[neighbor].parent = current;
            }
        }
    }
    
    *pathLen = 0;
    return -1; // No hay camino
}

// Función para generar permutaciones y encontrar el mejor tour TSP
void swap(City* a, City* b) {
    City temp = *a;
    *a = *b;
    *b = temp;
}

void tspPermutations(City cities[], int start, int end, City bestTour[], int* bestDistance) {
    if (start == end) {
        // Calcular distancia total del tour actual
        int totalDist = 0;
        City path[MAX_CITIES];
        int pathLen;
        
        // Desde Bucharest a la primera ciudad
        int dist = aStarDistance(BUCHAREST, cities[0], path, &pathLen);
        if (dist == -1) return;
        totalDist += dist;
        
        // Entre las ciudades a visitar
        for (int i = 0; i < end; i++) {
            dist = aStarDistance(cities[i], cities[i + 1], path, &pathLen);
            if (dist == -1) return;
            totalDist += dist;
        }
        
        // Regreso a Bucharest
        dist = aStarDistance(cities[end], BUCHAREST, path, &pathLen);
        if (dist == -1) return;
        totalDist += dist;
        
        // Si es mejor, actualizar
        if (totalDist < *bestDistance) {
            *bestDistance = totalDist;
            for (int i = 0; i <= end; i++) {
                bestTour[i] = cities[i];
            }
        }
        return;
    }
    
    for (int i = start; i <= end; i++) {
        swap(&cities[start], &cities[i]);
        tspPermutations(cities, start + 1, end, bestTour, bestDistance);
        swap(&cities[start], &cities[i]); // backtrack
    }
}

// Resolver TSP para Bucharest -> Iasi, Sibiu, Lugoj -> Bucharest
void solveTSP() {
    printf("===========================================\n");
    printf("  TSP - Mapa de Rumanía\n");
    printf("===========================================\n\n");
    printf("Ciudades a visitar: Bucharest -> Iasi, Sibiu, Lugoj -> Bucharest\n\n");
    
    // Ciudades a visitar (sin Bucharest)
    City citiesToVisit[] = {IASI, SIBIU, LUGOJ};
    int numCities = 3;
    
    City bestTour[3];
    int bestDistance = INF;
    
    // Generar todas las permutaciones y encontrar la mejor
    tspPermutations(citiesToVisit, 0, numCities - 1, bestTour, &bestDistance);
    
    if (bestDistance == INF) {
        printf("No se encontró solución\n");
        return;
    }
    
    // Mostrar resultado
    printf("=== SOLUCIÓN ENCONTRADA ===\n\n");
    printf("Mejor ruta:\n");
    printf("  %s", cityNames[BUCHAREST]);
    
    for (int i = 0; i < numCities; i++) {
        printf(" -> %s", cityNames[bestTour[i]]);
    }
    printf(" -> %s\n\n", cityNames[BUCHAREST]);
    
    printf("Distancia total: %d km\n\n", bestDistance);
    
    // Mostrar detalles de cada tramo
    printf("Detalle de tramos usando A*:\n");
    City path[MAX_CITIES];
    int pathLen;
    
    int dist = aStarDistance(BUCHAREST, bestTour[0], path, &pathLen);
    printf("\n1. %s -> %s: %d km\n", cityNames[BUCHAREST], cityNames[bestTour[0]], dist);
    printf("   Ruta: ");
    for (int i = 0; i < pathLen; i++) {
        printf("%s", cityNames[path[i]]);
        if (i < pathLen - 1) printf(" -> ");
    }
    
    for (int i = 0; i < numCities - 1; i++) {
        dist = aStarDistance(bestTour[i], bestTour[i + 1], path, &pathLen);
        printf("\n\n%d. %s -> %s: %d km\n", i + 2, cityNames[bestTour[i]], cityNames[bestTour[i + 1]], dist);
        printf("   Ruta: ");
        for (int j = 0; j < pathLen; j++) {
            printf("%s", cityNames[path[j]]);
            if (j < pathLen - 1) printf(" -> ");
        }
    }
    
    dist = aStarDistance(bestTour[numCities - 1], BUCHAREST, path, &pathLen);
    printf("\n\n%d. %s -> %s: %d km\n", numCities + 1, cityNames[bestTour[numCities - 1]], cityNames[BUCHAREST], dist);
    printf("   Ruta: ");
    for (int i = 0; i < pathLen; i++) {
        printf("%s", cityNames[path[i]]);
        if (i < pathLen - 1) printf(" -> ");
    }
    
    printf("\n\n===========================================\n");
}

int main() {
    initGraph();
    solveTSP();
    return 0;
}
