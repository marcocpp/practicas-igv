#ifndef __IGVCAMARA
#define __IGVCAMARA

#if defined(__APPLE__) && defined(__MACH__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else

#include <GL/glut.h>

#endif   // defined(__APPLE__) && defined(__MACH__)

#include "igvPunto3D.h"

/**
 * Etiquetas para los diferentes tipos de cámara
 */
enum tipoCamara
{
    IGV_PARALELA ///< Proyección paralela
    , IGV_FRUSTUM ///< Proyección en perspectiva usando OpenGL
    , IGV_PERSPECTIVA ///< Proyección en perspectiva usando GLU
};

/**
 * Los objetos de esta clase representan cámaras de visualización en la
 * aplicación (Práctica 1, con la órbita y el aspecto del viewport añadidos)
 */
class igvCamara
{
private:
    // atributos
    tipoCamara tipo = IGV_PARALELA; ///< Tipo de la cámara

    // ventana de visión: parámetros proyección paralela y frustum
    GLdouble xwmin = -3 ///< Coordenada X mínima del frustum/proyección paralela
    , xwmax = 3 ///< Coordenada X máxima del frustum/proyección paralela
    , ywmin = -3 ///< Coordenada Y mínima del frustum/proyección paralela
    , ywmax = 3 ///< Coordenada Y máxima del frustum/proyección paralela
    ;

    // ventana de visión: parámetros proyección perspectiva
    GLdouble angulo = 45 ///< Ángulo de apertura (proyección perspectiva)
    , raspecto = 1 ///< Razón de aspecto (proyección perspectiva)
    ;

    // distancias de planos cercano y lejano
    GLdouble znear = 1 ///< Distancia de la cámara al plano Z near
    , zfar = 200 ///< Distancia de la cámara al plano Z far
    ;

    igvPunto3D P0 = {3, 2, 4}; ///< Posición de la cámara
    igvPunto3D r = {0, 0, 0}; ///< Punto al que mira la cámara
    igvPunto3D V = {0, 1, 0}; ///< Vector que indica la vertical

    // Parámetros de la órbita: coordenadas esféricas de P0 respecto a r
    double distancia = 5.385; ///< Distancia de P0 a r
    double azimut = 36.87; ///< Ángulo horizontal en grados
    double elevacion = 21.8; ///< Ángulo vertical en grados

public:
    // Constructores por defecto y destructor
    igvCamara() = default;
    ~igvCamara() = default;

    // Otros constructores
    igvCamara(tipoCamara _tipo, igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V);

    // define la posición de la cámara
    void set(igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V);

    // define una cámara de tipo paralela o frustum
    void set(tipoCamara _tipo, igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V
             , double _xwmin, double _xwmax, double _ywmin, double _ywmax
             , double _znear, double _zfar);

    // define una cámara de tipo perspectiva
    void set(tipoCamara _tipo, igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V
             , double _angulo, double _raspecto, double _znear, double _zfar);

    // aplica la transformación de visión y la de proyección; aspecto es la
    // razón ancho / alto del viewport en el que se va a dibujar
    void aplicar(double aspecto = 1.0);

    void zoom(double factor); // realiza un zoom sobre la cámara

    // Movimientos de la Práctica 1
    void setZnear(GLdouble znear);
    void setZfar(GLdouble zfar);
    void moverPlanoDelantero(double incremento); // f/F, n/N: mueve el plano cercano
    void moverPlanoTrasero(double incremento); // b/B: mueve el plano lejano
    void orbitar(double dAzimut, double dElevacion); // órbita alrededor de r
    void sincronizarOrbita(); // recalcula distancia y ángulos desde P0 y r
    void pan(double angulo); // giro de la cámara sobre su eje vertical

    // Consulta de atributos
    tipoCamara getTipo() const;
    GLdouble getXwmin() const;
    GLdouble getXwmax() const;
    GLdouble getYwmin() const;
    GLdouble getYwmax() const;
    GLdouble getAngulo() const;
    GLdouble getRaspecto() const;
    GLdouble getZnear() const;
    GLdouble getZfar() const;
    const igvPunto3D& getP0() const;
    const igvPunto3D& getR() const;
    const igvPunto3D& getV() const;
};

#endif   // __IGVCAMARA
