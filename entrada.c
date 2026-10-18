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
      puts("Entrada inválida, intente de nuevo.");
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
    puts("Entrada inválida, intente de nuevo.");
  }
}

void leerLinea(const char *mensaje, char *linea, int tam) {
  printf("%s", mensaje);
  if (fgets(linea, tam, stdin) == NULL)
    exit(0);
  if (strchr(linea, '\n') == NULL)
    descartarLinea();
}

void esperarEnter() {
  printf("Presione Enter para continuar...");
  descartarLinea();
}