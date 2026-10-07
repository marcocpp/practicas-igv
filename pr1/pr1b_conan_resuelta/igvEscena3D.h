#ifndef __IGVESCENA3D
#define __IGVESCENA3D

#if defined(__APPLE__) && defined(__MACH__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else

#include <GL/glut.h>

#endif   // defined(__APPLE__) && defined(__MACH__)

/**
 * Los objetos de esta clase representan escenas 3D para su visualizaci�n
 */
class igvEscena3D
{
private:
    // Atributos
    bool ejes = true; ///< Indica si hay que dibujar los _ejes coordenados o no
    void pintar_casa();
    void pintar_robot();
    void pintar_arbol();
    void color(float r, float g, float b); // para fijar el color del material
    int objetoSel = 0;

    // Transformaciones acumuladas, una por objeto
    double tx[3] = {0, 0, 0}, ty[3] = {0, 0, 0}, tz[3] = {0, 0, 0};   // traslación
    double rx[3] = {0, 0, 0}, ry[3] = {0, 0, 0}, rz[3] = {0, 0, 0};   // rotación (grados)
    double esc[3] = {1, 1, 1};                                        // escala (empieza en 1, no en 0)

    void aplicar_transformaciones ( int i );

public:
    igvEscena3D() = default;
    ~igvEscena3D() = default;

    void seleccionar(int i);
    void trasladar ( double dx, double dy, double dz );
    void rotar ( double ax, double ay, double az );
    void escalar ( double factor );

    // m�todo con las llamadas OpenGL para visualizar la escena
    void visualizar();

    bool get_ejes();

    void set_ejes(bool _ejes);

private:
    void pintar_tubo();
    void pintar_ejes();
};

#endif   // __IGVESCENA3D
