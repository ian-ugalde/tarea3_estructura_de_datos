#include "juego.h"
#include "entrada.h"
#include "tdas/extra.h"
#include <stdio.h>
#include <stdlib.h>

#define TIEMPO_INICIAL 10

typedef struct {
  Escenario *actual;
  List *inventario;
  int peso;
  int puntaje;
  int tiempo;
} Jugador;

static const char *NOMBRES_DIRECCION[DIRECCIONES] = {"Arriba", "Abajo",
                                                      "Izquierda", "Derecha"};

static int costoAvanzar(int peso) { return (peso + 10) / 10; }

static int contarDisponibles(const Escenario *escenario) {
  int total = 0;
  for (Item *item = list_first(escenario->items); item != NULL;
       item = list_next(escenario->items))
    if (!item->recogido)
      total++;
  return total;
}

static void mostrarInventario(const Jugador *j) {
  int n = 0;
  for (Item *item = list_first(j->inventario); item != NULL;
       item = list_next(j->inventario))
    printf("  %d) %s (%d pts, %d kg)\n", ++n, item->nombre, item->valor,
           item->peso);
  if (n == 0)
    puts("  (vacío)");
  printf("Peso total: %d kg | Puntaje acumulado: %d\n", j->peso, j->puntaje);
}

static void mostrarEstado(const Grafo *grafo, const Jugador *j) {
  puts("========================================");
  printf(" %s\n", j->actual->nombre);
  puts("========================================");
  puts(j->actual->descripcion);

  puts("\nÍtems disponibles:");
  int n = 0;
  for (Item *item = list_first(j->actual->items); item != NULL;
       item = list_next(j->actual->items))
    if (!item->recogido)
      printf("  %d) %s (%d pts, %d kg)\n", ++n, item->nombre, item->valor,
             item->peso);
  if (n == 0)
    puts("  (ninguno)");

  printf("\nTiempo restante: %d\n", j->tiempo);
  puts("\nInventario:");
  mostrarInventario(j);

  printf("\nDirecciones disponibles:");
  n = 0;
  for (int d = 0; d < DIRECCIONES; d++)
    if (grafo_vecino(grafo, j->actual, d) != NULL)
      printf("%s%d) %s", n++ ? ", " : " ", d + 1, NOMBRES_DIRECCION[d]);
  if (n == 0)
    printf(" (ninguna)");
  printf("\nCosto de avanzar: %d de tiempo\n", costoAvanzar(j->peso));
  puts("----------------------------------------");
}

static int leerSeleccion(int cantidad, int *marcados) {
  char linea[128];
  char *fin;
  char *p = linea;
  int total = 0;

  leerLinea("Ingrese los números de los ítems separados por espacios (0 para "
            "cancelar): ",
            linea, sizeof(linea));

  for (int i = 0; i < cantidad; i++)
    marcados[i] = 0;

  while (1) {
    long n = strtol(p, &fin, 10);
    if (fin == p)
      break;
    if (n == 0)
      return 0;
    if (n < 1 || n > cantidad)
      return -1;
    if (!marcados[n - 1]) {
      marcados[n - 1] = 1;
      total++;
    }
    p = fin;
  }

  while (*p == ' ' || *p == '\n')
    p++;
  return *p == '\0' ? total : -1;
}

static void recoger(Jugador *j) {
  int cantidad = contarDisponibles(j->actual);
  if (cantidad == 0) {
    puts("No hay ítems para recoger en este escenario.");
    return;
  }

  int *marcados = calloc(cantidad, sizeof(int));
  int total = leerSeleccion(cantidad, marcados);

  if (total < 0) {
    puts("Selección inválida.");
  } else if (total == 0) {
    puts("Acción cancelada.");
  } else {
    int i = 0;
    for (Item *item = list_first(j->actual->items); item != NULL;
         item = list_next(j->actual->items)) {
      if (item->recogido)
        continue;
      if (marcados[i++]) {
        item->recogido = 1;
        list_pushBack(j->inventario, item);
        j->peso += item->peso;
        j->puntaje += item->valor;
      }
    }
    j->tiempo--;
    printf("Recogiste %d ítem(s). Tiempo usado: 1.\n", total);
  }
  free(marcados);
}

static void descartar(Jugador *j) {
  int cantidad = list_size(j->inventario);
  if (cantidad == 0) {
    puts("Tu inventario está vacío.");
    return;
  }

  int *marcados = calloc(cantidad, sizeof(int));
  int total = leerSeleccion(cantidad, marcados);

  if (total < 0) {
    puts("Selección inválida.");
  } else if (total == 0) {
    puts("Acción cancelada.");
  } else {
    for (int i = 0; i < cantidad; i++) {
      Item *item = list_popFront(j->inventario);
      if (marcados[i]) {
        j->peso -= item->peso;
        j->puntaje -= item->valor;
      } else {
        list_pushBack(j->inventario, item);
      }
    }
    j->tiempo--;
    printf("Descartaste %d ítem(s). Tiempo usado: 1.\n", total);
  }
  free(marcados);
}

static void avanzar(const Grafo *grafo, Jugador *j) {
  int n = 0;
  for (int d = 0; d < DIRECCIONES; d++)
    if (grafo_vecino(grafo, j->actual, d) != NULL)
      n++;

  if (n == 0) {
    puts("No hay direcciones disponibles desde este escenario.");
    return;
  }

  int costo = costoAvanzar(j->peso);
  if (costo > j->tiempo) {
    printf("No te alcanza el tiempo: avanzar cuesta %d y te quedan %d.\n",
           costo, j->tiempo);
    return;
  }

  for (int d = 0; d < DIRECCIONES; d++)
    if (grafo_vecino(grafo, j->actual, d) != NULL)
      printf("%d) %s\n", d + 1, NOMBRES_DIRECCION[d]);
  int opcion = leerEntero("Elija una dirección (0 para cancelar): ");

  if (opcion == 0) {
    puts("Acción cancelada.");
  } else if (opcion < 1 || opcion > DIRECCIONES ||
             grafo_vecino(grafo, j->actual, opcion - 1) == NULL) {
    puts("Dirección inválida.");
  } else {
    j->actual = grafo_vecino(grafo, j->actual, opcion - 1);
    j->tiempo -= costo;
    printf("Llegaste a: %s. Tiempo usado: %d.\n", j->actual->nombre, costo);
  }
}

static void reiniciar(Grafo *grafo, Jugador *j) {
  list_clean(j->inventario);
  grafo_reiniciarItems(grafo);
  j->actual = grafo->inicio;
  j->peso = 0;
  j->puntaje = 0;
  j->tiempo = TIEMPO_INICIAL;
}

static void mostrarVictoria(const Jugador *j) {
  puts("\n========================================");
  puts("        ¡LLEGASTE A LA SALIDA!");
  puts("========================================");
  puts("Ítems de tu inventario:");
  mostrarInventario(j);
  printf("Tiempo restante: %d\n", j->tiempo);
  printf("PUNTAJE FINAL: %d\n", j->puntaje);
}

static void mostrarDerrota() {
  puts("\n========================================");
  puts("   ¡SE ACABÓ EL TIEMPO! Has perdido.");
  puts("========================================");
  puts("No alcanzaste la salida a tiempo.");
}

static void liberarJugador(Jugador *j) {
  list_clean(j->inventario);
  free(j->inventario);
}

int jugar(Grafo *grafo) {
  Jugador j;
  j.inventario = list_create();
  reiniciar(grafo, &j);

  while (1) {
    limpiarPantalla();
    mostrarEstado(grafo, &j);
    puts("1) Recoger ítem(s)");
    puts("2) Descartar ítem(s)");
    puts("3) Avanzar en una dirección");
    puts("4) Reiniciar partida");
    puts("5) Salir del juego");
    int opcion = leerEntero("Ingrese su opción: ");

    switch (opcion) {
    case 1:
      recoger(&j);
      break;
    case 2:
      descartar(&j);
      break;
    case 3:
      avanzar(grafo, &j);
      break;
    case 4:
      reiniciar(grafo, &j);
      puts("Partida reiniciada.");
      break;
    case 5:
      liberarJugador(&j);
      return 1;
    default:
      puts("Opción inválida.");
    }

    if (j.actual->esFinal) {
      mostrarVictoria(&j);
      break;
    }
    if (j.tiempo <= 0) {
      mostrarDerrota();
      break;
    }
    esperarEnter();
  }

  liberarJugador(&j);
  return 0;
}