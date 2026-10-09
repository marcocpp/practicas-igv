# Explicación de detalles fundamentales de la implementación realizada

## Miembros del grupo:
- **Marco Carofiglio Pajares**
- **José María Valero Palomares**

## Enlace al video: [click aquí](https://drive.google.com/file/d/1Cnf5D0Y58yABffpLmBmpkyUpGaRPJGwu/view?usp=)

---
## Los objetos de la escena

- **Casa:** un cubo azul (el cuerpo), una pirámide roja (el tejado), y dos cubos aplastados que hacen de puerta y ventana.
- **Árbol:** un cilindro marrón (el tronco) y tres conos verdes de distinto tamaño, apilados uno sobre otro.
- **Robot:** cubos aplastados para el cuerpo, la cabeza, los brazos, las piernas y los ojos; un cilindro fino con una esfera roja en la punta forma la antena.

---

## Mover, girar y escalar los objetos

Con las teclas `1`, `2` y `3` se elige qué objeto se quiere manejar (casa, árbol o robot). Después, las demás teclas actúan **solo sobre ese objeto**.

Cada objeto guarda su propia "historia" de movimientos: cuánto se ha desplazado, cuánto se ha girado y cuánto se ha agrandado. Así, cada vez que se pulsa una tecla, el cambio **se suma** a lo que ya había.

- **Mover y girar se suman:** si giras 5° dos veces, el objeto está girado 10°.
- **El tamaño se multiplica:** si lo agrandas un 10 % dos veces, no es un 20 % más grande, sino un 21 %. Por eso el tamaño empieza valiendo 1 (tamaño normal) y no 0, porque multiplicar por 0 haría desaparecer el objeto.

---

## La cámara

### Dos formas de ver: paralela y perspectiva

- **Paralela:** no hay sensación de profundidad; los objetos lejanos se ven del mismo tamaño que los cercanos. Es como un plano técnico.
- **Perspectiva:** lo lejano se ve más pequeño, como en la vida real.

Con la tecla `p` se alterna entre las dos. La cámara guarda a la vez los ajustes de ambas formas, así que al cambiar no se pierde nada.

### El zoom

Con `+` y `-` se acerca o aleja la imagen, pero se hace de forma distinta según el tipo de cámara:

- **En paralela**, se encoge o agranda la "ventana" por la que se mira. Si la ventana es más pequeña, los objetos ocupan más espacio en pantalla.
- **En perspectiva**, se cambia el ángulo de apertura, igual que el zoom de una cámara de fotos. Se ha puesto un límite: el ángulo nunca llega a 180°, porque a partir de ahí la imagen deja de tener sentido.

### Los planos cercano y lejano

La cámara solo dibuja lo que está entre dos "paredes invisibles": una cercana y otra lejana. Lo que queda antes de la primera o después de la segunda **no se ve**.

- Las teclas `f`/`F` (y `n`/`N`) acercan o alejan la pared cercana.
- Las teclas `b`/`B` acercan o alejan la pared lejana.

Hay protecciones para que nunca se crucen: la pared cercana no puede llegar a la lejana, ni pasar por debajo de 0.1. Si un movimiento rompería esa regla, simplemente se ignora.

### Dos formas de mover la cámara

Con la tecla `c` se cambia entre **modo objeto** (el teclado mueve el objeto elegido) y **modo cámara** (el teclado mueve la cámara). En modo cámara hay dos movimientos distintos:

- **Órbita (flechas):** la cámara da vueltas alrededor del centro de la escena, siempre mirándolo. Es como caminar en círculo alrededor de una estatua. La inclinación está limitada a 89° (no llega a 90°) porque justo encima o debajo de la escena la cámara no sabría qué dirección es "arriba" y la imagen se descontrolaría.
- **Paneo (`y`/`Y`):** la cámara se queda en su sitio y **gira sobre sí misma**, como cuando giras la cabeza sin mover los pies.

---

## Varias vistas a la vez

La tecla `v` recorre en ciclo cuatro vistas de la misma escena: **panorámica ? planta ? alzado ? perfil ? panorámica…**

- **Planta:** vista desde arriba.
- **Alzado:** vista de frente.
- **Perfil:** vista de lado.

### El recuadro de la esquina

Por defecto, la ventana muestra la vista principal y, en la esquina superior derecha, un pequeño recuadro con la **vista de planta**. Para que ese recuadro quede limpio, antes de dibujarlo se borra únicamente esa zona; si no, el borrado afectaría a toda la ventana y taparía la vista principal.

### Modo de cuatro vistas

Con la tecla `4` la ventana se divide en cuatro cuadrantes y se muestran a la vez la planta, la panorámica, el alzado y el perfil. Es lo mismo que se hace en los programas de diseño 3D.

---

## La ventana y el teclado

### Resumen de teclas

| Tecla | Qué hace |
|---|---|
| `1` `2` `3` | Elegir casa, árbol o robot |
| `u` / `U` | Subir / bajar el objeto |
| `x` `y` `z` (y mayúsculas) | Girar el objeto en cada eje (mayúscula = sentido contrario) |
| `s` / `S` | Agrandar / reducir el objeto |
| Flechas | Mover el objeto (o la cámara en modo cámara) |
| `c` | Cambiar entre modo objeto y modo cámara |
| `y` / `Y` en modo cámara | Paneo de la cámara |
| `p` | Alternar cámara paralela / perspectiva |
| `+` / `-` | Zoom |
| `f` `F` `n` `N` | Mover el plano cercano |
| `b` / `B` | Mover el plano lejano |
| `v` | Cambiar de vista (panorámica, planta, alzado, perfil) |
| `4` | Mostrar cuatro vistas a la vez |
| `e` | Mostrar / ocultar los ejes |
| `Esc` | Salir |
