#include "entrada.h"
#include "grafo.h"
#include "tdas/extra.h"
#include <stdio.h>

#define RUTA_CSV "data/graphquest.csv"

int main() {
  Grafo *grafo = NULL;
  int opcion;

  do {
    limpiarPantalla();
    puts("========================================");
    puts("               GRAPHQUEST");
    puts("========================================");
    puts("1) Cargar laberinto desde archivo CSV");
    puts("2) Iniciar partida");
    puts("3) Salir");
    opcion = leerEntero("Ingrese su opcion: ");

    switch (opcion) {
    case 1: {
      Grafo *nuevo = grafo_cargar(RUTA_CSV);
      if (nuevo == NULL) {
        puts("No se pudo cargar el laberinto.");
        break;
      }
      grafo_liberar(grafo);
      grafo = nuevo;
      printf("Laberinto cargado: %d escenarios.\n", grafo->cantidad);
      grafo_imprimir(grafo);
      break;
    }
    case 2:
      if (grafo == NULL)
        puts("Primero debe cargar el laberinto (opcion 1).");
      else
        puts("Partida pendiente de implementar.");
      break;
    case 3:
      break;
    default:
      puts("Opcion invalida.");
    }

    if (opcion != 3)
      esperarEnter();
  } while (opcion != 3);

  grafo_liberar(grafo);
  return 0;
}
