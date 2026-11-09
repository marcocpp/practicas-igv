#include <cmath>

#include "igvCamara.h"

// Métodos constructores

/**
 * Constructor parametrizado
 * @param _tipo Tipo de cámara (IGV_PARALELA, IGV_FRUSTUM o IGV_PERSPECTIVA)
 * @param _P0 Posición de la cámara (punto de visión)
 * @param _r Punto al que mira la cámara (punto de referencia)
 * @param _V Vector que indica la vertical
 * @pre Se asume que todos los parámetros tienen valores válidos
 * @post Los atributos de la nueva cámara serán iguales a los parámetros
 */
igvCamara::igvCamara(tipoCamara _tipo, igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V)
    : tipo(_tipo), P0(_P0), r(_r), V(_V)
{
    sincronizarOrbita();
}

// Métodos públicos

/**
 * Define la posición de la cámara
 * @param _P0 Posición de la cámara (punto de visión)
 * @param _r Punto al que mira la cámara (punto de referencia)
 * @param _V Vector que indica la vertical
 * @post Cambian P0, r y V, y se recalculan los parámetros de la órbita
 */
void igvCamara::set(igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V)
{
    P0 = _P0;
    r = _r;
    V = _V;
    sincronizarOrbita();
}

/**
 * Define una cámara de tipo paralela o frustum
 * @param _tipo Tipo de la cámara (IGV_PARALELA o IGV_FRUSTUM)
 * @param _P0 Posición de la cámara
 * @param _r Punto al que mira la cámara
 * @param _V Vector que indica la vertical
 * @param _xwmin, _xwmax, _ywmin, _ywmax Ventana de visión
 * @param _znear Distancia de la cámara al plano Z near
 * @param _zfar Distancia de la cámara al plano Z far
 * @pre Se asume que todos los parámetros tienen valores válidos
 */
void igvCamara::set(tipoCamara _tipo, igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V
                    , double _xwmin, double _xwmax, double _ywmin, double _ywmax
                    , double _znear, double _zfar)
{
    tipo = _tipo;
    xwmin = _xwmin;
    xwmax = _xwmax;
    ywmin = _ywmin;
    ywmax = _ywmax;
    znear = _znear;
    zfar = _zfar;
    set(_P0, _r, _V);
}

/**
 * Define una cámara de tipo perspectiva
 * @param _tipo Tipo de la cámara (IGV_PERSPECTIVA)
 * @param _P0 Posición de la cámara
 * @param _r Punto al que mira la cámara
 * @param _V Vector que indica la vertical
 * @param _angulo Ángulo de apertura
 * @param _raspecto Razón de aspecto
 * @param _znear Distancia de la cámara al plano Z near
 * @param _zfar Distancia de la cámara al plano Z far
 * @pre Se asume que todos los parámetros tienen valores válidos
 */
void igvCamara::set(tipoCamara _tipo, igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V
                    , double _angulo, double _raspecto, double _znear, double _zfar)
{
    tipo = _tipo;
    angulo = _angulo;
    raspecto = _raspecto;
    znear = _znear;
    zfar = _zfar;
    set(_P0, _r, _V);
}

/**
 * Aplica a los objetos de la escena la transformación de visión y la
 * transformación de proyección asociadas a los parámetros de la cámara.
 * @param aspecto Razón ancho / alto del viewport. La ventana de visión se
 *        ensancha en X en esa proporción, para que la escena no se deforme
 *        aunque la ventana no sea cuadrada (con 1 se comporta como en la P1)
 */
void igvCamara::aplicar(double aspecto)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (tipo == IGV_PARALELA)
    {
        glOrtho(xwmin * aspecto, xwmax * aspecto, ywmin, ywmax, znear, zfar);
    }
    if (tipo == IGV_FRUSTUM)
    {
        glFrustum(xwmin * aspecto, xwmax * aspecto, ywmin, ywmax, znear, zfar);
    }
    if (tipo == IGV_PERSPECTIVA)
    {
        gluPerspective(angulo, raspecto * aspecto, znear, zfar);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(P0[X], P0[Y], P0[Z], r[X], r[Y], r[Z], V[X], V[Y], V[Z]);
}

/**
 * Realiza un zoom sobre la cámara
 * @param factor Factor por el que se multiplica la ventana (paralela y
 *        frustum) o el ángulo de apertura (perspectiva). Menor que 1 acerca.
 */
void igvCamara::zoom(double factor)
{
    if (tipo == IGV_PARALELA || tipo == IGV_FRUSTUM)
    {
        xwmin *= factor;
        xwmax *= factor;
        ywmin *= factor;
        ywmax *= factor;
    }
    else
    {
        if (angulo * factor < 180.0)
        {
            angulo *= factor;
        }
    }
}

void igvCamara::setZnear(GLdouble _znear)
{
    znear = _znear;
}

void igvCamara::setZfar(GLdouble _zfar)
{
    zfar = _zfar;
}

/**
 * Mueve el plano delantero del volumen de visión.
 * @param incremento Positivo: se aleja de la cámara. Negativo: se acerca.
 * @post znear se mantiene >= 0.1 y menor que zfar (si no, el cambio se ignora)
 */
void igvCamara::moverPlanoDelantero(double incremento)
{
    double nuevo = znear + incremento;
    if (nuevo >= 0.1 && nuevo < zfar - 0.1)
    {
        znear = nuevo;
    }
}

/**
 * Mueve el plano trasero del volumen de visión.
 * @param incremento Positivo: se aleja de la cámara. Negativo: se acerca.
 * @post zfar se mantiene mayor que znear (si no, el cambio se ignora)
 */
void igvCamara::moverPlanoTrasero(double incremento)
{
    double nuevo = zfar + incremento;
    if (nuevo > znear + 0.1)
    {
        zfar = nuevo;
    }
}

/**
 * Recalcula distancia, azimut y elevación de P0 respecto al punto de
 * referencia r. Se llama cada vez que cambian P0 o r.
 */
void igvCamara::sincronizarOrbita()
{
    double x = P0[X] - r[X], y = P0[Y] - r[Y], z = P0[Z] - r[Z];
    distancia = std::sqrt(x * x + y * y + z * z);
    if (distancia < 1e-9)
    {
        return;
    }
    azimut = std::atan2(x, z) * 180.0 / M_PI;
    elevacion = std::asin(y / distancia) * 180.0 / M_PI;
    if (elevacion > 89) elevacion = 89;
    if (elevacion < -89) elevacion = -89;
}

/**
 * Mueve la cámara en órbita alrededor del punto de referencia r, sin cambiar
 * la distancia: P0 = r + distancia (cos e sin a, sin e, cos e cos a).
 * Con dElevacion = 0 equivale a P0 <- r + Ry(dAzimut) (P0 - r). Lo usan las
 * flechas en modo cámara y la órbita automática.
 * @param dAzimut Incremento del ángulo horizontal (grados)
 * @param dElevacion Incremento del ángulo vertical (grados)
 * @post La elevación se limita a +-89 grados para que V no sea paralelo a la
 *       dirección de visión
 */
void igvCamara::orbitar(double dAzimut, double dElevacion)
{
    azimut += dAzimut;
    elevacion += dElevacion;
    if (elevacion > 89) elevacion = 89;
    if (elevacion < -89) elevacion = -89;

    double a = azimut * M_PI / 180.0;
    double e = elevacion * M_PI / 180.0;

    P0.set(r[X] + distancia * std::cos(e) * std::sin(a),
           r[Y] + distancia * std::sin(e),
           r[Z] + distancia * std::cos(e) * std::cos(a));
    V.set(0, 1, 0);
}

/**
 * Gira la cámara sobre su propio eje vertical (paneo): P0 no se mueve y r
 * gira alrededor de P0 en el plano XZ.
 * @param angulo Incremento del ángulo en grados
 */
void igvCamara::pan(double angulo)
{
    double rad = angulo * M_PI / 180.0;

    double dx = r[X] - P0[X];
    double dz = r[Z] - P0[Z];

    double nuevo_dx = dx * std::cos(rad) - dz * std::sin(rad);
    double nuevo_dz = dx * std::sin(rad) + dz * std::cos(rad);

    r.set(P0[X] + nuevo_dx, r[Y], P0[Z] + nuevo_dz);
    sincronizarOrbita(); // la órbita sigue al nuevo punto de referencia
}

tipoCamara igvCamara::getTipo() const { return tipo; }
GLdouble igvCamara::getXwmin() const { return xwmin; }
GLdouble igvCamara::getXwmax() const { return xwmax; }
GLdouble igvCamara::getYwmin() const { return ywmin; }
GLdouble igvCamara::getYwmax() const { return ywmax; }
GLdouble igvCamara::getAngulo() const { return angulo; }
GLdouble igvCamara::getRaspecto() const { return raspecto; }
GLdouble igvCamara::getZnear() const { return znear; }
GLdouble igvCamara::getZfar() const { return zfar; }
const igvPunto3D& igvCamara::getP0() const { return P0; }
const igvPunto3D& igvCamara::getR() const { return r; }
const igvPunto3D& igvCamara::getV() const { return V; }
