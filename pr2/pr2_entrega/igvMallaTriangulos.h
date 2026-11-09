#ifndef __IGVMALLATRIANGULOS
#define __IGVMALLATRIANGULOS

#if defined(__APPLE__) && defined(__MACH__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else

#include <GL/glut.h>

#endif   // defined(__APPLE__) && defined(__MACH__)

/**
 * Los objetos de esta clase representan mallas de triángulos indexadas
 * (lista de vértices + lista de caras), basadas en la clase de pr2a.
 * Se dibujan con arrays de vértices y de normales de OpenGL (glDrawElements).
 */
class igvMallaTriangulos
{
protected:
    // Atributos
    long int num_vertices = 0;          ///< Número de vértices de la malla
    float* vertices = nullptr;          ///< (num_vertices * 3) coordenadas de los vértices
    float* normales = nullptr;          ///< (num_vertices * 3) componentes de la normal en cada vértice

    long int num_triangulos = 0;        ///< Número de triángulos de la malla
    unsigned int* triangulos = nullptr; ///< (num_triangulos * 3) índices a los vértices de cada triángulo

    bool usarnormales = true;           ///< Usar el array de normales al dibujar (en pr2a, tecla n)
    bool gouraud = true;                ///< true: GL_SMOOTH (Gouraud); false: GL_FLAT (plano)

public:
    // Constructores y destructor
    /// Constructor por defecto
    igvMallaTriangulos() = default;

    igvMallaTriangulos(long int _num_vertices, float* _vertices
                       , long int _num_triangulos, unsigned int* _triangulos);

    /// Destructor virtual: la malla se borra a través de un puntero a la clase base
    virtual ~igvMallaTriangulos();

    // Método con las llamadas OpenGL para visualizar la malla de triángulos
    void visualizar();

    // Calcula las normales de los vértices a partir de los triángulos
    void calcular_normales();

    // Sombreado y uso de normales
    void cambiarnormales() { usarnormales = !usarnormales; }
    void cambiarvis() { gouraud = !gouraud; }
    void setGouraud(bool _gouraud) { gouraud = _gouraud; }
    bool getGouraud() const { return gouraud; }
    void setUsarNormales(bool _usar) { usarnormales = _usar; }
    bool getUsarNormales() const { return usarnormales; }

    long int getNumVertices() const { return num_vertices; }
    long int getNumTriangulos() const { return num_triangulos; }
};

#endif   // __IGVMALLATRIANGULOS
