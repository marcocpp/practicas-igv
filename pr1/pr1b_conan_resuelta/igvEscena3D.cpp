#include <cstdlib>
#include <stdio.h>

#include "igvEscena3D.h"

/**
 * M�todo para pintar los ejes coordenados llamando a funciones de OpenGL
 */
void igvEscena3D::pintar_ejes()
{  GLfloat rojo[] = { 1, 0, 0, 1.0 };
   GLfloat verde[] = { 0, 1, 0, 1.0 };
   GLfloat azul[] = { 0, 0, 1, 1.0 };

   glMaterialfv ( GL_FRONT, GL_EMISSION, rojo );
   glBegin ( GL_LINES );
   glVertex3f ( 1000, 0, 0 );
   glVertex3f ( -1000, 0, 0 );
   glEnd ();

   glMaterialfv ( GL_FRONT, GL_EMISSION, verde );
   glBegin ( GL_LINES );
   glVertex3f ( 0, 1000, 0 );
   glVertex3f ( 0, -1000, 0 );
   glEnd ();

   glMaterialfv ( GL_FRONT, GL_EMISSION, azul );
   glBegin ( GL_LINES );
   glVertex3f ( 0, 0, 1000 );
   glVertex3f ( 0, 0, -1000 );
   glEnd ();
}


void igvEscena3D::color ( float r, float g, float b )
{  GLfloat c[] = { r, g, b, 1.0 };
   GLfloat nada[] = { 0, 0, 0, 1.0 };
   glMaterialfv ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c );
   glMaterialfv ( GL_FRONT, GL_EMISSION, nada );   // sin emisión: el color solo depende de la luz
}

void igvEscena3D::pintar_casa ()
{
   // ---------------- CUERPO ---------------
   color(0.1,0.1, 0.7);

   glPushMatrix ();              // guarda la matriz: lo que hagamos no afecta a otras partes
      glutSolidCube ( 1 );       // cubo de lado 1, ya centrado en el origen
   glPopMatrix ();               // restaura la matriz

   // ---------------- TEJADO ----------------
   color(0.7,0.1, 0.1);

   glPushMatrix ();
      glTranslatef ( 0, 0.5, 0 );          // sube hasta la cara superior del cubo (Y = 0.5)
      glRotatef ( -90, 1, 0, 0 );          // el cono de GLUT apunta a +Z; lo giramos para que apunte a +Y
      glRotatef(45, 0,0,1);
      glutSolidCone ( 0.85, 0.7, 4, 4 );   // base 0.85, altura 0.7, solo 4 lados => pirámide
   glPopMatrix ();

   // ---------------- PUERTA ----------------
   color(0.3,0.7, 0.05);

   glPushMatrix ();
      glTranslatef ( 0, -0.25, 0.51 );     // cara frontal (Z = 0.5) + 0.01 para evitar parpadeo
      glScalef ( 0.25, 0.5, 0.05 );        // aplasta el cubo: ancho 0.25, alto 0.5, grosor 0.05
      glutSolidCube ( 1 );
   glPopMatrix ();

   // ---------------- VENTANA ----------------
   color(0.6,0.8, 1.0);

   glPushMatrix ();
      glTranslatef ( 0.3, 0.1, 0.51 );     // a la derecha de la puerta y un poco arriba
      glScalef ( 0.2, 0.2, 0.05 );         // cuadrado de 0.2 x 0.2 y poco grosor
      glutSolidCube ( 1 );
   glPopMatrix ();
}

// metodos publico

/**
 * M�todo con las llamadas OpenGL para visualizar la escena
 */
void igvEscena3D::visualizar(void)
{  // crear luces
   GLfloat luz0[] = { 10, 8, 9, 1 }; // luz puntual
   glLightfv ( GL_LIGHT0, GL_POSITION, luz0 );
   glEnable ( GL_LIGHT0 );

   // crear el modelo
   glPushMatrix (); // guarda la matriz de modelado

   // se pintan los ejes
   if ( ejes )
   { pintar_ejes (); }

   // --- se pintan los objetos de la escena ---
   pintar_casa ();   // dibuja la casa en el origen

   glPopMatrix (); // restaura la matriz de modelado
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