#include "entrada.h"
#include "grafo.h"
#include "juego.h"
#include "tdas/extra.h"
#include <stdio.h>

#define RUTA_CSV "data/graphquest.csv"

int main() {
  Grafo *grafo = NULL;
  int salir = 0;

  do {
    limpiarPantalla();
    puts("========================================");
    puts("               GRAPHQUEST");
    puts("========================================");
    puts("1) Cargar laberinto desde archivo CSV");
    puts("2) Iniciar partida");
    puts("3) Salir");
    int opcion = leerEntero("Ingrese su opción: ");

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
      break;
    }
    case 2:
      if (grafo == NULL)
        puts("Primero debe cargar el laberinto (opción 1).");
      else
        salir = jugar(grafo);
      break;
    case 3:
      salir = 1;
      break;
    default:
      puts("Opción inválida.");
    }

    if (!salir)
      esperarEnter();
  } while (!salir);

  grafo_liberar(grafo);
  return 0;
}