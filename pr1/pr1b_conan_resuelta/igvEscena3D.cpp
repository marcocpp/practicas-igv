#include <cstdlib>
#include <stdio.h>

#include "igvEscena3D.h"

/**
 * M�todo para pintar los ejes coordenados llamando a funciones de OpenGL
 */
void igvEscena3D::pintar_ejes()
{
    GLfloat rojo[] = {1, 0, 0, 1.0};
    GLfloat verde[] = {0, 1, 0, 1.0};
    GLfloat azul[] = {0, 0, 1, 1.0};

    glMaterialfv(GL_FRONT, GL_EMISSION, rojo);
    glBegin(GL_LINES);
        glVertex3f(1000, 0, 0);
        glVertex3f(-1000, 0, 0);
    glEnd();

    glMaterialfv(GL_FRONT, GL_EMISSION, verde);
    glBegin(GL_LINES);
        glVertex3f(0, 1000, 0);
        glVertex3f(0, -1000, 0);
    glEnd();

    glMaterialfv(GL_FRONT, GL_EMISSION, azul);
    glBegin(GL_LINES);
        glVertex3f(0, 0, 1000);
        glVertex3f(0, 0, -1000);
    glEnd();
}


void igvEscena3D::color(float r, float g, float b)
{
    GLfloat c[] = {r, g, b, 1.0};
    GLfloat nada[] = {0, 0, 0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);
    glMaterialfv(GL_FRONT, GL_EMISSION, nada); // sin emisión: el color solo depende de la luz
}

void igvEscena3D::pintar_casa()
{
    // ---------------- CUERPO ---------------
    color(0.1, 0.1, 0.7);

    glPushMatrix(); // guarda la matriz: lo que hagamos no afecta a otras partes
        glutSolidCube(1); // cubo de lado 1, ya centrado en el origen
    glPopMatrix(); // restaura la matriz

    // ---------------- TEJADO ----------------
    color(0.7, 0.1, 0.1);

    glPushMatrix();
        glTranslatef(0, 0.5, 0); // sube hasta la cara superior del cubo (Y = 0.5)
        glRotatef(-90, 1, 0, 0); // el cono de GLUT apunta a +Z; lo giramos para que apunte a +Y
        glRotatef(45, 0, 0, 1);
        glutSolidCone(0.85, 0.7, 4, 4); // base 0.85, altura 0.7, solo 4 lados => pirámide
    glPopMatrix();

    // ---------------- PUERTA ----------------
    color(0.3, 0.7, 0.05);

    glPushMatrix();
        glTranslatef(0, -0.25, 0.51); // cara frontal (Z = 0.5) + 0.01 para evitar parpadeo
        glScalef(0.25, 0.5, 0.05); // aplasta el cubo: ancho 0.25, alto 0.5, grosor 0.05
        glutSolidCube(1);
    glPopMatrix();

    // ---------------- VENTANA ----------------
    color(0.6, 0.8, 1.0);

    glPushMatrix();
        glTranslatef(0.3, 0.1, 0.51); // a la derecha de la puerta y un poco arriba
        glScalef(0.2, 0.2, 0.05); // cuadrado de 0.2 x 0.2 y poco grosor
        glutSolidCube(1);
    glPopMatrix();
}


void igvEscena3D::pintar_robot()
{
    GLUquadricObj* q = gluNewQuadric(); // cuádrica para el cilindro de la antena

    // ---------------- CUERPO ----------------
    color(0.6, 0.6, 0.65); // gris
    glPushMatrix();
        glScalef(0.7, 0.8, 0.4); // cubo aplastado: ancho 0.7, alto 0.8, fondo 0.4
        glutSolidCube(1);
    glPopMatrix();

    // ---------------- CABEZA ----------------
    color(0.8, 0.8, 0.85); // gris claro
    glPushMatrix();
        glTranslatef(0, 0.6, 0); // encima del cuerpo
        glScalef(0.4, 0.35, 0.35);
        glutSolidCube(1);
    glPopMatrix();

    // ---------------- OJOS ----------------
    color(0.9, 0.1, 0.1); // rojo
    glPushMatrix();
        glTranslatef(-0.1, 0.65, 0.18); // ojo izquierdo, en la cara frontal de la cabeza
        glScalef(0.08, 0.08, 0.05);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.1, 0.65, 0.18); // ojo derecho (X con signo contrario)
        glScalef(0.08, 0.08, 0.05);
        glutSolidCube(1);
    glPopMatrix();

    // ---------------- ANTENA ----------------
    color(0.6, 0.6, 0.65);
    glPushMatrix();
        glTranslatef(0, 0.775, 0); // sobre la parte alta de la cabeza
        glRotatef(-90, 1, 0, 0); // el cilindro crece hacia arriba
        gluCylinder(q, 0.03, 0.03, 0.25, 10, 2);
    glPopMatrix();

    color(0.9, 0.1, 0.1); // bola roja en la punta
    glPushMatrix();
        glTranslatef(0, 1.05, 0);
        glutSolidSphere(0.06, 12, 12);
    glPopMatrix();

    // ---------------- BRAZOS ----------------
    color(0.2, 0.3, 0.8); // azul
    glPushMatrix();
        glTranslatef(-0.5, 0, 0); // brazo izquierdo, pegado al lateral del cuerpo
        glScalef(0.15, 0.6, 0.15);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.5, 0, 0); // brazo derecho
        glScalef(0.15, 0.6, 0.15);
        glutSolidCube(1);
    glPopMatrix();

    // ---------------- PIERNAS ----------------
    glPushMatrix();
        glTranslatef(-0.18, -0.65, 0); // pierna izquierda, debajo del cuerpo
        glScalef(0.2, 0.5, 0.2);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.18, -0.65, 0); // pierna derecha
        glScalef(0.2, 0.5, 0.2);
        glutSolidCube(1);
    glPopMatrix();

    gluDeleteQuadric(q);
}

void igvEscena3D::pintar_arbol()
{
    GLUquadricObj* q = gluNewQuadric(); // para el cono


    // ---------------- TRONCO ----------------
    color(0.4, 0.2, 0.05);
    glPushMatrix();
        glTranslatef(0, -1, 0); // la base del cilintro se coloca en y = -1
        glRotatef(-90, 1, 0, 0);
        gluCylinder(q, 0.12, 0.12, 0.8, 20, 5);
    glPopMatrix();
    // ---------------- CONO INFERIOR ----------------
    color(0, 0.35, 0.1);
    glPushMatrix();
        glTranslatef(0, -0.5, 0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(0.6, 0.8, 20, 5);
    glPopMatrix();

    // ---------------- CONO MEDIO ----------------
    glPushMatrix();
        glTranslatef(0, 0, 0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(0.4, 0.6, 20, 5);
    glPopMatrix();

    // ---------------- CONO SUPERIOR ----------------
    glPushMatrix();
        glTranslatef(0, 0.4, 0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(0.2, 0.4, 20, 5);
    glPopMatrix();
}


/**
 * M�todo con las llamadas OpenGL para visualizar la escena
 */
void igvEscena3D::visualizar(void)
{
    // crear luces
    GLfloat luz0[] = {10, 8, 9, 1}; // luz puntual
    glLightfv(GL_LIGHT0, GL_POSITION, luz0);
    glEnable(GL_LIGHT0);

    // crear el modelo
    glPushMatrix(); // guarda la matriz de modelado

    // se pintan los ejes
    if (ejes)
    {
        pintar_ejes();
    }

    // --- se pintan los objetos de la escena ---
    pintar_casa(); // dibuja la casa en el origen

    glPushMatrix();
        glTranslatef(2, 0, 0);
        pintar_robot();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-2, 0, 0);
        pintar_arbol();
    glPopMatrix();

    glPopMatrix(); // restaura la matriz de modelado
}

/**
 * M�todo para consultar si hay que dibujar los ejes o no
 * @retval true Si hay que dibujar los ejes
 * @retval false Si no hay que dibujar los ejes
 */
bool igvEscena3D::get_ejes()
{
    return ejes;
}

/**
 * M�todo para activar o desactivar el dibujado de los ejes
 * @param _ejes Indica si hay que dibujar los ejes (true) o no (false)
 * @post El estado del objeto cambia en lo que respecta al dibujado de ejes,
 *       de acuerdo al valor pasado como par�metro
 */
void igvEscena3D::set_ejes(bool _ejes)
{
    ejes = _ejes;
}

void igvEscena3D::seleccionar(int i)
{
    if (i >= 0 && i < 3)
    {
        objetoSel = i;
    }
}
