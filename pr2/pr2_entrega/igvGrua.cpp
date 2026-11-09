#include <algorithm>
#include <cmath>

#include "igvGrua.h"

// ---------------------------------------------------------------------------
// Medidas de la grúa (en unidades del mundo; el suelo es y = 0)
// ---------------------------------------------------------------------------
namespace
{
    const float Y_TORRE = 0.5f;        ///< Altura del chasis, donde empieza la torre
    const float ALTO_TORRE = 3.0f;     ///< Altura de la torre
    const float MEDIO_TORRE = 0.18f;   ///< Mitad del lado de la torre
    const float Y_PLUMA = 0.2f;        ///< Altura de la pluma sobre la corona
    const float LARGO_PLUMA = 3.6f;    ///< Longitud de la pluma
    const float LARGO_CONTRAPLUMA = 1.5f; ///< Longitud de la contrapluma
    const float Y_CABLE = -0.1f;       ///< Punto del carro del que cuelga el cable
    const float ALTO_GANCHO = 1.15f;   ///< Del enganche del cable a la base de la carga

    // Límites de los grados de libertad
    const float AVANCE_MAX = 2.8f;     ///< La vía mide 7: las ruedas no se salen
    const float GIRO_MAX = 180.0f;     ///< Una vuelta como mucho (cable de alimentación)
    const float CARRO_MIN = 0.5f;      ///< La carga no choca con la torre
    const float CARRO_MAX = 3.2f;      ///< El carro no se sale de la punta
    const float CABLE_MIN = 0.2f;      ///< El gancho no choca con el carro
    const float GIRO_GANCHO_MAX = 90.0f;
    const float RADIO_SEGURO = 1.4f;   ///< Más cerca de la torre, la carga podría chocar con el lastre

    // Altura del enganche del cable sobre el suelo
    const float Y_ENGANCHE = Y_TORRE + ALTO_TORRE + Y_PLUMA + Y_CABLE;
    // Cable máximo: la carga queda a 0,15 del suelo, o por encima del lastre cerca de la torre
    const float CABLE_MAX_LEJOS = Y_ENGANCHE - ALTO_GANCHO - 0.15f;
    const float CABLE_MAX_CERCA = Y_ENGANCHE - ALTO_GANCHO - 0.9f;

    const int PASO_COLOR = 40; ///< El id k se codifica como rojo = k * 40 (tolerante a la precisión del buffer)

    float limitar(float v, float minimo, float maximo)
    {
        return std::max(minimo, std::min(maximo, v));
    }
}

/**
 * Destructor: libera la cuádrica
 */
igvGrua::~igvGrua()
{
    if (quadric)
    {
        gluDeleteQuadric(quadric);
        quadric = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Funciones auxiliares de dibujo
// ---------------------------------------------------------------------------

/**
 * Fija el color de la pieza que se va a dibujar a continuación
 * - En la pasada de selección: el color del identificador, sin luz.
 * - Si la parte está seleccionada: amarillo.
 * - Si no: su material normal.
 * @param parte Parte a la que pertenece la pieza (NINGUNA si es fija)
 * @param r,g,b Color normal de la pieza
 */
void igvGrua::material(int parte, float r, float g, float b)
{
    if (modoSeleccion)
    {
        glColor3ub((GLubyte) (parte * PASO_COLOR), 0, 0);
        return;
    }

    GLfloat negro[] = {0, 0, 0, 1};
    if (parte != NINGUNA && parte == seleccionada)
    {
        GLfloat amarillo[] = {1.0f, 0.85f, 0.0f, 1};
        GLfloat emision[] = {0.35f, 0.3f, 0.0f, 1};
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, amarillo);
        glMaterialfv(GL_FRONT, GL_EMISSION, emision);
    }
    else
    {
        GLfloat c[] = {r, g, b, 1};
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);
        glMaterialfv(GL_FRONT, GL_EMISSION, negro);
    }
    glMaterialfv(GL_FRONT, GL_SPECULAR, negro);
}

/**
 * Dibuja un cubo escalado (sx, sy, sz) y centrado en (cx, cy, cz).
 * La escala va entre su propio Push/Pop: no afecta a nada más.
 */
void igvGrua::caja(float cx, float cy, float cz, float sx, float sy, float sz)
{
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glScalef(sx, sy, sz);
    glutSolidCube(1);
    glPopMatrix();
}

/**
 * Dibuja una barra (cilindro fino) entre dos puntos. Se usa para las
 * celosías de la torre y la pluma, los tirantes, el cable y las eslingas.
 * gluCylinder crece en +Z: se gira el eje Z hasta la dirección d = p2 - p1
 * con un giro de ángulo acos(dz / |d|) alrededor de Z x d = (-dy, dx, 0).
 */
void igvGrua::barra(float x1, float y1, float z1, float x2, float y2, float z2, float radio)
{
    float dx = x2 - x1, dy = y2 - y1, dz = z2 - z1;
    float largo = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (largo < 1e-6f) return;

    glPushMatrix();
    glTranslatef(x1, y1, z1);
    if (std::fabs(dx) < 1e-6f && std::fabs(dy) < 1e-6f)
    {
        if (dz < 0) glRotatef(180, 1, 0, 0);
    }
    else
    {
        float angulo = std::acos(dz / largo) * 180.0f / M_PI;
        glRotatef(angulo, -dy, dx, 0);
    }
    gluCylinder(quadric, radio, radio, largo, 6, 1);
    glPopMatrix();
}

/**
 * Cilindro con tapas, vertical, desde y = 0 hasta y = alto
 */
void igvGrua::cilindroY(float radio, float alto)
{
    glPushMatrix();
    glRotatef(-90, 1, 0, 0); // el eje Z del cilindro pasa a ser +Y
    gluCylinder(quadric, radio, radio, alto, 24, 1);
    glPushMatrix();
    glTranslatef(0, 0, alto);
    gluDisk(quadric, 0, radio, 24, 1); // tapa superior
    glPopMatrix();
    glRotatef(180, 1, 0, 0);
    gluDisk(quadric, 0, radio, 24, 1); // tapa inferior, mirando hacia abajo
    glPopMatrix();
}

/**
 * Cilindro con tapas, con el eje en Z y centrado en el origen (ruedas)
 */
void igvGrua::cilindroZ(float radio, float ancho)
{
    glPushMatrix();
    glTranslatef(0, 0, -ancho / 2);
    gluCylinder(quadric, radio, radio, ancho, 20, 1);
    glPushMatrix();
    glTranslatef(0, 0, ancho);
    gluDisk(quadric, 0, radio, 20, 1);
    glPopMatrix();
    glRotatef(180, 1, 0, 0);
    gluDisk(quadric, 0, radio, 20, 1);
    glPopMatrix();
}

// ---------------------------------------------------------------------------
// Piezas: cada una centrada en su propio sistema de coordenadas
// ---------------------------------------------------------------------------

/// Vía: dos raíles de 7 unidades y traviesas (fija, no seleccionable)
void igvGrua::pintar_via()
{
    material(NINGUNA, 0.35f, 0.25f, 0.15f); // traviesas de madera
    for (float x = -3.4f; x <= 3.41f; x += 0.5f)
    {
        caja(x, 0.02f, 0, 0.14f, 0.04f, 1.3f);
    }
    material(NINGUNA, 0.55f, 0.55f, 0.6f); // raíles de acero
    caja(0, 0.07f, 0.5f, 7.0f, 0.06f, 0.08f);
    caja(0, 0.07f, -0.5f, 7.0f, 0.06f, 0.08f);
}

/// Chasis de la base: una plataforma sobre las ruedas
void igvGrua::pintar_chasis()
{
    material(BASE, 0.3f, 0.3f, 0.33f);
    caja(0, 0.4f, 0, 1.4f, 0.2f, 1.3f);
}

/// Cuatro ruedas apoyadas en los raíles (radio 0.13, sobre y = 0.10)
void igvGrua::pintar_ruedas()
{
    material(BASE, 0.1f, 0.1f, 0.1f);
    for (int i = -1; i <= 1; i += 2)
    {
        for (int k = -1; k <= 1; k += 2)
        {
            glPushMatrix();
            glTranslatef(i * 0.5f, 0.23f, k * 0.5f);
            cilindroZ(0.13f, 0.1f);
            glPopMatrix();
        }
    }
}

/// Lastre: dos bloques de hormigón sobre el chasis
void igvGrua::pintar_lastre()
{
    material(BASE, 0.62f, 0.62f, 0.58f);
    caja(-0.45f, 0.65f, 0, 0.4f, 0.3f, 1.1f);
    caja(0.45f, 0.65f, 0, 0.4f, 0.3f, 1.1f);
}

/// Torre de celosía: 4 montantes, travesaños cada 0.5 y diagonales en zigzag
void igvGrua::pintar_torre()
{
    const float a = MEDIO_TORRE;
    const float r = 0.025f;
    const int tramos = 6;
    const float h = ALTO_TORRE / tramos;
    // esquinas en el plano XZ, en orden alrededor de la torre
    const float esq[4][2] = {{-a, -a}, {a, -a}, {a, a}, {-a, a}};

    material(BASE, 0.8f, 0.12f, 0.08f);
    for (int e = 0; e < 4; e++) // montantes
    {
        barra(esq[e][0], 0, esq[e][1], esq[e][0], ALTO_TORRE, esq[e][1], 0.035f);
    }
    for (int t = 0; t <= tramos; t++)
    {
        float y = t * h;
        for (int e = 0; e < 4; e++)
        {
            int s = (e + 1) % 4;
            barra(esq[e][0], y, esq[e][1], esq[s][0], y, esq[s][1], r); // travesaño
            if (t < tramos) // diagonal: alterna el sentido en cada tramo
            {
                if (t % 2 == 0)
                    barra(esq[e][0], y, esq[e][1], esq[s][0], y + h, esq[s][1], r);
                else
                    barra(esq[s][0], y, esq[s][1], esq[e][0], y + h, esq[e][1], r);
            }
        }
    }
}

/// Corona de giro y plataforma (y de 0 a 0.2)
void igvGrua::pintar_corona()
{
    material(GIRATORIA, 0.35f, 0.35f, 0.38f);
    cilindroY(0.3f, 0.12f);
    material(GIRATORIA, 0.8f, 0.12f, 0.08f);
    caja(0, 0.16f, 0, 0.6f, 0.08f, 0.6f);
}

/// Cabina del gruista, a un lado de la torre, mirando hacia la pluma
void igvGrua::pintar_cabina()
{
    material(GIRATORIA, 0.92f, 0.92f, 0.9f);
    caja(0.35f, 0.42f, 0.42f, 0.4f, 0.4f, 0.4f);
    material(GIRATORIA, 0.15f, 0.3f, 0.5f); // cristales
    caja(0.555f, 0.47f, 0.42f, 0.02f, 0.22f, 0.32f);
    caja(0.35f, 0.47f, 0.625f, 0.32f, 0.22f, 0.02f);
}

/// Cúspide: pirámide de 4 barras sobre la corona, de la que salen los tirantes
void igvGrua::pintar_cuspide()
{
    material(GIRATORIA, 0.8f, 0.12f, 0.08f);
    for (int i = -1; i <= 1; i += 2)
    {
        for (int k = -1; k <= 1; k += 2)
        {
            barra(i * 0.15f, 0.2f, k * 0.15f, 0, 1.4f, 0, 0.03f);
        }
    }
    glPushMatrix();
    glTranslatef(0, 1.4f, 0);
    glutSolidSphere(0.06f, 12, 12);
    glPopMatrix();
}

/// Tirantes: de la cúspide a la pluma y a la contrapluma
void igvGrua::pintar_tirantes()
{
    material(GIRATORIA, 0.2f, 0.2f, 0.2f);
    barra(0, 1.4f, 0, MEDIO_TORRE + 2.6f, Y_PLUMA + 0.32f, 0, 0.012f);
    barra(0, 1.4f, 0, -MEDIO_TORRE - LARGO_CONTRAPLUMA, Y_PLUMA + 0.1f, 0, 0.012f);
}

/// Contrapluma: pasarela hacia -X con barandillas
void igvGrua::pintar_contrapluma()
{
    material(GIRATORIA, 0.8f, 0.12f, 0.08f);
    caja(-LARGO_CONTRAPLUMA / 2, 0.04f, 0, LARGO_CONTRAPLUMA, 0.08f, 0.4f);
    material(GIRATORIA, 0.85f, 0.85f, 0.85f);
    for (int k = -1; k <= 1; k += 2)
    {
        barra(0, 0.25f, k * 0.19f, -LARGO_CONTRAPLUMA, 0.25f, k * 0.19f, 0.012f);
        for (float x = 0; x >= -LARGO_CONTRAPLUMA - 0.01f; x -= 0.5f)
        {
            barra(x, 0.08f, k * 0.19f, x, 0.25f, k * 0.19f, 0.012f);
        }
    }
}

/// Contrapeso: bloque de hormigón colgado al final de la contrapluma
void igvGrua::pintar_contrapeso()
{
    material(GIRATORIA, 0.62f, 0.62f, 0.58f);
    caja(0, -0.2f, 0, 0.4f, 0.6f, 0.42f);
}

/// Pluma: celosía triangular (dos cordones abajo, uno arriba) a lo largo de +X
void igvGrua::pintar_pluma()
{
    const float L = LARGO_PLUMA, w = 0.15f, h = 0.32f, paso = 0.4f;
    const float r = 0.02f;

    material(GIRATORIA, 0.8f, 0.12f, 0.08f);
    barra(0, 0, -w, L, 0, -w, 0.03f);  // cordones inferiores (raíles del carro)
    barra(0, 0, w, L, 0, w, 0.03f);
    barra(0, h, 0, L - paso / 2, h, 0, 0.03f); // cordón superior
    for (float x = 0; x < L - 0.01f; x += paso)
    {
        float xm = x + paso / 2, x2 = x + paso;
        barra(x, 0, -w, x, 0, w, r); // travesaño inferior
        for (int k = -1; k <= 1; k += 2) // diagonales de las dos caras (Warren)
        {
            barra(x, 0, k * w, xm, h, 0, r);
            if (x2 < L)
                barra(xm, h, 0, x2, 0, k * w, r);
            else
                barra(xm, h, 0, L, 0, 0, r);
        }
    }
    caja(L, 0, 0, 0.06f, 0.08f, 2 * w + 0.06f); // remate de la punta
}

/// Carro de la pluma: cuerpo bajo los cordones y 4 ruedecillas
void igvGrua::pintar_carro()
{
    material(CARRO, 0.25f, 0.25f, 0.28f);
    caja(0, -0.06f, 0, 0.34f, 0.08f, 0.38f);
    for (int i = -1; i <= 1; i += 2)
    {
        for (int k = -1; k <= 1; k += 2)
        {
            glPushMatrix();
            glTranslatef(i * 0.1f, 0.0f, k * 0.15f);
            glutSolidSphere(0.035f, 8, 8);
            glPopMatrix();
        }
    }
}

/// Cable: dos hilos de longitud 1 escalados por el grado de libertad cable
void igvGrua::pintar_cable()
{
    material(CABLE, 0.08f, 0.08f, 0.08f);
    glPushMatrix();
    glTranslatef(0, Y_CABLE, 0);
    glScalef(1, cable, 1); // GDL 4: la escala en Y da la longitud del cable
    barra(0, 0, -0.05f, 0, -1, -0.05f, 0.012f);
    barra(0, 0, 0.05f, 0, -1, 0.05f, 0.012f);
    glPopMatrix();
}

/// Gancho: polipasto, vástago y gancho (toro)
void igvGrua::pintar_gancho()
{
    material(GANCHO, 0.95f, 0.5f, 0.05f);
    caja(0, -0.11f, 0, 0.24f, 0.22f, 0.16f);
    material(GANCHO, 0.45f, 0.45f, 0.48f);
    barra(0, -0.22f, 0, 0, -0.28f, 0, 0.02f);
    glPushMatrix();
    glTranslatef(0, -0.33f, 0);
    glutSolidTorus(0.018f, 0.055f, 8, 16);
    glPopMatrix();
}

/// Carga: un palé con una caja de madera colgado con cuatro eslingas
void igvGrua::pintar_carga()
{
    material(GANCHO, 0.15f, 0.15f, 0.15f);
    for (int i = -1; i <= 1; i += 2)
    {
        for (int k = -1; k <= 1; k += 2)
        {
            barra(0, -0.37f, 0, i * 0.24f, -0.75f, k * 0.24f, 0.008f);
        }
    }
    material(GANCHO, 0.6f, 0.4f, 0.18f);
    caja(0, -0.93f, 0, 0.55f, 0.36f, 0.55f);
    material(GANCHO, 0.42f, 0.27f, 0.12f);
    caja(0, -1.13f, 0, 0.6f, 0.04f, 0.6f);
}

// ---------------------------------------------------------------------------
// Grafo de escena
// ---------------------------------------------------------------------------

/**
 * Recorre el grafo de escena de la grúa con la pila de matrices de OpenGL.
 * La cadena principal (base -> torre -> parte giratoria -> pluma -> carro ->
 * gancho) no hace Pop entre niveles: cada nivel hereda del anterior. Los
 * hermanos (vía y base; contrapluma y pluma) van entre Push y Pop.
 * Las transformaciones marcadas con GDL son los grados de libertad.
 * @param seleccion true: pasada de selección (colores de identificador)
 */
void igvGrua::visualizar(bool seleccion)
{
    modoSeleccion = seleccion;
    if (!quadric)
    {
        quadric = gluNewQuadric();
        gluQuadricNormals(quadric, GLU_SMOOTH);
    }

    glPushMatrix(); // nivel 1: grúa
        pintar_via(); // hijo fijo

        glPushMatrix(); // nivel 2: base móvil
            glTranslatef(avance, 0, 0); // GDL 1: avance sobre los raíles
            pintar_chasis();
            pintar_ruedas();
            pintar_lastre();

            // nivel 3: torre
            glTranslatef(0, Y_TORRE, 0);
            pintar_torre();

            // nivel 4: parte giratoria
            glTranslatef(0, ALTO_TORRE, 0);
            glRotatef(giro, 0, 1, 0); // GDL 2: giro de la grúa
            pintar_corona();
            pintar_cabina();
            pintar_cuspide();
            pintar_tirantes();

            glPushMatrix(); // nivel 5: contrapluma (rama hermana de la pluma)
                glTranslatef(-MEDIO_TORRE, Y_PLUMA, 0);
                pintar_contrapluma();
                // nivel 6: contrapeso
                glTranslatef(-LARGO_CONTRAPLUMA + 0.35f, 0, 0);
                pintar_contrapeso();
            glPopMatrix();

            glPushMatrix(); // nivel 5: pluma
                glTranslatef(MEDIO_TORRE, Y_PLUMA, 0);
                pintar_pluma();

                // nivel 6: carro
                glTranslatef(carro, 0, 0); // GDL 3: carro a lo largo de la pluma
                pintar_carro();
                pintar_cable(); // GDL 4 (escala en Y dentro de la pieza)

                // nivel 7: gancho
                glTranslatef(0, Y_CABLE - cable, 0); // GDL 4: el gancho baja lo que mide el cable
                glRotatef(giroGancho, 0, 1, 0); // GDL 5: giro del gancho
                pintar_gancho();
                // nivel 8: carga
                pintar_carga();
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();

    modoSeleccion = false;
}

// ---------------------------------------------------------------------------
// Grados de libertad y sus límites
// ---------------------------------------------------------------------------

void igvGrua::setAvance(float v)
{
    avance = limitar(v, -AVANCE_MAX, AVANCE_MAX);
}

void igvGrua::setGiro(float v)
{
    giro = limitar(v, -GIRO_MAX, GIRO_MAX);
}

/**
 * Mueve el carro. Si la carga está baja, el carro no puede acercarse a la
 * torre más allá del radio seguro (la carga chocaría con el lastre).
 */
void igvGrua::setCarro(float v)
{
    float minimo = CARRO_MIN;
    if (cable > CABLE_MAX_CERCA)
    {
        minimo = std::max(minimo, RADIO_SEGURO - MEDIO_TORRE);
    }
    // si ya estaba dentro de la zona (no debería), no se le obliga a saltar
    minimo = std::min(minimo, carro);
    carro = limitar(v, minimo, CARRO_MAX);
}

/**
 * Cambia la longitud del cable. La carga nunca llega al suelo, y cerca de la
 * torre se queda por encima del lastre.
 */
void igvGrua::setCable(float v)
{
    float maximo = (MEDIO_TORRE + carro < RADIO_SEGURO) ? CABLE_MAX_CERCA : CABLE_MAX_LEJOS;
    if (v > cable) // bajar: hasta el máximo (si ya lo superaba, se queda donde está)
        cable = std::min(v, std::max(maximo, cable));
    else           // subir: hasta el mínimo
        cable = std::max(v, CABLE_MIN);
}

void igvGrua::setGiroGancho(float v)
{
    giroGancho = limitar(v, -GIRO_GANCHO_MAX, GIRO_GANCHO_MAX);
}

/**
 * Teclado: mueve un paso la parte indicada.
 * @param parte Parte (1..5)
 * @param sentido +1 (cursor derecha / arriba) o -1 (izquierda / abajo).
 *        En el cable, +1 sube el gancho (acorta el cable)
 */
void igvGrua::mover(int parte, int sentido)
{
    switch (parte)
    {
    case BASE: setAvance(avance + 0.1f * sentido); break;
    case GIRATORIA: setGiro(giro + 5.0f * sentido); break;
    case CARRO: setCarro(carro + 0.1f * sentido); break;
    case CABLE: setCable(cable - 0.1f * sentido); break;
    case GANCHO: setGiroGancho(giroGancho + 5.0f * sentido); break;
    default: break;
    }
}

/**
 * Ratón: mueve la parte según el desplazamiento del ratón con el botón pulsado
 * @param parte Parte seleccionada
 * @param dx Desplazamiento horizontal en píxeles (hacia la derecha > 0)
 * @param dy Desplazamiento vertical en píxeles (hacia arriba > 0)
 */
void igvGrua::arrastrar(int parte, int dx, int dy)
{
    switch (parte)
    {
    case BASE: setAvance(avance + 0.01f * dx); break;         // izquierda-derecha
    case GIRATORIA: setGiro(giro + 0.5f * dx); break;         // izquierda-derecha
    case CARRO: setCarro(carro + 0.01f * dx); break;          // izquierda-derecha
    case CABLE: setCable(cable - 0.01f * dy); break;          // arriba-abajo
    case GANCHO:                                              // los dos
        setGiroGancho(giroGancho + 0.5f * dx);
        setCable(cable - 0.01f * dy);
        break;
    default: break;
    }
}

namespace
{
    /**
     * Acerca un valor a su objetivo sin pasar de la velocidad máxima
     * @param actual Valor actual
     * @param objetivo Valor al que se quiere llegar
     * @param paso Máximo cambio permitido en este paso (velocidad * dt)
     */
    float acercar(float actual, float objetivo, float paso)
    {
        return actual + limitar(objetivo - actual, -paso, paso);
    }
}

/**
 * Animación automática: cada grado de libertad sigue una función seno del
 * tiempo (pose objetivo), con amplitudes dentro de sus límites. Para que el
 * movimiento sea realista, cada articulación se acerca a su objetivo con una
 * velocidad máxima: así, al activar la animación desde cualquier pose, no hay
 * saltos. Los valores pasan además por los set..., que aplican los límites.
 * @param dt Tiempo transcurrido desde el último paso, en segundos
 */
void igvGrua::animar(float dt)
{
    fase += dt;
    float objAvance = 1.5f * std::sin(0.25f * fase);
    float objGiro = 150.0f * std::sin(0.2f * fase);
    float objCable = 1.25f + 1.0f * std::sin(0.6f * fase + 1.0f);
    float objCarro = 2.2f + 0.9f * std::sin(0.45f * fase); // nunca entra en el radio seguro
    float objGancho = 70.0f * std::sin(0.8f * fase);

    // velocidades máximas (unidades o grados por segundo), algo mayores que
    // las de las funciones seno para poder seguirlas una vez alcanzadas
    setAvance(acercar(avance, objAvance, 0.6f * dt));
    setGiro(acercar(giro, objGiro, 40.0f * dt));
    setCable(acercar(cable, objCable, 0.8f * dt));
    setCarro(acercar(carro, objCarro, 0.6f * dt));
    setGiroGancho(acercar(giroGancho, objGancho, 70.0f * dt));
}

// ---------------------------------------------------------------------------
// Selección
// ---------------------------------------------------------------------------

/**
 * Traduce el color leído del buffer a la parte de la grúa (0 si no es ninguna).
 * Se redondea al múltiplo de PASO_COLOR más cercano.
 */
int igvGrua::parte_de_color(const GLubyte pixel[3])
{
    if (pixel[1] != 0 || pixel[2] != 0) return NINGUNA;
    int parte = (pixel[0] + PASO_COLOR / 2) / PASO_COLOR;
    if (parte < 1 || parte > NUM_PARTES) return NINGUNA;
    return parte;
}

/**
 * Nombre de una parte, para el título de la ventana y la consola
 */
const char* igvGrua::nombre_parte(int parte)
{
    switch (parte)
    {
    case BASE: return "base (avance)";
    case GIRATORIA: return "pluma (giro)";
    case CARRO: return "carro";
    case CABLE: return "cable";
    case GANCHO: return "gancho";
    default: return "ninguna";
    }
}
