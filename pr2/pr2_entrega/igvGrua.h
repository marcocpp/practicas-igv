#ifndef __IGVGRUA
#define __IGVGRUA

#if defined(__APPLE__) && defined(__MACH__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else

#include <GL/glut.h>

#endif   // defined(__APPLE__) && defined(__MACH__)

/**
 * Partes móviles (seleccionables) de la grúa. El valor es el identificador
 * que se usa con las teclas 1..5 y en la selección por buffer de color.
 * NINGUNA (0) corresponde a lo que no se puede seleccionar (vía, fondo...).
 */
enum ParteGrua
{
    NINGUNA = 0 ///< Nada seleccionado / partes fijas
    , BASE = 1 ///< Carro de la base: se traslada sobre los raíles
    , GIRATORIA = 2 ///< Parte giratoria (pluma, contrapluma, cabina): gira en Y
    , CARRO = 3 ///< Carro de la pluma: se traslada a lo largo de la pluma
    , CABLE = 4 ///< Cable: sube y baja el gancho
    , GANCHO = 5 ///< Gancho con la carga: gira en Y
};

const int NUM_PARTES = 5; ///< Número de partes móviles

/**
 * Los objetos de esta clase representan una grúa torre sobre raíles, modelada
 * como un grafo de escena con primitivas de OpenGL y la pila de matrices.
 *
 * Grados de libertad (todos con límites, que se comprueban en los métodos
 * set..., así valen igual para el teclado, el ratón y la animación):
 *  1. avance      traslación en X de la base sobre los raíles
 *  2. giro        rotación en Y de la parte giratoria
 *  3. carro       traslación en X del carro a lo largo de la pluma
 *  4. cable       longitud del cable: traslación en Y del gancho
 *  5. giroGancho  rotación en Y del gancho y la carga
 */
class igvGrua
{
private:
    // Grados de libertad
    float avance = 0;       ///< GDL 1: traslación de la base en X
    float giro = 0;         ///< GDL 2: giro de la parte giratoria (grados)
    float carro = 2.0f;     ///< GDL 3: posición del carro en la pluma
    float cable = 1.0f;     ///< GDL 4: longitud del cable
    float giroGancho = 0;   ///< GDL 5: giro del gancho (grados)

    float fase = 0;         ///< Tiempo acumulado de la animación (segundos)

    int seleccionada = NINGUNA; ///< Parte resaltada en amarillo
    bool modoSeleccion = false; ///< true durante la pasada de selección por color
    GLUquadric* quadric = nullptr; ///< Cuádrica para cilindros y discos

    // Funciones auxiliares de dibujo
    void material(int parte, float r, float g, float b);
    void caja(float cx, float cy, float cz, float sx, float sy, float sz);
    void barra(float x1, float y1, float z1, float x2, float y2, float z2, float radio);
    void cilindroY(float radio, float alto);
    void cilindroZ(float radio, float ancho);

    // Una función por pieza: cada una se dibuja en su sistema local
    void pintar_via();
    void pintar_chasis();
    void pintar_ruedas();
    void pintar_lastre();
    void pintar_torre();
    void pintar_corona();
    void pintar_cabina();
    void pintar_cuspide();
    void pintar_tirantes();
    void pintar_contrapluma();
    void pintar_contrapeso();
    void pintar_pluma();
    void pintar_carro();
    void pintar_cable();
    void pintar_gancho();
    void pintar_carga();

public:
    igvGrua() = default;
    ~igvGrua();

    // Dibuja la grúa. Si seleccion es true, cada parte se pinta sin luz con
    // su color de identificador (pasada de selección por buffer de color)
    void visualizar(bool seleccion = false);

    // Grados de libertad: cada set aplica los límites
    void setAvance(float v);
    void setGiro(float v);
    void setCarro(float v);
    void setCable(float v);
    void setGiroGancho(float v);
    float getAvance() const { return avance; }
    float getGiro() const { return giro; }
    float getCarro() const { return carro; }
    float getCable() const { return cable; }
    float getGiroGancho() const { return giroGancho; }

    // Interacción
    void mover(int parte, int sentido);          // teclado: un paso en el sentido (+1 / -1)
    void arrastrar(int parte, int dx, int dy);   // ratón: desplazamiento en píxeles
    void animar(float dt);                       // animación automática

    // Selección
    void set_seleccionada(int parte) { seleccionada = parte; }
    int get_seleccionada() const { return seleccionada; }
    static int parte_de_color(const GLubyte pixel[3]);
    static const char* nombre_parte(int parte);
};

#endif   // __IGVGRUA
