# GraphQuest
 
## Descripción
 
GraphQuest es un juego de consola escrito en C en el que el jugador recorre un laberinto representado como un grafo. Cada nodo es un escenario con una descripción, ítems coleccionables (con puntaje y peso) y hasta cuatro salidas (arriba, abajo, izquierda, derecha). El jugador parte en la entrada principal y debe llegar a la salida tomando decisiones sobre el camino y los ítems que recoge: cada acción consume tiempo y el peso del inventario hace más lento al jugador. El objetivo es obtener el mayor puntaje posible antes de que se acabe el tiempo.
 
El laberinto se carga desde un archivo CSV (`data/graphquest.csv`).
 
Organización del código:
 
- `main.c`: menú principal (cargar laberinto, iniciar partida, salir).
- `juego.c` / `juego.h`: lógica de la partida (estado del jugador, recoger, descartar, avanzar, reiniciar, victoria y derrota).
- `grafo.c` / `grafo.h`: TDA Grafo (tipos, carga desde CSV, vecinos y liberación de memoria).
- `entrada.c` / `entrada.h`: lectura segura de números y líneas desde el teclado.
- `tdas/`: TDAs entregados por el curso (sin modificar).
## TDAs utilizados
 
El proyecto usa dos TDAs: Lista (`List`, entregada por el curso) y un Grafo explícito propio (`grafo.h` / `grafo.c`).
 
**Lista enlazada (`List`)**: se usa de tres formas.
- Cada escenario guarda sus ítems en una `List` de `Item` (nombre, valor, peso y una marca `recogido`).
- El inventario del jugador es una `List` de punteros a los mismos `Item` del escenario donde fueron recogidos (no se copian). Al descartar ítems se recorre la lista una vez con `list_popFront` y `list_pushBack`, dejando fuera los descartados y conservando el orden de los demás.
- Durante la carga del CSV se usa una `List` temporal para ir acumulando los escenarios leídos.
Los ítems recogidos no se borran del escenario: se marcan como `recogido` y dejan de mostrarse como disponibles. Así, reiniciar la partida solo requiere limpiar las marcas (`grafo_reiniciarItems`) y vaciar el inventario, sin volver a leer el archivo.
 
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
 
## Reglas del juego
 
- **Tiempo inicial:** 10 (constante `TIEMPO_INICIAL` en `juego.c`).
- **Recoger ítem(s):** se pueden elegir uno o varios ítems del escenario en una sola acción. Cuesta 1 de tiempo, sin importar cuántos ítems se recojan.
- **Descartar ítem(s):** se pueden elegir uno o varios ítems del inventario en una sola acción. Cuesta 1 de tiempo. Los ítems descartados se pierden y su puntaje se descuenta.
- **Avanzar:** cuesta `ceil((peso total del inventario + 1) / 10)` de tiempo. Con el inventario entre 0 y 9 kg cada movimiento cuesta 1; entre 10 y 19 kg cuesta 2; entre 20 y 29 kg cuesta 3, y así sucesivamente. Si el costo es mayor que el tiempo restante, el movimiento no se realiza.
- **Puntaje:** suma de los valores de los ítems que el jugador lleva en su inventario.
- **Victoria:** llegar al escenario de salida. Si se llega con el tiempo justo (queda en 0), cuenta como victoria. Se muestran los ítems del inventario y el puntaje final.
- **Derrota:** que el tiempo llegue a 0 estando en un escenario que no es la salida.
- **Reiniciar partida:** vuelve a la entrada principal con el inventario vacío, el tiempo completo y todos los ítems de nuevo en sus escenarios.
- **Salir del juego:** termina la partida y cierra la aplicación.
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
 
5. Lo primero que debes hacer es cargar el laberinto (opción 1). El programa lee automáticamente `data/graphquest.csv`, por lo que no pide la ruta. Luego puedes iniciar la partida (opción 2).
## Funcionalidades
 
### Funcionando correctamente
 
- **Cargar Laberinto:** lee `data/graphquest.csv` y construye el grafo con todos sus escenarios, ítems y conexiones (16 escenarios en el archivo de ejemplo). Se puede ejecutar más de una vez: si la nueva carga funciona reemplaza al laberinto anterior, y si falla se mantiene el anterior.
- **Iniciar Partida:** comienza en la entrada principal y muestra en cada turno la descripción del escenario, los ítems disponibles, el tiempo restante, el inventario (ítems, peso total y puntaje) y las direcciones posibles. Se puede jugar varias veces seguidas.
- **Recoger ítem(s):** permite elegir uno o varios ítems indicando sus números separados por espacios. Descuenta 1 de tiempo.
- **Descartar ítem(s):** permite elegir uno o varios ítems del inventario. Descuenta 1 de tiempo y actualiza peso y puntaje.
- **Avanzar en una dirección:** muestra solo las direcciones válidas, descuenta el tiempo según el peso del inventario y detecta la llegada a la salida (victoria) o el fin del tiempo (derrota).
- **Reiniciar Partida:** vuelve al estado inicial, incluyendo los ítems recogidos.
- **Salir del Juego:** cierra la aplicación liberando la memoria.
- **Validación de entradas:** los menús rechazan texto y opciones fuera de rango, y la selección de ítems rechaza números inexistentes. Una entrada inválida o cancelada (0 o Enter vacío) no consume tiempo.
### Problemas conocidos
 
- Los ítems descartados se pierden: no quedan en el escenario para volver a recogerlos.
- Si el costo de avanzar supera el tiempo restante, el movimiento se rechaza y la partida no termina sola hasta que el tiempo llegue a 0; el jugador puede descartar ítems para bajar el costo, reiniciar o salir.
- El laberinto siempre se carga desde `data/graphquest.csv` (la ruta no se ingresa por teclado).
- Las filas del CSV con formato inválido se ignoran sin mostrar ningún aviso.
- No se implementó el modo multijugador colaborativo, que es opcional para trabajos en pareja (este trabajo es individual).
### A mejorar
 
- Permitir ingresar por teclado la ruta del archivo CSV.
- Avisar cuando una fila del CSV es ignorada por tener formato inválido.
## Ejemplo de uso
 
### Paso 1: Cargar el laberinto
 
```
========================================
               GRAPHQUEST
========================================
1) Cargar laberinto desde archivo CSV
2) Iniciar partida
3) Salir
Ingrese su opción: 1
Laberinto cargado: 16 escenarios.
Presione Enter para continuar...
```
 
### Paso 2: Iniciar la partida
 
```
Ingrese su opción: 2
========================================
 Entrada principal
========================================
Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver.
 
Ítems disponibles:
  (ninguno)
 
Tiempo restante: 10
 
Inventario:
  (vacío)
Peso total: 0 kg | Puntaje acumulado: 0
 
Direcciones disponibles: Abajo
Costo de avanzar: 1 de tiempo
----------------------------------------
1) Recoger ítem(s)
2) Descartar ítem(s)
3) Avanzar en una dirección
4) Reiniciar partida
5) Salir del juego
Ingrese su opción:
```
 
### Paso 3: Avanzar a otro escenario
 
```
Ingrese su opción: 3
1) Abajo
Elija una dirección (0 para cancelar): 1
Llegaste a: Cocina. Tiempo usado: 1.
Presione Enter para continuar...
```
 
### Paso 4: Recoger ítems
 
En la Cocina hay dos ítems disponibles:
 
```
Ítems disponibles:
  1) Cuchillo (3 pts, 1 kg)
  2) Pan (2 pts, 1 kg)
...
Ingrese su opción: 1
Ingrese los números de los ítems separados por espacios (0 para cancelar): 1 2
Recogiste 2 ítem(s). Tiempo usado: 1.
Presione Enter para continuar...
```
 
Luego el estado muestra el tiempo restante (8), el inventario con ambos ítems, `Peso total: 2 kg | Puntaje acumulado: 5`.
 
### Paso 5: Descartar un ítem
 
```
Ingrese su opción: 2
Ingrese los números de los ítems separados por espacios (0 para cancelar): 2
Descartaste 1 ítem(s). Tiempo usado: 1.
Presione Enter para continuar...
```
 
Luego el inventario queda solo con el Cuchillo (`Peso total: 1 kg | Puntaje acumulado: 3`) y el tiempo restante es 7.
 
### Paso 6: Tiempo insuficiente para avanzar
 
Con un inventario pesado y poco tiempo, el movimiento se rechaza:
 
```
Ingrese su opción: 3
No te alcanza el tiempo: avanzar cuesta 2 y te quedan 1.
Presione Enter para continuar...
```
 
### Paso 7: Llegar a la salida (victoria)
 
Una ruta posible (Entrada principal → Cocina → Jardín → Dormitorio → Corredor → Cámara secreta → Salida) recogiendo los ítems de la Cocina, el Jardín, el Dormitorio y la Cámara secreta:
 
```
Llegaste a: Salida. Tiempo usado: 1.
 
========================================
        ¡LLEGASTE A LA SALIDA!
========================================
Ítems de tu inventario:
  1) Cuchillo (3 pts, 1 kg)
  2) Pan (2 pts, 1 kg)
  3) Moneda (2 pts, 1 kg)
  4) Rubi (20 pts, 5 kg)
  5) Mapa (10 pts, 1 kg)
Peso total: 9 kg | Puntaje acumulado: 37
Tiempo restante: 0
PUNTAJE FINAL: 37
Presione Enter para continuar...
```
 
Después de la victoria se vuelve al menú principal, donde se puede iniciar otra partida.
 
### Paso 8: Quedarse sin tiempo (derrota)
 
```
========================================
   ¡SE ACABÓ EL TIEMPO! Has perdido.
========================================
No alcanzaste la salida a tiempo.
Presione Enter para continuar...
```
 
### Paso 9: Reiniciar y salir
 
```
Ingrese su opción: 4
Partida reiniciada.
Presione Enter para continuar...
```
 
La opción 5 del menú de la partida ("Salir del juego") cierra la aplicación.
 
## Autor
 
Trabajo individual — Ian Ugalde, PUCV, Estructura de Datos (ICI2240).