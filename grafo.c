#include "grafo.h"
#include "tdas/extra.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void cargarItems(Escenario *escenario, char *texto) {
  List *partes = split_string(texto, ";");
  char *parte;

  while ((parte = list_popFront(partes)) != NULL) {
    char nombre[64];
    int valor, peso;
    if (sscanf(parte, " %63[^,],%d,%d", nombre, &valor, &peso) == 3) {
      Item *item = malloc(sizeof(Item));
      item->nombre = strdup(nombre);
      item->valor = valor;
      item->peso = peso;
      list_pushBack(escenario->items, item);
    }
    free(parte);
  }
  free(partes);
}

static Escenario *crearEscenario(char **campos) {
  Escenario *escenario = malloc(sizeof(Escenario));

  escenario->id = atoi(campos[0]);
  escenario->nombre = strdup(campos[1]);
  escenario->descripcion = strdup(campos[2]);
  escenario->items = list_create();
  cargarItems(escenario, campos[3]);
  for (int d = 0; d < DIRECCIONES; d++)
    escenario->conexiones[d] = atoi(campos[4 + d]);
  escenario->esFinal = tolower((unsigned char)campos[8][0]) == 's';
  return escenario;
}

Grafo *grafo_cargar(const char *ruta) {
  FILE *archivo = fopen(ruta, "r");
  if (archivo == NULL) {
    perror("Error al abrir el archivo");
    return NULL;
  }

  List *leidos = list_create();
  int maxId = 0;
  char **campos;

  leer_linea_csv(archivo, ',');
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    int n = 0;
    while (campos[n] != NULL)
      n++;
    if (n < 9 || atoi(campos[0]) < 1)
      continue;

    Escenario *escenario = crearEscenario(campos);
    list_pushBack(leidos, escenario);
    if (escenario->id > maxId)
      maxId = escenario->id;
  }
  fclose(archivo);

  Grafo *grafo = malloc(sizeof(Grafo));
  grafo->capacidad = maxId + 1;
  grafo->escenarios = calloc(grafo->capacidad, sizeof(Escenario *));
  grafo->cantidad = 0;
  grafo->inicio = NULL;

  Escenario *escenario;
  while ((escenario = list_popFront(leidos)) != NULL) {
    grafo->escenarios[escenario->id] = escenario;
    grafo->cantidad++;
    if (grafo->inicio == NULL)
      grafo->inicio = escenario;
  }
  free(leidos);

  if (grafo->cantidad == 0) {
    puts("El archivo no contiene escenarios validos.");
    grafo_liberar(grafo);
    return NULL;
  }
  return grafo;
}

Escenario *grafo_vecino(const Grafo *grafo, const Escenario *escenario,
                        Direccion dir) {
  int id = escenario->conexiones[dir];
  if (id < 1 || id >= grafo->capacidad)
    return NULL;
  return grafo->escenarios[id];
}

void grafo_imprimir(const Grafo *grafo) {
  for (int i = 0; i < grafo->capacidad; i++) {
    Escenario *e = grafo->escenarios[i];
    if (e == NULL)
      continue;

    printf("[%d] %s%s\n", e->id, e->nombre, e->esFinal ? " (FINAL)" : "");
    printf("    Items:");
    Item *item = list_first(e->items);
    if (item == NULL)
      printf(" ninguno");
    for (; item != NULL; item = list_next(e->items))
      printf(" %s (%d pts, %d kg);", item->nombre, item->valor, item->peso);
    printf("\n    Arriba:%d Abajo:%d Izquierda:%d Derecha:%d\n",
           e->conexiones[ARRIBA], e->conexiones[ABAJO],
           e->conexiones[IZQUIERDA], e->conexiones[DERECHA]);
  }
}

void grafo_liberar(Grafo *grafo) {
  if (grafo == NULL)
    return;

  for (int i = 0; i < grafo->capacidad; i++) {
    Escenario *e = grafo->escenarios[i];
    if (e == NULL)
      continue;

    Item *item;
    while ((item = list_popFront(e->items)) != NULL) {
      free(item->nombre);
      free(item);
    }
    free(e->items);
    free(e->nombre);
    free(e->descripcion);
    free(e);
  }
  free(grafo->escenarios);
  free(grafo);
}