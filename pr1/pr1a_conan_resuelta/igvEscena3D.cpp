#include <cstdlib>
#include <stdio.h>

#include "igvEscena3D.h"

// M�todos constructores -----------------------------------

// M�todos p�blicos ----------------------------------------

/**
 * M�todo para pintar los ejes coordenados llamando a funciones de OpenGL
 */
void igvEscena3D::pintar_ejes ()
{  GLfloat rojo[] = { 1,0,0,1.0 };
   GLfloat verde[] = { 0,1,0,1.0 };
   GLfloat azul[] = { 0,0,1,1.0 };

   glBegin(GL_LINES);
   glMaterialfv(GL_FRONT, GL_EMISSION, rojo);
   glVertex3f(1000, 0, 0);
   glVertex3f(-1000, 0, 0);

   glMaterialfv(GL_FRONT, GL_EMISSION, verde);
   glVertex3f(0, 1000, 0);
   glVertex3f(0, -1000, 0);

   glMaterialfv(GL_FRONT, GL_EMISSION, azul);
   glVertex3f(0, 0, 1000);
   glVertex3f(0, 0, -1000);
   glEnd();
}

void igvEscena3D::cajaZapatos() {
    GLfloat color_pieza[] = { 0,0.25,0 };
    GLfloat color_pieza2[] = { 0,0.3,0 };

    glMaterialfv(GL_FRONT, GL_EMISSION, color_pieza);

    glPushMatrix();
    glScalef(1, 1, 2);
    glutSolidCube(1);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_EMISSION, color_pieza2);
    glPushMatrix();
    glTranslatef(0, 0.4, 0);
    glScalef(1.1, 0.2, 2.1);
    glutSolidCube(1);
    glPopMatrix();
}

void igvEscena3D::incrPilasX() {
    nPilasX++;
};

void igvEscena3D::decrPilasX() {
    if (nPilasX > 1)
        nPilasX--;
};

void igvEscena3D::incrPilasY() {
    nPilasY++;
};

void igvEscena3D::decrPilasY() {
    if (nPilasY > 1)
        nPilasY--;
};

void igvEscena3D::incrPilasZ() {
    nPilasZ++;
};

void igvEscena3D::decrPilasZ() {
    if (nPilasZ > 1)
        nPilasZ--;
};

/**
 * M�todo con las llamadas OpenGL para visualizar la escena
 * @param escena Identificador del tipo de escena a dibujar
 * @pre Se asume que el valor del par�metro es correcto
 */
void igvEscena3D::visualizar ( int escena )
{  // borra la ventana y el Z-buffer
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   //Luces
   GLfloat light0[] = { 10, 8, 9, 1 }; // point light source
   glLightfv ( GL_LIGHT0, GL_POSITION, light0 );
   glEnable ( GL_LIGHT0 );

   glPushMatrix (); // guarda la matriz de modelado

   // se pintan los ejes
   if ( ejes )
   {  pintar_ejes ();
   }

   // Escena seleccionada a trav�s del men� (clic bot�n derecho)
   if ( escena == EscenaA )
   {  renderEscenaA ();
   }
   else
   {  if ( escena == EscenaB )
      {  renderEscenaB ();
      }
      else
      {  if ( escena == EscenaC )
         {  renderEscenaC ();
         }
      }
   }

   glPopMatrix (); // restaura la matriz de modelado
   glutSwapBuffers (); // se utiliza, en vez de glFlush(), para evitar el parpadeo
}

/**
 * Pinta la escena A llamando a las funciones de OpenGL
 */
void igvEscena3D::renderEscenaA ()
{
    cajaZapatos();
}

/**
 * Pinta la escena B llamando a las funciones de OpenGL
 */
void igvEscena3D::renderEscenaB ()
{
    GLfloat color_pieza[] = { 0,0,0.5 };

    glMaterialfv(GL_FRONT, GL_EMISSION, color_pieza);
    for (int pilasY = 0; pilasY < nPilasY; pilasY++) {
        glPushMatrix();
        glTranslatef(0, pilasY, 0);
        cajaZapatos();
        glPopMatrix();
    }
}

/**
 * Pinta la escena C llamando a las funciones de OpenGL
 */
void igvEscena3D::renderEscenaC ()
{
    GLfloat color_pieza[] = { 0,0,0.5 };
    GLfloat separacionX = 1.5;
    GLfloat separacionZ = 2.5;

    glMaterialfv(GL_FRONT, GL_EMISSION, color_pieza);
    for (int pilasY = 0; pilasY < nPilasY; pilasY++) {
        for (int pilasX = 0; pilasX < nPilasX; pilasX++) {
            for (int pilasZ = 0; pilasZ < nPilasZ; pilasZ++) {
                glPushMatrix();
                glTranslatef(pilasX * separacionX, pilasY, pilasZ * separacionZ);
                cajaZapatos();
                glPopMatrix();
            }
        }
    }
}

/**
 * M�todo para consultar si hay que dibujar los ejes o no
 * @retval true Si hay que dibujar los ejes
 * @retval false Si no hay que dibujar los ejes
 */
bool igvEscena3D::get_ejes ()
{  return ejes;
}

/**
 * M�todo para activar o desactivar el dibujado de los ejes
 * @param _ejes Indica si hay que dibujar los ejes (true) o no (false)
 * @post El estado del objeto cambia en lo que respecta al dibujado de ejes,
 *       de acuerdo al valor pasado como par�metro
 */
void igvEscena3D::set_ejes ( bool _ejes )
{  ejes = _ejes;
}




