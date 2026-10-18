#include "entrada.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void descartarLinea() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

int leerEntero(const char *mensaje) {
  char linea[32];

  while (1) {
    printf("%s", mensaje);
    if (fgets(linea, sizeof(linea), stdin) == NULL)
      exit(0);

    size_t largo = strlen(linea);
    if (largo == sizeof(linea) - 1 && linea[largo - 1] != '\n') {
      descartarLinea();
      puts("Entrada invalida, intente de nuevo.");
      continue;
    }

    char *fin;
    long numero = strtol(linea, &fin, 10);
    if (fin != linea) {
      while (*fin == ' ')
        fin++;
      if (*fin == '\n' || *fin == '\0')
        return (int)numero;
    }
    puts("Entrada invalida, intente de nuevo.");
  }
}

void esperarEnter() {
  printf("Presione Enter para continuar...");
  descartarLinea();
}