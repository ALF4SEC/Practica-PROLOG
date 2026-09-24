# Práctica de búsqueda: viajante sobre el mapa de Rumanía

Práctica de Inteligencia Artificial sobre el problema del viajante (TSP) en el mapa de carreteras de Rumanía que usa el libro de Russell y Norvig. Hay dos versiones: una en C que resuelve el problema con A* y otra en Visual Prolog, basada en backtracking, que está a medio hacer.

## Versión en C con A* (`AStar/`)

El programa busca la ruta más corta que sale de Bucharest, pasa por Iasi, Sibiu y Lugoj y vuelve a Bucharest.

1. El mapa se guarda como un grafo no dirigido de 20 ciudades y 23 carreteras con sus distancias en kilómetros.
2. La distancia entre cada par de ciudades se calcula con A*. La heurística es la distancia euclídea entre las coordenadas aproximadas de cada ciudad y el destino, así que se recalcula para cada destino en lugar de usar la tabla fija de distancias a Bucharest. La lista abierta es una lista enlazada ordenada por `f`.
3. Se prueban todas las permutaciones del orden de visita de las tres ciudades y se queda la de menor distancia total.

La salida muestra la mejor ruta, la distancia total y cada tramo con las ciudades intermedias por las que pasa.

```bash
cd AStar
make
./algoritmoAStar
```

Hace falta `gcc` y `make`; se enlaza con la librería matemática (`-lm`). El ejecutable `algoritmoAStar` que hay en el repositorio está compilado para Linux x86-64.

## Versión en Visual Prolog (`Prolog/`)

Proyecto de Visual Prolog que iba a resolver el mismo problema por backtracking. Tiene el mapa de carreteras como hechos `carretera/3` y el inicio de los predicados auxiliares (`camino`, `miembro` e `invertir_lista`), pero no está terminado y no compila tal como está: hay dominios mal escritos, predicados incompletos y la sección `goal` está vacía.

| Archivo | Contenido |
|---|---|
| `algoritmoBacktrackTSP.pro` | Código fuente. |
| `algoritmoBacktrackTSP.inc`, `.pre`, `.vpr` | Archivos de proyecto generados por Visual Prolog. |
| `algoritmoBacktrackTSP.BAK`, `.bk1` | Copias de seguridad automáticas del IDE. |
| `Exe/prolog.err` | Tabla de mensajes de error del entorno de Visual Prolog. |
