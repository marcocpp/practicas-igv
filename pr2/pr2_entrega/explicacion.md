# Práctica 2 · Representación de modelos 3D e interacción

## Miembros del grupo
- **Marco Carofiglio Pajares**
- **José María Valero Palomares**

## Enlace al vídeo: [click aquí](https://drive.google.com/file/d/1TgO2jE2ODs56DQB8zvXJjESK0jEI6lbb/view?usp=sharing)

> Comprobar que el enlace permite descargar el vídeo a cualquier docente.

---

## Resumen

La escena tiene dos modelos sobre un suelo:

- **Una caracola**: una malla de triángulos paramétrica propia, con arrays de vértices y de normales de OpenGL.
- **Una grúa torre sobre raíles**: un modelo articulado construido como grafo de escena, con primitivas de OpenGL y la pila de matrices. Tiene **8 niveles**, más de **20 nodos** con nombre (cada uno con varias primitivas), una estructura **no lineal** y **5 grados de libertad** (3 traslaciones y 2 rotaciones), todos con límites.

Las partes de la grúa se mueven con el **teclado** (teclas 1–5 y cursores) o con el **ratón** (selección por **buffer de color** y arrastre). Se cambia entre los dos modos desde el **menú**. Está integrada toda la **Práctica 1**: transformaciones de cada modelo completo, movimientos de cámara, recorte, zoom, proyecciones, vistas y viewports. También hay **animación automática** de la escena y **órbita automática** de la cámara, que se activan desde el menú.

| Requisito del enunciado | Dónde está |
|---|---|
| Objeto paramétrico con arrays de vértices y normales | `igvCaracola`, `igvMallaTriangulos` |
| Sombreado plano / interpolado por menú | Menú *Sombreado de la caracola* (y tecla `g`) |
| Grafo de escena ≥ 4 niveles, ≥ 12 nodos, no lineal | `igvGrua::visualizar()` (8 niveles) |
| ≥ 4 GDL con rotaciones y alguna traslación, con límites | `igvGrua` (5 GDL; límites en los métodos `set...`) |
| Partes con 1, 2, 3… y cursores | Modo de teclado *mover articulaciones* |
| Selección por buffer de color y arrastre con el ratón | `igvInterfaz::seleccionar()`, `mouseFunc`, `motionFunc` |
| Cambio teclado / ratón por menú | Menú *Articulaciones de la grua* (y tecla `r`) |
| Práctica 1 sobre los dos modelos, cámara y viewports | `igvEscena3D` (T·S·R por modelo), `igvCamara`, `igvInterfaz` |
| Animación del modelo y órbita de la cámara por menú | Menú *Animacion* (y teclas `a`, `o`) |

---

## Ficheros del proyecto

| Fichero | Contenido |
|---|---|
| `pr2.cpp` | `main()`: crea la ventana (800 × 800), registra los callbacks e inicia el bucle de GLUT |
| `igvInterfaz.h/.cpp` | Ventana, callbacks (teclado, ratón, menú, idle), modos, selección por color y viewports |
| `igvEscena3D.h/.cpp` | Escena: luces, suelo, ejes, los dos modelos y sus transformaciones de la P1 |
| `igvMallaTriangulos.h/.cpp` | Malla indexada de pr2a: arrays de vértices, normales e índices; `calcular_normales()` |
| `igvCaracola.h/.cpp` | Malla paramétrica de la caracola (hereda de `igvMallaTriangulos`) |
| `igvGrua.h/.cpp` | Modelo articulado: grafo de escena, grados de libertad, límites, selección y animación |
| `igvCamara.h/.cpp` | Cámara de la P1 con la órbita alrededor del punto de referencia y el aspecto del viewport |
| `igvPunto3D.h/.cpp` | Puntos y vectores 3D (sin cambios) |
| `CMakeLists.txt`, `conanfile.py`, `conandata.yml`, `conan_provider.cmake` | Configuración del proyecto (FreeGLUT con Conan) |

---

## 1. El objeto paramétrico: la caracola

### Parametrización

La caracola es un **tubo que se enrolla en una espiral logarítmica**: en cada vuelta, el tubo y la espiral crecen en la misma proporción (×1,8). Es una superficie paramétrica `P(u, v)`:

- `u ∈ [0, 2π)` recorre la sección del tubo. Es **cerrada**: la costura reutiliza los vértices de `i = 0`, igual que el cilindro de pr2a.
- `v ∈ [0, 2π · vueltas]` recorre la espiral. Es **abierta**: hay `divV + 1` anillos, y la boca de la concha queda abierta.

```
e(v)   = crecimiento^((v − V) / 2π)              factor de escala (vale 1 en la boca, v = V)
c(v)   = ( R0·e·cos v ,  H0·(1 − e) ,  R0·e·sin v )   eje del tubo: espiral que sube hacia el vértice
ρ(v)   = r0·e                                      radio del tubo
P(u,v) = c(v) + ρ(v) · ( cos u · (cos v, 0, sin v) + sin u · (0, 1, 0) )
```

Valores usados: `R0 = 0,55`, `r0 = 0,42`, `H0 = 1,1`, crecimiento 1,8, 3 vueltas, 24 divisiones en `u` y 40 por vuelta. Salen **2 904 vértices y 5 760 triángulos**.

### Vértices, triángulos y normales

- El vértice `(i, j)` ocupa la posición `k = j · divU + i` del array, y en total hay `num_vertices = divU · (divV + 1)`.
- Cada celda tiene esquinas `a = (i, j)`, `b = (i+1, j)`, `c = (i+1, j+1)` y `d = (i, j+1)`, y da los triángulos **(a, b, d)** y **(b, c, d)**. Con esta parametrización, ese es el orden **antihorario visto desde fuera**. El orden (a, d, b) de la transparencia del enunciado corresponde al toro, cuya `u` y `v` van en otro sentido, y aquí daría las caras al revés. Lo comprobamos con un programa de prueba: con (a, d, b) las 2 904 normales apuntaban hacia dentro del tubo, y con (a, b, d) apuntan todas hacia fuera.
- **Normales**: el tubo se curva y crece a la vez, así que no hay una fórmula sencilla. Se calculan **a partir de los triángulos** con el método genérico que propone el enunciado, `igvMallaTriangulos::calcular_normales()`. Se suma el producto vectorial `(b − a) × (c − a)` de cada cara en sus tres vértices y al final se normaliza. Las caras grandes pesan más, lo que da un buen promedio. Este método sirve para cualquier malla, incluida una cargada de un `.obj`.

### Dibujo con arrays de OpenGL

`igvMallaTriangulos::visualizar()` es el de pr2a. Usa `glVertexPointer` y `glNormalPointer`, y dibuja la malla entera con **una sola llamada** a `glDrawElements(GL_TRIANGLES, …)`, sin `glBegin/glEnd`. `igvCaracola` solo implementa el constructor, como `igvCilindro`.

### Sombreado plano o interpolado (menú)

- **Plano** (`GL_FLAT`): todo el triángulo toma el color calculado en su último vértice. Se ven las facetas.
- **Gouraud** (`GL_SMOOTH`): se interpolan los colores de los tres vértices. La superficie se ve lisa.

Los dos modos usan **el mismo array de normales**; solo cambia `glShadeModel`. Se elige en el menú *Sombreado de la caracola → Plano / Gouraud*, o con la tecla `g`. El menú también permite desactivar el array de normales, que en pr2a era la tecla `n`, para ver la diferencia.

### Mejoras sobre el código de pr2a

Seguimos lo que señala la guía de pr2a:

- El destructor de `igvMallaTriangulos` es **virtual**, porque la escena borra la caracola a través de un puntero a la clase base.
- `usarnormales` y `gouraud` **se inicializan** en la declaración (en la solución de pr2a no tenían valor inicial).
- Después de dibujar la malla se vuelve a `GL_SMOOTH`, para que el sombreado plano no se aplique también a la grúa.
- La caracola tiene material con un poco de brillo especular (la grúa no tiene), y las normales se dan ya normalizadas además de usar `GL_NORMALIZE`.

---

## 2. El modelo articulado: la grúa torre

Es una grúa torre que se desplaza sobre una vía. Está formada por una **base móvil** con lastre y ruedas, una **torre de celosía**, una **parte giratoria** con cabina, cúspide, tirantes, **contrapluma con contrapeso** y **pluma de celosía**, un **carro** que recorre la pluma, el **cable** y el **gancho** con una carga colgada de eslingas.

Cada pieza se dibuja **centrada en su propio sistema local** con una función `pintar_X()`. Las escalas de una pieza van dentro de su propio `glPushMatrix/glPopMatrix`, así que no llegan a sus hijos. Las primitivas son `glutSolidCube`, `glutSolidSphere`, `glutSolidTorus`, `gluCylinder` y `gluDisk`. Las celosías, tirantes, cables y eslingas se dibujan con una función `barra(p1, p2, radio)`. Esta función coloca un `gluCylinder` entre dos puntos: lo gira desde el eje Z hasta la dirección `d = p2 − p1`, con un ángulo `acos(dz/|d|)` alrededor del eje `Z × d`.

### Grafo de escena

Las transformaciones marcadas con **⟵ GDL** son las que llevan los grados de libertad. Las demás son fijas. Las transformaciones de cada nodo están en el orden en que se escriben en el código: se aplican al vértice de abajo arriba.

```
Grúa (raíz, grupo) ............................................................. nivel 1
├── Vía (fija, no seleccionable)
│     traviesas ×14: T(x, 0.02, 0)·S(0.14, 0.04, 1.3)·cubo
│     raíles ×2:     T(0, 0.07, ±0.5)·S(7, 0.06, 0.08)·cubo
│
└── Base móvil:  T(avance, 0, 0)  ⟵ GDL 1 ...................................... nivel 2
    ├── Chasis:      T(0, 0.4, 0)·S(1.4, 0.2, 1.3)·cubo
    ├── Ruedas ×4:   T(±0.5, 0.23, ±0.5)·cilindro(r 0.13) con tapas
    ├── Lastre ×2:   T(±0.45, 0.65, 0)·S(0.4, 0.3, 1.1)·cubo
    │
    └── Torre:  T(0, 0.5, 0) ................................................... nivel 3
        │     celosía: 4 montantes, 7 anillos de travesaños y diagonales en zigzag (barras)
        │
        └── Parte giratoria:  T(0, 3, 0)·Ry(giro)  ⟵ GDL 2 ...................... nivel 4
            ├── Corona:    cilindro(r 0.3, alto 0.12) + T(0, 0.16, 0)·S(0.6, 0.08, 0.6)·cubo
            ├── Cabina:    T(0.35, 0.42, 0.42)·S(0.4)·cubo + 2 cristales (cubos)
            ├── Cúspide:   4 barras hasta (0, 1.4, 0) + T(0, 1.4, 0)·esfera
            ├── Tirantes:  2 barras desde la cúspide a la pluma y a la contrapluma
            │
            ├── Contrapluma:  T(−0.18, 0.2, 0) ...................................... nivel 5
            │   │     pasarela: T(−0.75, 0.04, 0)·S(1.5, 0.08, 0.4)·cubo + barandillas (barras)
            │   └── Contrapeso:  T(−1.15, 0, 0) ...................................... nivel 6
            │         T(0, −0.2, 0)·S(0.4, 0.6, 0.42)·cubo
            │
            └── Pluma:  T(0.18, 0.2, 0) ............................................. nivel 5
                │     celosía triangular de 3.6: 3 cordones, travesaños y diagonales (barras)
                │
                └── Carro:  T(carro, 0, 0)  ⟵ GDL 3 ................................. nivel 6
                    │     T(0, −0.06, 0)·S(0.34, 0.08, 0.38)·cubo + 4 ruedecillas (esferas)
                    │
                    ├── Cable:  T(0, −0.1, 0)·S(1, cable, 1)  ⟵ GDL 4
                    │     2 barras de longitud 1 (la escala les da la longitud del cable)
                    │
                    └── Gancho:  T(0, −0.1 − cable, 0)  ⟵ GDL 4 · Ry(giroGancho)  ⟵ GDL 5 . nivel 7
                        │     polipasto T(0, −0.11, 0)·S(0.24, 0.22, 0.16)·cubo, vástago (barra)
                        │     gancho T(0, −0.33, 0)·toro
                        │
                        └── Carga ............................................................ nivel 8
                              4 eslingas (barras) + caja T(0, −0.93, 0)·S(0.55, 0.36, 0.55)·cubo
                              + palé T(0, −1.13, 0)·S(0.6, 0.04, 0.6)·cubo
```

- **Niveles**: 8 (grúa → base → torre → parte giratoria → pluma → carro → gancho → carga).
- **Nodos**: más de 20 nodos con nombre: grúa, vía, base, chasis, ruedas, lastre, torre, parte giratoria, corona, cabina, cúspide, tirantes, contrapluma, contrapeso, pluma, carro, cable, gancho y carga. Cada uno tiene varias primitivas, y las celosías tienen decenas de barras.
- **No lineal**: la raíz tiene dos hijos (vía y base). La base tiene chasis, ruedas, lastre y torre. La parte giratoria tiene corona, cabina, cúspide, tirantes, contrapluma y pluma. La pluma y la contrapluma son ramas hermanas y van cada una entre `glPushMatrix` y `glPopMatrix`.

La **cadena principal** (base → torre → parte giratoria → pluma → carro → gancho) **no hace Pop entre niveles**, porque cada nivel hereda la matriz del anterior. Los hermanos van entre Push y Pop, para que la transformación de uno no afecte al otro.

### Grados de libertad, teclas y límites

| Tecla | Parte | Grado de libertad (código) | Transformación | Límites | Ratón |
|---|---|---|---|---|---|
| `1` | base | `avance` | `T(avance, 0, 0)` en X | [−2,8; 2,8] (la vía mide 7: las ruedas no se salen) | izquierda-derecha |
| `2` | pluma (parte giratoria) | `giro` | `Ry(giro)` | [−180°; 180°] (una vuelta como mucho, como el cable de alimentación) | izquierda-derecha |
| `3` | carro | `carro` | `T(carro, 0, 0)` en X local | [0,5; 3,2] (no choca con la torre ni se sale de la punta) | izquierda-derecha |
| `4` | cable | `cable` | `S(1, cable, 1)` y `T(0, −0,1 − cable, 0)` | [0,2; máx.] (la carga nunca toca el suelo) | arriba-abajo |
| `5` | gancho | `giroGancho` | `Ry(giroGancho)` | [−90°; 90°] | izquierda-derecha gira, arriba-abajo sube y baja |

**Dónde están los grados de libertad en el código** (`igvGrua::visualizar()`, en `igvGrua.cpp`):

- GDL 1: `glTranslatef(avance, 0, 0)` (línea 424)
- GDL 2: `glRotatef(giro, 0, 1, 0)` (línea 435)
- GDL 3: `glTranslatef(carro, 0, 0)` (línea 454)
- GDL 4: `glScalef(1, cable, 1)` dentro de `pintar_cable()` (línea 363) y `glTranslatef(0, Y_CABLE - cable, 0)` (línea 459)
- GDL 5: `glRotatef(giroGancho, 0, 1, 0)` (línea 460)

**Límites realistas.** Los límites se comprueban **en un solo sitio**: los métodos `setAvance`, `setGiro`, `setCarro`, `setCable` y `setGiroGancho`. Así valen igual para el teclado, el ratón y la animación. Dos de los límites dependen de otro grado de libertad, para que no haya choques:

- El **cable** no deja que la carga baje a menos de 0,15 del suelo. Si el carro está cerca de la torre (radio < 1,4), la carga se queda por encima del lastre, porque si no chocaría con los bloques de hormigón.
- El **carro** no puede entrar en esa zona cercana a la torre mientras la carga está baja.

Las rotaciones se hacen en el punto de unión: cada nodo **traslada primero el pivote a la articulación y después gira**. Por ejemplo, la parte giratoria gira sobre el eje de la torre y el gancho sobre el cable.

---

## 3. Selección con el ratón (buffer de color)

Funciona igual que en pr2c, pero con las piezas del grafo en lugar de cajas:

1. **Clic** (`mouseFunc`): con el botón izquierdo se guardan el píxel y el estado del botón, se pasa a `modo = IGV_SELECCIONAR` y se pide redibujar. No se selecciona dentro de `mouseFunc`, porque ahí no se debe dibujar.
2. **Pasada de selección** (`igvInterfaz::seleccionar()`, llamada desde `displayFunc`). Se dibuja en el buffer trasero, **sin `glutSwapBuffers`**, así que el usuario nunca la ve. Se desactivan la iluminación, el dithering, las texturas y la eliminación de caras traseras, y el fondo se borra a negro. Cada parte de la grúa se pinta con `glColor3ub(id · 40, 0, 0)`. La caracola, el suelo y la vía se pintan de negro: tapan lo que tienen detrás, pero no se seleccionan.
3. Se lee el píxel con `glReadPixels(x, alto − 1 − y, …)`. La `y` de GLUT crece hacia abajo, así que la fila de OpenGL es `alto − 1 − y`, no `alto − y` como en la solución de pr2c. El color se traduce a la parte **redondeando al múltiplo de 40 más cercano**, lo que tolera buffers con menos bits por canal. Si el color no es de ninguna parte, se interpreta como fondo y se deselecciona.
4. La parte seleccionada **se resalta en amarillo** y la pantalla se redibuja enseguida (`glutPostRedisplay`), sin esperar a que se mueva el ratón.

**Arrastrar** (`motionFunc`): con el botón pulsado, el desplazamiento desde el evento anterior (`dx` hacia la derecha, `dy` hacia arriba) mueve el grado de libertad de la parte con un factor de escala: 0,01 unidades o 0,5° por píxel. Siempre se respetan los límites.

Detalles corregidos respecto a pr2c, siguiendo su guía:

- **El segundo clic sobre la misma parte funciona.** En la pasada de selección se pinta **siempre** el color de identificador, nunca el amarillo del resaltado. En pr2c la caja seleccionada se pintaba amarilla también en esa pasada, y por eso no se podía volver a elegir.
- Se vuelven a activar `GL_DITHER` y la iluminación, y se restaura el color de fondo.
- **Con varios viewports**, la selección usa **la cámara y el viewport de la vista en la que se ha hecho clic**. Se comprueba primero el recuadro de planta, porque está encima de la vista principal. Así se pueden seleccionar partes en cualquiera de las cuatro vistas o en el recuadro.

**Teclado o ratón.** El menú *Articulaciones de la grua* (o la tecla `r`) cambia entre:

- *Con teclado*: con el modo de teclado *mover articulaciones*, la parte se elige con `1`–`5` y se mueve con los cursores.
- *Con raton*: la parte se elige con un clic y se mueve arrastrando.

La parte elegida se resalta en amarillo en los dos modos.

---

## 4. Integración de la Práctica 1

- **Transformaciones de cada modelo completo.** Como en la P1, cada modelo guarda su traslación, sus ángulos y su escala, acumulados por separado. Se aplican en el orden `M = T · S · R`, dentro de su propio Push/Pop (`igvEscena3D::aplicar_transformaciones`). Las transformaciones de la P1 mueven el modelo entero y los grados de libertad mueven sus partes: son cosas distintas y conviven.
- **Cámara**: órbita, cabeceo y paneo, recorte con los planos cercano y lejano, zoom, proyección paralela o perspectiva, y vistas panorámica, planta, alzado y perfil.
- **Viewports**: la vista principal con el **recuadro de planta** en la esquina superior derecha, o **cuatro vistas** a la vez (planta, panorámica, alzado y perfil).

Cambios respecto a nuestro código de la P1:

- **Escena nueva.** La casa, el árbol y el robot de la P1 se sustituyen por los dos modelos de esta práctica. Las teclas `1`/`2` eligen la caracola o la grúa.
- **La órbita gira alrededor del punto al que mira la cámara** (`r`), no siempre alrededor del origen: `P0 ← r + Ry(α)·(P0 − r)`. Ahora la escena está centrada en `(0; 1,8; 0)`. Además, `sincronizarOrbita()` se llama siempre que cambian `P0` o `r`, también tras el paneo, para que la siguiente órbita no dé un salto.
- **Aspecto por viewport.** `igvCamara::aplicar(aspecto)` ensancha la ventana de visión en la proporción ancho / alto del viewport, así que la escena no se deforma aunque se cambie el tamaño de la ventana.
- **Cursores en el modo modelos**: izquierda/derecha trasladan en −X/+X y arriba/abajo en −Z/+Z, como pide la tabla del enunciado. En la P1 izquierda y derecha estaban al revés.
- **Cuatro vistas con `w`**, no con `4`. La tecla `4` ahora elige el cable en el modo *mover articulaciones*, y así el mapa de teclas no tiene conflictos.
- **ESC** termina con `exit(0)`.

---

## 5. Animación automática

- **Escena** (menú *Animacion → Animar escena*, o tecla `a`). La grúa mueve sus **cinco grados de libertad** a la vez, siguiendo funciones seno del tiempo con amplitudes dentro de los límites: avanza por la vía, gira, mueve el carro, sube y baja la carga y gira el gancho. La caracola gira despacio sobre sí misma.
  - **Sin saltos**: cada articulación se acerca a su pose objetivo con una **velocidad máxima**. Por eso, al activar la animación desde cualquier pose, el movimiento empieza suave.
  - Los valores pasan siempre por los métodos `set...`, así que **nunca hay poses imposibles**.
- **Órbita de la cámara** (menú *Animacion → Orbitar camara*, o tecla `o`): 20° por segundo alrededor del centro de la escena. Es la misma `orbitar()` de la P1. Es independiente de la animación de la escena: pueden ir a la vez, y al pararla la cámara se queda donde está.
- **Tiempo real.** `idleFunc` mide `dt` con `glutGet(GLUT_ELAPSED_TIME)`, así que la velocidad no depende del ordenador. `dt` se limita a 0,1 s para que no haya saltos tras una pausa.
- **No gasta CPU en reposo.** `idleFunc` solo se registra (`glutIdleFunc`) mientras hay algo animándose.

---

## 6. Uso del programa

El **estado está siempre a la vista en el título de la ventana**: el modo del teclado, el modelo elegido, si las articulaciones van con teclado o ratón, la parte elegida y si hay animación u órbita. La tecla `h` (o *Ayuda* en el menú) muestra en la consola todas las teclas.

### Menú (botón derecho)

| Submenú | Opciones |
|---|---|
| Sombreado de la caracola | Plano · Gouraud · Array de normales sí/no |
| Articulaciones de la grua | Con teclado (1-5 y cursores) · Con raton (clic y arrastrar) |
| Modo del teclado | Transformar modelos · Mover articulaciones · Mover camara |
| Animacion | Animar escena sí/no · Orbitar camara sí/no |
| Vistas | Siguiente vista · Cuatro vistas sí/no · Proyeccion paralela/perspectiva · Ejes sí/no |
| Ayuda (h) · Salir | |

Las opciones excluyentes marcan con `(*)` la activa, y las que alternan muestran su estado (sí / no). Las etiquetas van **sin tildes**, como recomienda el enunciado. Los textos no se pueden cambiar con el menú abierto (`glutMenuStatusFunc`), así que se actualizan en el siguiente fotograma.

### Teclas

| Tecla | Transformar modelos | Mover articulaciones | Mover cámara |
|---|---|---|---|
| `1`, `2`, … | elegir modelo (1 caracola, 2 grúa) | elegir parte (1 base … 5 gancho) | — |
| ← → | traslación en X | GDL de la parte | órbita |
| ↑ ↓ | traslación en Z | GDL de la parte | cabeceo |
| `x X` `y Y` `z Z` | rotaciones | — | `y`/`Y`: paneo |
| `s S` · `u U` | escala · traslación en Y | — | — |

Teclas comunes a todos los modos:

| Tecla | Acción |
|---|---|
| `c` | cambiar el modo del teclado (modelos → articulaciones → cámara) |
| `r` | articulaciones con teclado / con ratón |
| `a` / `o` | animación de la escena / órbita de la cámara |
| `g` | sombreado plano / Gouraud de la caracola |
| `p` | proyección paralela / perspectiva |
| `+` / `-` | zoom |
| `f F` · `n N` | plano cercano (hacia delante / atrás) |
| `b B` | plano lejano (hacia atrás / delante) |
| `v` | vista panorámica → planta → alzado → perfil |
| `w` | cuatro vistas sí/no |
| `e` | ejes sí/no |
| `h` | ayuda en la consola |
| `ESC` | salir |

### Ratón

- **Botón derecho**: menú.
- **Botón izquierdo** (con *Articulaciones → Con raton*): un clic en una parte de la grúa la selecciona y se pone amarilla. Un clic en el fondo, el suelo o la caracola la deselecciona. **Arrastrar** izquierda-derecha mueve la base, gira la pluma, mueve el carro o gira el gancho. Arrastrar arriba-abajo sube y baja el cable o el gancho.

