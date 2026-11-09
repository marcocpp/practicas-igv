#ifndef __IGVESCENA3D
#define __IGVESCENA3D

#if defined(__APPLE__) && defined(__MACH__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else

#include <GL/glut.h>

#endif   // defined(__APPLE__) && defined(__MACH__)

#include "igvMallaTriangulos.h"
#include "igvGrua.h"

/// Modelos de la escena a los que se aplican las transformaciones de la P1
enum ModeloEscena
{
    MODELO_MALLA = 0 ///< La caracola (malla de triángulos paramétrica)
    , MODELO_GRUA = 1 ///< La grúa (modelo articulado)
};

const int NUM_MODELOS = 2;

/**
 * Los objetos de esta clase representan la escena 3D: el suelo, la malla
 * paramétrica (caracola) y el modelo articulado (grúa). Cada modelo tiene sus
 * propias transformaciones geométricas, como los objetos de la Práctica 1.
 */
class igvEscena3D
{
private:
    bool ejes = true; ///< Indica si hay que dibujar los ejes coordenados

    igvMallaTriangulos* malla = nullptr; ///< Malla paramétrica (una igvCaracola)
    igvGrua grua; ///< Modelo articulado

    int modeloSel = MODELO_MALLA; ///< Modelo al que afectan las transformaciones

    // Transformaciones acumuladas de cada modelo (Práctica 1), M = T * S * R
    // Posición inicial: la caracola (escala 1.3) apoyada en el suelo y fuera del
    // alcance de la pluma de la grúa durante la animación
    double tx[NUM_MODELOS] = {4.0, -2.0}, ty[NUM_MODELOS] = {0.55, 0}, tz[NUM_MODELOS] = {2.6, -0.8};
    double rx[NUM_MODELOS] = {0, 0}, ry[NUM_MODELOS] = {-40, 0}, rz[NUM_MODELOS] = {0, 0};
    double esc[NUM_MODELOS] = {1.3, 1};

    float giroMalla = 0; ///< Giro de la caracola durante la animación

    void aplicar_transformaciones(int i);
    void pintar_ejes();
    void pintar_suelo(bool seleccion);
    void material_malla();

public:
    igvEscena3D();
    ~igvEscena3D();

    // Para no copiar el puntero a la malla
    igvEscena3D(const igvEscena3D&) = delete;
    igvEscena3D& operator=(const igvEscena3D&) = delete;

    // Dibuja la escena. seleccion = true: pasada de selección por color
    void visualizar(bool seleccion = false);

    // Transformaciones de la Práctica 1 sobre el modelo seleccionado
    void seleccionar(int i);
    int get_seleccionado() const { return modeloSel; }
    void trasladar(double dx, double dy, double dz);
    void rotar(double ax, double ay, double az);
    void escalar(double factor);

    // Animación automática de la escena
    void animar(float dt);

    igvMallaTriangulos* getMalla() { return malla; }
    igvGrua& getGrua() { return grua; }

    bool get_ejes();
    void set_ejes(bool _ejes);
};

#endif   // __IGVESCENA3D
