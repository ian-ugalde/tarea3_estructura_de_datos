#ifndef GRAFO_H
#define GRAFO_H

#include "tdas/list.h"

typedef enum { ARRIBA, ABAJO, IZQUIERDA, DERECHA, DIRECCIONES } Direccion;

typedef struct {
  char *nombre;
  int valor;
  int peso;
  int recogido;
} Item;

typedef struct {
  int id;
  char *nombre;
  char *descripcion;
  List *items;
  int conexiones[DIRECCIONES];
  int esFinal;
} Escenario;

typedef struct {
  Escenario **escenarios;
  int capacidad;
  int cantidad;
  Escenario *inicio;
} Grafo;

Grafo *grafo_cargar(const char *ruta);

Escenario *grafo_vecino(const Grafo *grafo, const Escenario *escenario,
                        Direccion dir);

void grafo_reiniciarItems(Grafo *grafo);

void grafo_liberar(Grafo *grafo);

#endif