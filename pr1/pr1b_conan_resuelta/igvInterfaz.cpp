#include <cstdlib>
#include <stdio.h>
#include "igvInterfaz.h"

// Aplicaci�n del patr�n Singleton
igvInterfaz* igvInterfaz::_instancia = nullptr;

// M�todos p�blicos ----------------------------------------

/**
 * M�todo para acceder al objeto �nico de la clase, en aplicaci�n del patr�n de
 * dise�o Singleton
 * @return Una referencia al objeto �nico de la clase
 */
igvInterfaz& igvInterfaz::getInstancia()
{
    if (!_instancia)
    {
        _instancia = new igvInterfaz;
    }

    return *_instancia;
}

/**
 * Crea el mundo que se visualiza en la ventana
 */
void igvInterfaz::crear_mundo()
{
    // r tiene valor por defecto (0,0,0)
    // crear c�maras
    p0 = igvPunto3D(3.0, 2.0, 4);
    r = igvPunto3D(0, 0, 0);
    V = igvPunto3D(0, 1.0, 0);

    _instancia->camara.set(IGV_PARALELA, p0, r, V, -1 * 3, 1 * 3, -1 * 3, 1 * 3, 1, 200);

    // Las c�maras se han creado con valores por defecto de 60 grados de apertura
    // y ratio de aspecto 1
    // Cámara de planta: sobre el eje Y, mirando al origen, paralela.
    // El vector arriba es (0,0,-1) porque (0,1,0) coincide con la dirección de visión.
    camaraPlanta.set(IGV_PARALELA, igvPunto3D(0, 10, 0), igvPunto3D(0, 0, 0), igvPunto3D(0, 0, -1),
                     -3, 3, -3, 3, 1, 200);

    // Cámara de alzado: sobre el eje Z
    camaraAlzado.set(IGV_PARALELA, igvPunto3D(0, 0, 10), igvPunto3D(0, 0, 0), igvPunto3D(0, 1, 0),
                     -3, 3, -3, 3, 1, 200);

    // Cámara de perfil: sobre el eje X
    camaraPerfil.set(IGV_PARALELA, igvPunto3D(10, 0, 0), igvPunto3D(0, 0, 0), igvPunto3D(0, 1, 0),
                     -3, 3, -3, 3, 1, 200);

}

/**
 * Inicializa todos los par�metros para crear una ventana de visualizaci�n
 * @param argc N�mero de par�metros por l�nea de comandos al ejecutar la
 *             aplicaci�n
 * @param argv Par�metros por l�nea de comandos al ejecutar la aplicaci�n
 * @param _ancho_ventana Ancho inicial de la ventana de visualizaci�n
 * @param _alto_ventana Alto inicial de la ventana de visualizaci�n
 * @param _pos_X Coordenada X de la posici�n inicial de la ventana de
 *               visualizaci�n
 * @param _pos_Y Coordenada Y de la posici�n inicial de la ventana de
 *               visualizaci�n
 * @param _titulo T�tulo de la ventana de visualizaci�n
 * @pre Se asume que todos los par�metros tienen valores v�lidos
 * @post Cambia el alto y ancho de ventana almacenado en el objeto
 */
void
igvInterfaz::configura_entorno(int argc, char** argv, int _ancho_ventana, int _alto_ventana, int _pos_X, int _pos_Y,
                               std::string _titulo)
{
    // inicializaci�n de los atributos de la interfaz
    ancho_ventana = _ancho_ventana;
    alto_ventana = _alto_ventana;


    // inicializaci�n de la ventana de visualizaci�n
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(_ancho_ventana, _alto_ventana);
    glutInitWindowPosition(_pos_X, _pos_Y);
    glutCreateWindow(_titulo.c_str());

    glEnable(GL_DEPTH_TEST); // activa el ocultamiento de superficies por z-buffer
    glClearColor(0.15, 0.15, 0.17, 1.0); // establece el color de fondo de la ventana

    glEnable(GL_LIGHTING); // activa la iluminacion de la escena
    glEnable(GL_NORMALIZE); // normaliza los vectores normales para calculo iluminacion

    crear_mundo(); // crea el mundo a visualizar en la ventana
}

/**
 * M�todo para visualizar la escena y esperar a eventos sobre la interfaz
 */
void igvInterfaz::inicia_bucle_visualizacion()
{
    glutMainLoop(); // inicia el bucle de visualizaci�n de GLUT
}

/**
 * M�todo para control de eventos del teclado
 * @param key C�digo de la tecla pulsada
 * @param x Coordenada X de la posici�n del cursor del rat�n en el momento del
 *          evento de teclado
 * @param y Coordenada Y de la posici�n del cursor del rat�n en el momento del
 *          evento de teclado
 * @pre Se asume que todos los par�metros tienen valores v�lidos
 * @post Los atributos de la clase pueden cambiar, dependiendo de la tecla pulsada
 */
void igvInterfaz::keyboardFunc(unsigned char key, int x, int y)
{
    /* IMPORTANTE: en la implementaci�n de este m�todo hay que cambiar convenientemente el estado
        de los objetos de la aplicaci�n, pero no hacer llamadas directas a funciones de OpenGL */

    switch (key)
    {
    case 'p': // cambia el tipo de proyección de paralela a perspectiva y viceversa
        if (_instancia->camara.getTipo() == IGV_PARALELA)
        {
            // Modo perspectiva
            _instancia->camara.set(IGV_PERSPECTIVA,
                                   _instancia->camara.getP0(),
                                   _instancia->camara.getR(),
                                   _instancia->camara.getV(),
                                   _instancia->camara.getAngulo(),
                                   _instancia->camara.getRaspecto(),
                                   _instancia->camara.getZnear(),
                                   _instancia->camara.getZfar()
            );
        }
        else
        {
            _instancia->camara.set(IGV_PARALELA,
                                   _instancia->camara.getP0(),
                                   _instancia->camara.getR(),
                                   _instancia->camara.getV(),
                                   _instancia->camara.getXwmin(),
                                   _instancia->camara.getXwmax(),
                                   _instancia->camara.getYwmin(),
                                   _instancia->camara.getYwmax(),
                                   _instancia->camara.getZnear(),
                                   _instancia->camara.getZfar()
            );
        }
        _instancia->camara.aplicar();
        break;
    case 'P': // cambia el tipo de proyección de paralela a perspectiva y viceversa
        if (_instancia->camara.getTipo() == IGV_PARALELA)
        {
            // Modo perspectiva
            _instancia->camara.set(IGV_PERSPECTIVA,
                                   _instancia->camara.getP0(),
                                   _instancia->camara.getR(),
                                   _instancia->camara.getV(),
                                   _instancia->camara.getAngulo(),
                                   _instancia->camara.getRaspecto(),
                                   _instancia->camara.getZnear(),
                                   _instancia->camara.getZfar()
            );
        }
        else
        {
            _instancia->camara.set(IGV_PARALELA,
                                   _instancia->camara.getP0(),
                                   _instancia->camara.getR(),
                                   _instancia->camara.getV(),
                                   _instancia->camara.getXwmin(),
                                   _instancia->camara.getXwmax(),
                                   _instancia->camara.getYwmin(),
                                   _instancia->camara.getYwmax(),
                                   _instancia->camara.getZnear(),
                                   _instancia->camara.getZfar()
            );
        }
        _instancia->camara.aplicar();
        break;
    case '+': // zoom in
        _instancia->camara.zoom(0.95);
        _instancia->camara.aplicar();
        break;
    case '-': // zoom out
        _instancia->camara.zoom(1.0 / 0.95);
        _instancia->camara.aplicar();
        break;
    case 'e': // activa/desactiva la visualizacion de los ejes
        _instancia->escena.set_ejes(_instancia->escena.get_ejes() ? false : true);
        break;
    case 27: // tecla de escape para SALIR
        exit(1);
        break;
    case '1':
    case '2':
    case '3':
        _instancia->escena.seleccionar(key - '1');
        break;
    case 'u': _instancia->escena.trasladar(0, 0.1, 0);
        break;
    case 'U': _instancia->escena.trasladar(0, -0.1, 0);
        break;
    case 'x': _instancia->escena.rotar(5, 0, 0);
        break;
    case 'X': _instancia->escena.rotar(-5, 0, 0);
        break;
    case 'y':
    case 'Y':
        if (_instancia->modoCamara) {
            // giro de la cámara sobre el eje Y (paneo)
            _instancia->camara.pan(key == 'y' ? 5.0 : -5.0);
            _instancia->camara.aplicar();
        } else {
            _instancia->escena.rotar(0, key == 'y' ? 5 : -5, 0);   // rota el objeto
        }
        break;
    case 'z': _instancia->escena.rotar(0, 0, 5);
        break;
    case 'Z': _instancia->escena.rotar(0, 0, -5);
        break;
    case 's': _instancia->escena.escalar(1.1);
        break;
    case 'S': _instancia->escena.escalar(1 / 1.1);
        break;
    case 'c':
    case 'C':   // alterna entre mover el objeto y mover la cámara
        _instancia->modoCamara = !_instancia->modoCamara;
        break;
    case 'f':   // plano delantero hacia delante
        _instancia->camara.moverPlanoDelantero(0.2);
        _instancia->camara.aplicar();
        break;
    case 'F':   // plano delantero hacia atrás
        _instancia->camara.moverPlanoDelantero(-0.2);
        _instancia->camara.aplicar();
        break;
    case 'b':   // plano trasero hacia atrás
        _instancia->camara.moverPlanoTrasero(0.2);
        _instancia->camara.aplicar();
        break;
    case 'B':   // plano trasero hacia delante
        _instancia->camara.moverPlanoTrasero(-0.2);
        _instancia->camara.aplicar();
        break;
    case 'n': // Aleja plano cercano
        _instancia->camara.moverPlanoDelantero(0.2);
        _instancia->camara.aplicar();
        break;
    case 'N': // Acerca plano cercano
        _instancia->camara.moverPlanoDelantero(-0.2);
        _instancia->camara.aplicar();
        break;

    case '4': // Activa/Desactiva los 4 viewports
        _instancia->cuatroVistas = !_instancia->cuatroVistas;
        break;
    case 'v':
    case 'V':
        // Cambia interactivamente entre vistas: panorámica, planta, alzado, perfil
        _instancia->vistaActual = (_instancia->vistaActual + 1) % 4;
        switch (_instancia->vistaActual) {
        case 0: // Panorámica inicial
            _instancia->camara.set(igvPunto3D(3.0, 2.0, 4.0), igvPunto3D(0,0,0), igvPunto3D(0,1,0));
            break;
        case 1: // Planta
            _instancia->camara.set(igvPunto3D(0, 10, 0), igvPunto3D(0,0,0), igvPunto3D(0,0,-1));
            break;
        case 2: // Alzado
            _instancia->camara.set(igvPunto3D(0, 0, 10), igvPunto3D(0,0,0), igvPunto3D(0,1,0));
            break;
        case 3: // Perfil
            _instancia->camara.set(igvPunto3D(10, 0, 0), igvPunto3D(0,0,0), igvPunto3D(0,1,0));
            break;
        }
        _instancia->camara.sincronizarOrbita(); // recalcula por si orbitamos después
        _instancia->camara.aplicar();
        break;
    }

    glutPostRedisplay(); // renueva el contenido de la ventana de vision y redibuja la escena
}

/**
 * M�todo que define la c�mara de visi�n y el viewport. Se llama autom�ticamente
 * cuando se cambia el tama�o de la ventana.
 * @param w Nuevo ancho de la ventana
 * @param h Nuevo alto de la ventana
 * @pre Se asume que todos los par�metros tienen valores v�lidos
 */
void igvInterfaz::reshapeFunc(int w, int h) {
    _instancia->set_ancho_ventana(w);
    _instancia->set_alto_ventana(h);
}

/**
 * M�todo para visualizar la escena
 */
void igvInterfaz::displayFunc() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int ancho = _instancia->get_ancho_ventana();
    int alto = _instancia->get_alto_ventana();

    if (!_instancia->cuatroVistas) {
        // ---- COMPORTAMIENTO ORIGINAL ----
        // Vista principal: ocupa toda la ventana
        glViewport(0, 0, ancho, alto);
        _instancia->camara.aplicar();
        _instancia->escena.visualizar();

        // Recuadro de planta: esquina superior derecha, cuadrado
        int lado = alto / 4;
        glViewport(ancho - lado, alto - lado, lado, lado);

        glEnable(GL_SCISSOR_TEST);
        glScissor(ancho - lado, alto - lado, lado, lado);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        _instancia->camaraPlanta.aplicar();
        _instancia->escena.visualizar();

    } else {
        // ---- COMPORTAMIENTO 4 VISTAS SIMULTÁNEAS ----
        int mitad_ancho = ancho / 2;
        int mitad_alto = alto / 2;

        // 1. Superior Izquierda: Planta
        glViewport(0, mitad_alto, mitad_ancho, mitad_alto);
        _instancia->camaraPlanta.aplicar();
        _instancia->escena.visualizar();

        // 2. Superior Derecha: Panorámica
        glViewport(mitad_ancho, mitad_alto, mitad_ancho, mitad_alto);
        _instancia->camara.aplicar();
        _instancia->escena.visualizar();

        // 3. Inferior Izquierda: Alzado
        glViewport(0, 0, mitad_ancho, mitad_alto);
        _instancia->camaraAlzado.aplicar();
        _instancia->escena.visualizar();

        // 4. Inferior Derecha: Perfil
        glViewport(mitad_ancho, 0, mitad_ancho, mitad_alto);
        _instancia->camaraPerfil.aplicar();
        _instancia->escena.visualizar();
    }

    glutSwapBuffers();
}

/**
 * M�todo para inicializar los callbacks GLUT
 */
void igvInterfaz::inicializa_callbacks()
{
    glutKeyboardFunc(keyboardFunc);
    glutReshapeFunc(reshapeFunc);
    glutDisplayFunc(displayFunc);
    glutSpecialFunc (specialFunc);
}

/**
 * M�todo para consultar el ancho de la ventana de visualizaci�n
 * @return El valor almacenado como ancho de la ventana de visualizaci�n
 */
int igvInterfaz::get_ancho_ventana()
{
    return ancho_ventana;
}

/**
 * M�todo para consultar el alto de la ventana de visualizaci�n
 * @return El valor almacenado como alto de la ventana de visualizaci�n
 */
int igvInterfaz::get_alto_ventana()
{
    return alto_ventana;
}

/**
 * M�todo para cambiar el ancho de la ventana de visualizaci�n
 * @param _ancho_ventana Nuevo valor para el ancho de la ventana de visualizaci�n
 * @pre Se asume que el par�metro tiene un valor v�lido
 * @post El ancho de ventana almacenado en la aplicaci�n cambia al nuevo valor
 */
void igvInterfaz::set_ancho_ventana(int _ancho_ventana)
{
    ancho_ventana = _ancho_ventana;
}

/**
 * M�todo para cambiar el alto de la ventana de visualizaci�n
 * @param _alto_ventana Nuevo valor para el alto de la ventana de visualizaci�n
 * @pre Se asume que el par�metro tiene un valor v�lido
 * @post El alto de ventana almacenado en la aplicaci�n cambia al nuevo valor
 */
void igvInterfaz::set_alto_ventana(int _alto_ventana)
{
    alto_ventana = _alto_ventana;
}



void igvInterfaz::specialFunc(int key, int x, int y)
{
    if (_instancia->modoCamara) {
        if (_instancia->modoCamara) {
            switch (key) {
            case GLUT_KEY_LEFT:  _instancia->camara.orbitar(-5, 0); break;
            case GLUT_KEY_RIGHT: _instancia->camara.orbitar(5, 0);  break;
            case GLUT_KEY_UP:    _instancia->camara.orbitar(0, 5);  break;
            case GLUT_KEY_DOWN:  _instancia->camara.orbitar(0, -5); break;
            }
        }
    } else {
        switch (key) {
        case GLUT_KEY_LEFT:  _instancia->escena.trasladar(0.1, 0, 0);  break;
        case GLUT_KEY_RIGHT: _instancia->escena.trasladar(-0.1, 0, 0); break;
        case GLUT_KEY_UP:    _instancia->escena.trasladar(0, 0, 0.1);  break;
        case GLUT_KEY_DOWN:  _instancia->escena.trasladar(0, 0, -0.1); break;
        }
    }
    glutPostRedisplay();
}
