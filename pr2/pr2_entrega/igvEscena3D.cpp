#include "igvEscena3D.h"
#include "igvCaracola.h"

/**
 * Constructor: crea la malla de la caracola
 * (R0 = 0.55, r0 = 0.42, H0 = 1.1, crece x1.8 por vuelta, 3 vueltas,
 *  24 divisiones en la sección y 40 por vuelta: 2904 vértices, 5760 triángulos)
 */
igvEscena3D::igvEscena3D()
{
    malla = new igvCaracola(0.55f, 0.42f, 1.1f, 1.8f, 3, 24, 40);
}

/**
 * Destructor: libera la malla (el destructor de igvMallaTriangulos es virtual)
 */
igvEscena3D::~igvEscena3D()
{
    delete malla;
    malla = nullptr;
}

/**
 * Pinta los ejes coordenados (sin iluminación, con su color exacto)
 */
void igvEscena3D::pintar_ejes()
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(1000, 0, 0);
    glVertex3f(-1000, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, 1000, 0);
    glVertex3f(0, -1000, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 1000);
    glVertex3f(0, 0, -1000);
    glEnd();
    glEnable(GL_LIGHTING);
}

/**
 * Pinta el suelo: un cuadrado de 14 x 14 dividido en celdas de 1 x 1 (así la
 * luz puntual, que se calcula en los vértices, se reparte mejor).
 * En la pasada de selección se pinta de negro, el color de "ninguna parte".
 */
void igvEscena3D::pintar_suelo(bool seleccion)
{
    if (seleccion)
    {
        glColor3ub(0, 0, 0);
    }
    else
    {
        GLfloat c1[] = {0.42f, 0.45f, 0.38f, 1};
        GLfloat negro[] = {0, 0, 0, 1};
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c1);
        glMaterialfv(GL_FRONT, GL_EMISSION, negro);
        glMaterialfv(GL_FRONT, GL_SPECULAR, negro);
    }

    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    for (int i = -7; i < 7; i++)
    {
        for (int k = -7; k < 7; k++) // antihorario visto desde arriba
        {
            glVertex3f(i, -0.001f, k);
            glVertex3f(i, -0.001f, k + 1);
            glVertex3f(i + 1, -0.001f, k + 1);
            glVertex3f(i + 1, -0.001f, k);
        }
    }
    glEnd();
}

/**
 * Material de la caracola: color crema con un brillo especular suave
 */
void igvEscena3D::material_malla()
{
    GLfloat color[] = {0.93f, 0.76f, 0.58f, 1};
    GLfloat especular[] = {0.35f, 0.35f, 0.35f, 1};
    GLfloat negro[] = {0, 0, 0, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT, GL_SHININESS, 30);
    glMaterialfv(GL_FRONT, GL_EMISSION, negro);
}

/**
 * Método con las llamadas OpenGL para visualizar la escena
 * @param seleccion true en la pasada de selección: no se pintan los ejes, la
 *        malla y el suelo se pintan de negro (tapan lo que tienen detrás, pero
 *        no se seleccionan) y la grúa con sus colores de identificador
 */
void igvEscena3D::visualizar(bool seleccion)
{
    // luces: una puntual principal y una direccional de relleno, fijas en el
    // mundo (se colocan después de la cámara y antes de los modelos)
    GLfloat luz0[] = {10, 12, 9, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, luz0);
    glEnable(GL_LIGHT0);

    GLfloat luz1[] = {-1, 0.6f, -0.4f, 0};
    GLfloat difusa1[] = {0.3f, 0.3f, 0.33f, 1};
    glLightfv(GL_LIGHT1, GL_POSITION, luz1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, difusa1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, difusa1);
    glEnable(GL_LIGHT1);

    glPushMatrix();

    if (ejes && !seleccion)
    {
        pintar_ejes();
    }

    pintar_suelo(seleccion);

    // la malla: T * S * R de la P1 y, si hay animación, su giro
    glPushMatrix();
    aplicar_transformaciones(MODELO_MALLA);
    glRotatef(giroMalla, 0, 1, 0);
    if (seleccion)
    {
        glColor3ub(0, 0, 0);
    }
    else
    {
        material_malla();
    }
    malla->visualizar();
    glPopMatrix();

    // la grúa: T * S * R de la P1; sus grados de libertad van dentro
    glPushMatrix();
    aplicar_transformaciones(MODELO_GRUA);
    grua.visualizar(seleccion);
    glPopMatrix();

    glPopMatrix();
}

/**
 * Animación automática: la grúa mueve todos sus grados de libertad y la
 * caracola gira despacio sobre sí misma
 * @param dt Segundos desde el paso anterior
 */
void igvEscena3D::animar(float dt)
{
    grua.animar(dt);
    giroMalla += 25.0f * dt; // 25 grados por segundo
    if (giroMalla >= 360) giroMalla -= 360;
}

/**
 * Método para consultar si hay que dibujar los ejes o no
 */
bool igvEscena3D::get_ejes()
{
    return ejes;
}

/**
 * Método para activar o desactivar el dibujado de los ejes
 */
void igvEscena3D::set_ejes(bool _ejes)
{
    ejes = _ejes;
}

/**
 * Elige el modelo al que se aplican las transformaciones (0 malla, 1 grúa)
 */
void igvEscena3D::seleccionar(int i)
{
    if (i >= 0 && i < NUM_MODELOS)
    {
        modeloSel = i;
    }
}

// La traslación y la rotación se suman
void igvEscena3D::trasladar(double dx, double dy, double dz)
{
    tx[modeloSel] += dx;
    ty[modeloSel] += dy;
    tz[modeloSel] += dz;
}

void igvEscena3D::rotar(double ax, double ay, double az)
{
    rx[modeloSel] += ax;
    ry[modeloSel] += ay;
    rz[modeloSel] += az;
}

// La escala se multiplica
void igvEscena3D::escalar(double factor)
{
    esc[modeloSel] *= factor;
}

/**
 * Aplica las transformaciones del modelo i en el orden M = T * S * R:
 * el modelo se gira primero, luego se escala y por último se traslada
 */
void igvEscena3D::aplicar_transformaciones(int i)
{
    glTranslatef(tx[i], ty[i], tz[i]); // T: se aplica la última
    glScalef(esc[i], esc[i], esc[i]); // S
    glRotatef(rx[i], 1, 0, 0); // R: se aplica la primera
    glRotatef(ry[i], 0, 1, 0);
    glRotatef(rz[i], 0, 0, 1);
}
