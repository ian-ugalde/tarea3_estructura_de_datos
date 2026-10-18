# GraphQuest
 
## Descripción
 
GraphQuest es un juego de consola escrito en C en el que el jugador recorre un laberinto representado como un grafo. Cada nodo es un escenario con una descripción, ítems coleccionables (con puntaje y peso) y hasta cuatro salidas (arriba, abajo, izquierda, derecha). El jugador parte en la entrada principal y debe llegar a la salida tomando decisiones sobre el camino y los ítems que recoge: cada movimiento consume tiempo y el peso del inventario lo hace más lento. El objetivo es obtener el mayor puntaje posible antes de que se acabe el tiempo.
 
El laberinto se carga desde un archivo CSV (`data/graphquest.csv`).
 
## TDAs utilizados
 
El proyecto usa dos TDAs: Lista (`List`, entregada por el curso) y un Grafo explícito propio (`grafo.h` / `grafo.c`).
 
**Lista enlazada (`List`)**: cada escenario guarda sus ítems en una `List` de `Item` (nombre, valor y peso). Durante la carga del CSV también se usa una `List` temporal para ir acumulando los escenarios leídos.
 
**Grafo explícito (`Grafo`)**:
- **Nodos:** cada escenario es un `Escenario` con `id`, `nombre`, `descripcion`, su lista de ítems, un indicador `esFinal` y el arreglo `conexiones[4]`.
- **Aristas:** `conexiones[dir]` guarda el ID del escenario al que se llega en esa dirección (`ARRIBA`, `ABAJO`, `IZQUIERDA`, `DERECHA`), o `-1` si no hay camino. Las aristas son dirigidas: en el archivo de ejemplo se puede pasar desde el escenario 12 y desde el 15 a la Salida (16), pero la Salida no tiene conexiones de vuelta.
- **Grafo:** la estructura `Grafo` contiene un arreglo `escenarios` indexado por ID (de tamaño ID máximo + 1; las posiciones sin escenario quedan en `NULL`), la cantidad de escenarios y un puntero `inicio` al escenario inicial, que es el primero que aparece en el archivo.
- **Moverse entre escenarios:** `grafo_vecino(grafo, escenario, dir)` toma el ID guardado en `conexiones[dir]` y entrega el escenario accediendo directamente al arreglo, por lo que cada movimiento cuesta O(1). Si no hay conexión, retorna `NULL`.
- **Carga (`grafo_cargar`):** se hace en dos pasos, con costo total O(n) para n escenarios. Primero se lee el archivo fila por fila y se crea un `Escenario` por cada fila válida (los ítems se separan por `;` y cada uno se interpreta como `nombre,valor,peso`), calculando el ID máximo. Luego se crea el arreglo indexado por ID y cada escenario se ubica en su posición.
- **Memoria:** `grafo_liberar` libera ítems, listas, textos y escenarios. Se llama al volver a cargar el laberinto y al salir del programa.
No se usó el Map del curso para buscar escenarios por ID porque por dentro es una lista enlazada (cada búsqueda recorre nodo por nodo). Con un arreglo indexado por ID el acceso es directo.
 
## Formato del archivo CSV
 
Una fila por escenario, con encabezado en la primera línea:
 
| Columna | Contenido |
|---|---|
| `ID` | Identificador entero positivo del escenario |
| `Nombre` | Nombre del escenario |
| `Descripcion` | Texto descriptivo (puede ir entre comillas y contener comas) |
| `Items` | Ítems separados por `;`, cada uno como `nombre,valor,peso`. Vacío si no hay ítems |
| `Arriba`, `Abajo`, `Izquierda`, `Derecha` | ID del escenario al que se llega en esa dirección, o `-1` si no hay salida |
| `EsFinal` | `Si` para el escenario de salida, `No` en los demás |
 
En el archivo de ejemplo los ítems vienen como **nombre, valor (puntos), peso (kg)**: `Libro antiguo,6,2` es un libro de 6 puntos y 2 kg, igual que en el diagrama del enunciado. El enunciado escribe "(nombre, peso, valor)", pero los datos y el diagrama siguen el orden valor, peso, y así se interpretan en este programa.
 
Las filas con menos de 9 campos o con ID menor a 1 se ignoran.
 
## Cómo compilar y ejecutar
 
Este proyecto fue desarrollado en Replit y se ejecuta desde la Shell.
 
### Requisitos previos
 
- Replit (o cualquier entorno con `gcc`).
- El archivo `data/graphquest.csv` debe existir dentro del proyecto (viene incluido en el repositorio).
### Pasos para compilar y ejecutar
 
1. Descarga o clona el repositorio completo.
2. Abre una terminal (Shell) y ubícate en la carpeta raíz del proyecto, que es la que contiene `main.c`, `grafo.c`, `data/` y `tdas/`:
   - En Replit, la Shell se abre directamente en esa carpeta.
   - Si clonaste el repositorio en otro computador, entra a la carpeta con `cd nombre-del-repositorio`.
   - Para comprobar que estás en el lugar correcto, ejecuta `ls`: deben aparecer `main.c`, `grafo.c`, `data` y `tdas`.
3. Compila con el siguiente comando:
```bash
gcc tdas/*.c *.c -Wno-unused-result -o tarea3
```
 
4. Ejecuta el programa con:
```bash
./tarea3
```
5. Lo primero que debes hacer es cargar el laberinto (opción 1). El programa lee automáticamente `data/graphquest.csv`, por lo que no pide la ruta.
## Funcionalidades
 
### Funcionando correctamente
 
- **Cargar Laberinto:** lee `data/graphquest.csv` y construye el grafo con todos sus escenarios, ítems y conexiones (16 escenarios en el archivo de ejemplo). Se puede ejecutar más de una vez: si la nueva carga funciona reemplaza al laberinto anterior, y si falla se mantiene el anterior.
- **Validación del menú:** rechaza texto y opciones fuera de rango sin cerrar el programa.
- **Salir:** finaliza la ejecución liberando la memoria del grafo.
### Problemas conocidos
 
- **Iniciar Partida:** aún no está implementada; por ahora el programa solo informa que está pendiente.
- El laberinto siempre se carga desde `data/graphquest.csv` (la ruta no se ingresa por teclado).
- Las filas del CSV con formato inválido se ignoran sin mostrar ningún aviso.
### A mejorar
 
- Permitir ingresar por teclado la ruta del archivo CSV.
## Ejemplo de uso
 
### Paso 1: Cargar el laberinto
 
```
========================================
               GRAPHQUEST
========================================
1) Cargar laberinto desde archivo CSV
2) Iniciar partida
3) Salir
Ingrese su opcion: 1
Laberinto cargado: 16 escenarios.
```
 
## Autor
 
Trabajo individual — Ian Ugalde, PUCV, Estructura de Datos (ICI2240).
 