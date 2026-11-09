#include <cstdio>
#include <cstdlib>

#include "igvInterfaz.h"

int main(int argc, char** argv)
{
    // la ayuda y los mensajes se ven en la consola en cuanto se escriben
    setvbuf(stdout, nullptr, _IONBF, 0);

    // inicializa la ventana de visualización
    igvInterfaz::getInstancia().configura_entorno(argc, argv
                                                  , 800, 800 // tamaño de la ventana
                                                  , 100, 50 // posición de la ventana
                                                  , "IGV: Practica 2" // título de la ventana
    );

    // establece las funciones callbacks para la gestión de los eventos
    igvInterfaz::getInstancia().inicializa_callbacks();

    // inicia el bucle de visualización de GLUT
    igvInterfaz::getInstancia().inicia_bucle_visualizacion();

    return (0);
}
