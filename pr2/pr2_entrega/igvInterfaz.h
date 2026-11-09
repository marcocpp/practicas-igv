#ifndef __IGVINTERFAZ
#define __IGVINTERFAZ

#if defined(__APPLE__) && defined(__MACH__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else

#include <GL/glut.h>

#endif   // defined(__APPLE__) && defined(__MACH__)

#include <string>

#include "igvEscena3D.h"
#include "igvCamara.h"

/**
 * Modos de dibujo de la interfaz (como en pr2c)
 */
enum modoInterfaz
{
    IGV_VISUALIZAR ///< Visualización normal de la escena
    , IGV_SELECCIONAR ///< Se ha hecho clic: la siguiente pasada es de selección
};

/**
 * Qué controla el teclado (las teclas comunes funcionan en todos los modos)
 */
enum modoTeclado
{
    TECLADO_MODELOS ///< Transformaciones de la P1 sobre el modelo completo
    , TECLADO_ARTICULACIONES ///< Grados de libertad de la grúa (1..5 y cursores)
    , TECLADO_CAMARA ///< Órbita, cabeceo y paneo de la cámara
};

/**
 * Cómo se mueven las articulaciones de la grúa (se elige en el menú)
 */
enum modoArticulaciones
{
    ARTICULACIONES_TECLADO ///< Teclas 1..5 para elegir la parte y cursores para moverla
    , ARTICULACIONES_RATON ///< Clic para elegir la parte (buffer de color) y arrastrar
};

/**
 * Opciones de los menús
 */
enum opcionMenu
{
    M_PLANO, M_GOURAUD, M_NORMALES
    , M_ART_TECLADO, M_ART_RATON
    , M_TEC_MODELOS, M_TEC_ARTICULACIONES, M_TEC_CAMARA
    , M_ANIMAR, M_ORBITAR
    , M_SIGUIENTE_VISTA, M_CUATRO_VISTAS, M_PROYECCION, M_EJES
    , M_AYUDA, M_SALIR
};

/**
 * Un viewport de la ventana con la cámara que se usa en él
 */
struct igvViewport
{
    int x, y, ancho, alto; ///< Rectángulo en coordenadas de OpenGL (origen abajo)
    igvCamara* camara; ///< Cámara de esa vista
    bool recuadro; ///< true si se superpone a otra vista (hay que limpiar su zona)
};

/**
 * Los objetos de esta clase encapsulan la interfaz y el estado de la aplicación
 */
class igvInterfaz
{
private:
    // Ventana
    int ancho_ventana = 0; ///< Ancho de la ventana de visualización
    int alto_ventana = 0; ///< Alto de la ventana de visualización

    igvEscena3D escena; ///< Escena que se visualiza
    igvCamara camara; ///< Cámara principal (la que mueve el usuario)
    igvCamara camaraPlanta; ///< Cámara cenital del recuadro y de las cuatro vistas
    igvCamara camaraAlzado; ///< Cámara de alzado (cuatro vistas)
    igvCamara camaraPerfil; ///< Cámara de perfil (cuatro vistas)

    bool cuatroVistas = false; ///< true: cuatro viewports; false: principal + planta
    int vistaActual = 0; ///< 0: panorámica, 1: planta, 2: alzado, 3: perfil (tecla v)

    // Modos
    modoTeclado modoTec = TECLADO_MODELOS; ///< Qué controla el teclado
    modoArticulaciones modoArt = ARTICULACIONES_TECLADO; ///< Teclado o ratón para la grúa

    // Selección con el ratón (pr2c)
    modoInterfaz modo = IGV_VISUALIZAR; ///< Visualizar o seleccionar en la siguiente pasada
    int cursorX = 0, cursorY = 0; ///< Píxel del ratón (coordenadas de GLUT)
    bool boton_retenido = false; ///< Botón izquierdo pulsado

    // Animación
    bool animar_modelo = false; ///< Animación automática de la escena
    bool orbitar_camara = false; ///< Órbita automática de la cámara
    int t_prev = 0; ///< Instante del paso anterior (ms)

    // Menús
    int menuSombreado = 0, menuInteraccion = 0, menuTeclado = 0, menuAnimacion = 0, menuVistas = 0;
    bool menus_pendientes = false; ///< Hay que actualizar los textos de los menús

    // Aplicación del patrón Singleton
    static igvInterfaz* _instancia; ///< Puntero al objeto único de la clase
    igvInterfaz() = default;

    // Métodos auxiliares
    int calcular_viewports(igvViewport vps[4]);
    void seleccionar();
    void crear_menus();
    void actualizar_menus();
    void actualizar_titulo();
    void actualizar_idle();
    void cambiar_proyeccion();
    void siguiente_vista();
    void cambiar_modo_teclado(modoTeclado m);
    void cambiar_modo_articulaciones(modoArticulaciones m);
    void mostrar_ayuda();

public:
    static igvInterfaz& getInstancia();

    ~igvInterfaz() = default;

    // Callbacks de eventos
    static void keyboardFunc(unsigned char key, int x, int y); // teclado
    static void specialFunc(int key, int x, int y); // teclas especiales (cursores)
    static void reshapeFunc(int w, int h); // cambio de tamaño de la ventana
    static void displayFunc(); // visualizar la escena
    static void mouseFunc(GLint boton, GLint estado, GLint x, GLint y); // pulsación del ratón
    static void motionFunc(GLint x, GLint y); // arrastre del ratón con el botón pulsado
    static void idleFunc(); // animación
    static void menuHandle(int valor); // opciones de menú

    // crea el mundo que se visualiza en la ventana
    void crear_mundo();

    // inicializa todos los parámetros para crear una ventana de visualización
    void configura_entorno(int argc, char** argv // parámetros del main
                           , int _ancho_ventana, int _alto_ventana // tamaño de la ventana
                           , int _pos_X, int _pos_Y // posición inicial de la ventana
                           , std::string _titulo // título de la ventana
    );

    void inicializa_callbacks(); // inicializa todos los callbacks

    void inicia_bucle_visualizacion(); // visualiza la escena y espera a eventos

    // métodos get_ y set_ de acceso a los atributos
    int get_ancho_ventana();
    int get_alto_ventana();
    void set_ancho_ventana(int _ancho_ventana);
    void set_alto_ventana(int _alto_ventana);
};

#endif   // __IGVINTERFAZ
