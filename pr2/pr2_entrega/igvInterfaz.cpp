#include <algorithm>
#include <cstdio>
#include <cstdlib>

#include "igvInterfaz.h"

// Aplicación del patrón Singleton
igvInterfaz* igvInterfaz::_instancia = nullptr;

namespace
{
    // Color de fondo (cielo)
    const GLfloat FONDO[4] = {0.62f, 0.75f, 0.88f, 1.0f};

    // Cámaras: la escena ocupa unas 11 x 5 unidades; se mira a su centro
    const igvPunto3D CENTRO(0, 1.8, 0);
    const igvPunto3D P0_PANORAMICA(8, 6, 10);
    const double VENTANA = 5.5; ///< Semilado de la ventana de visión principal
    const double VENTANA_PLANTA = 7.0; ///< Semilado de la vista de planta (el suelo mide 14)

    /// true si el menú de GLUT está abierto: entonces no se pueden cambiar sus entradas
    bool menu_en_uso = false;

    void menuStatus(int estado, int, int)
    {
        menu_en_uso = (estado == GLUT_MENU_IN_USE);
    }

    const char* marca(bool activo)
    {
        return activo ? "(*) " : "( ) ";
    }
}

// Métodos públicos ----------------------------------------

/**
 * Método para acceder al objeto único de la clase (patrón Singleton)
 * @return Una referencia al objeto único de la clase
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
 * Crea el mundo que se visualiza en la ventana: las cámaras
 */
void igvInterfaz::crear_mundo()
{
    // Cámara principal: panorámica, paralela
    camara.set(IGV_PARALELA, P0_PANORAMICA, CENTRO, igvPunto3D(0, 1, 0)
               , -VENTANA, VENTANA, -VENTANA, VENTANA, 1, 200);

    // Cámara de planta: sobre el eje Y. El vector arriba es (0, 0, -1) porque
    // (0, 1, 0) coincide con la dirección de visión
    camaraPlanta.set(IGV_PARALELA, igvPunto3D(0, 15, 0), igvPunto3D(0, 0, 0), igvPunto3D(0, 0, -1)
                     , -VENTANA_PLANTA, VENTANA_PLANTA, -VENTANA_PLANTA, VENTANA_PLANTA, 1, 200);

    // Cámaras de alzado (eje Z) y perfil (eje X) para el modo de cuatro vistas
    camaraAlzado.set(IGV_PARALELA, igvPunto3D(0, CENTRO[Y], 15), CENTRO, igvPunto3D(0, 1, 0)
                     , -VENTANA, VENTANA, -VENTANA, VENTANA, 1, 200);
    camaraPerfil.set(IGV_PARALELA, igvPunto3D(15, CENTRO[Y], 0), CENTRO, igvPunto3D(0, 1, 0)
                     , -VENTANA, VENTANA, -VENTANA, VENTANA, 1, 200);
}

/**
 * Inicializa todos los parámetros para crear una ventana de visualización
 * @param argc Número de parámetros por línea de comandos
 * @param argv Parámetros por línea de comandos
 * @param _ancho_ventana Ancho inicial de la ventana de visualización
 * @param _alto_ventana Alto inicial de la ventana de visualización
 * @param _pos_X Coordenada X de la posición inicial de la ventana
 * @param _pos_Y Coordenada Y de la posición inicial de la ventana
 * @param _titulo Título de la ventana de visualización
 * @pre Se asume que todos los parámetros tienen valores válidos
 * @post Cambia el alto y ancho de ventana almacenado en el objeto
 */
void igvInterfaz::configura_entorno(int argc, char** argv, int _ancho_ventana, int _alto_ventana
                                    , int _pos_X, int _pos_Y, std::string _titulo)
{
    ancho_ventana = _ancho_ventana;
    alto_ventana = _alto_ventana;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(_ancho_ventana, _alto_ventana);
    glutInitWindowPosition(_pos_X, _pos_Y);
    glutCreateWindow(_titulo.c_str());

    glEnable(GL_DEPTH_TEST); // ocultamiento de superficies por z-buffer
    glClearColor(FONDO[0], FONDO[1], FONDO[2], FONDO[3]);

    glEnable(GL_LIGHTING); // iluminación de la escena
    glEnable(GL_NORMALIZE); // normaliza las normales (hay escalas en los modelos)
    glShadeModel(GL_SMOOTH);

    crear_mundo();
    crear_menus();
    actualizar_titulo();
    mostrar_ayuda();
}

/**
 * Método para visualizar la escena y esperar a eventos sobre la interfaz
 */
void igvInterfaz::inicia_bucle_visualizacion()
{
    glutMainLoop();
}

// ---------------------------------------------------------------------------
// Teclado
// ---------------------------------------------------------------------------

/**
 * Método para control de eventos del teclado.
 * Primero las teclas comunes a todos los modos (recorte, zoom, proyección,
 * vistas, animación...) y después las del modo de teclado activo.
 * @param key Código de la tecla pulsada
 * @param x, y Posición del ratón en el momento del evento
 * @post Cambia el estado de la aplicación; no se llama a OpenGL directamente
 */
void igvInterfaz::keyboardFunc(unsigned char key, int x, int y)
{
    igvInterfaz& in = *_instancia;
    igvGrua& grua = in.escena.getGrua();

    switch (key)
    {
    // ---- teclas comunes ----
    case 'p':
    case 'P': in.cambiar_proyeccion(); break;
    case '+': in.camara.zoom(0.95); break;
    case '-': in.camara.zoom(1.0 / 0.95); break;
    case 'f':
    case 'n': in.camara.moverPlanoDelantero(0.2); break; // plano cercano hacia delante
    case 'F':
    case 'N': in.camara.moverPlanoDelantero(-0.2); break; // plano cercano hacia atrás
    case 'b': in.camara.moverPlanoTrasero(0.2); break; // plano lejano hacia atrás
    case 'B': in.camara.moverPlanoTrasero(-0.2); break; // plano lejano hacia delante
    case 'v':
    case 'V': in.siguiente_vista(); break;
    case 'w':
    case 'W':
        in.cuatroVistas = !in.cuatroVistas;
        in.menus_pendientes = true;
        break;
    case 'e':
    case 'E':
        in.escena.set_ejes(!in.escena.get_ejes());
        in.menus_pendientes = true;
        break;
    case 'a':
    case 'A':
        in.animar_modelo = !in.animar_modelo;
        in.actualizar_idle();
        break;
    case 'o':
    case 'O':
        in.orbitar_camara = !in.orbitar_camara;
        in.actualizar_idle();
        break;
    case 'g':
    case 'G':
        in.escena.getMalla()->cambiarvis();
        in.menus_pendientes = true;
        break;
    case 'c':
    case 'C': in.cambiar_modo_teclado((modoTeclado) ((in.modoTec + 1) % 3)); break;
    case 'r':
    case 'R':
        in.cambiar_modo_articulaciones(in.modoArt == ARTICULACIONES_TECLADO
                                           ? ARTICULACIONES_RATON : ARTICULACIONES_TECLADO);
        break;
    case 'h':
    case 'H': in.mostrar_ayuda(); break;
    case 27: // ESC: salir
        exit(0);

    // ---- teclas que dependen del modo ----
    default:
        if (in.modoTec == TECLADO_MODELOS)
        {
            switch (key)
            {
            case '1':
            case '2': in.escena.seleccionar(key - '1'); break; // 1 caracola, 2 grúa
            case 'x': in.escena.rotar(5, 0, 0); break;
            case 'X': in.escena.rotar(-5, 0, 0); break;
            case 'y': in.escena.rotar(0, 5, 0); break;
            case 'Y': in.escena.rotar(0, -5, 0); break;
            case 'z': in.escena.rotar(0, 0, 5); break;
            case 'Z': in.escena.rotar(0, 0, -5); break;
            case 's': in.escena.escalar(1.1); break;
            case 'S': in.escena.escalar(1 / 1.1); break;
            case 'u': in.escena.trasladar(0, 0.1, 0); break;
            case 'U': in.escena.trasladar(0, -0.1, 0); break;
            default: break;
            }
        }
        else if (in.modoTec == TECLADO_ARTICULACIONES)
        {
            if (key >= '1' && key <= '0' + NUM_PARTES)
            {
                if (in.modoArt == ARTICULACIONES_TECLADO)
                {
                    grua.set_seleccionada(key - '0');
                }
                else
                {
                    printf("Articulaciones en modo raton: elige la parte con un clic (o pulsa r)\n");
                }
            }
        }
        else // TECLADO_CAMARA
        {
            if (key == 'y') in.camara.pan(5.0); // paneo
            if (key == 'Y') in.camara.pan(-5.0);
        }
        break;
    }

    in.actualizar_titulo();
    glutPostRedisplay();
}

/**
 * Teclas especiales: los cursores, según el modo de teclado
 * - modelos: traslación en X (izquierda/derecha) y en Z (arriba/abajo)
 * - articulaciones: grado de libertad de la parte elegida
 * - cámara: órbita (izquierda/derecha) y cabeceo (arriba/abajo)
 */
void igvInterfaz::specialFunc(int key, int x, int y)
{
    igvInterfaz& in = *_instancia;
    igvGrua& grua = in.escena.getGrua();

    switch (in.modoTec)
    {
    case TECLADO_MODELOS:
        switch (key)
        {
        case GLUT_KEY_LEFT: in.escena.trasladar(-0.1, 0, 0); break;
        case GLUT_KEY_RIGHT: in.escena.trasladar(0.1, 0, 0); break;
        case GLUT_KEY_UP: in.escena.trasladar(0, 0, -0.1); break;
        case GLUT_KEY_DOWN: in.escena.trasladar(0, 0, 0.1); break;
        default: break;
        }
        break;
    case TECLADO_ARTICULACIONES:
        if (in.modoArt == ARTICULACIONES_TECLADO)
        {
            int parte = grua.get_seleccionada();
            switch (key)
            {
            case GLUT_KEY_LEFT: grua.mover(parte, -1); break;
            case GLUT_KEY_RIGHT: grua.mover(parte, +1); break;
            case GLUT_KEY_UP: grua.mover(parte, +1); break;
            case GLUT_KEY_DOWN: grua.mover(parte, -1); break;
            default: break;
            }
        }
        break;
    case TECLADO_CAMARA:
        switch (key)
        {
        case GLUT_KEY_LEFT: in.camara.orbitar(-5, 0); break;
        case GLUT_KEY_RIGHT: in.camara.orbitar(5, 0); break;
        case GLUT_KEY_UP: in.camara.orbitar(0, 5); break;
        case GLUT_KEY_DOWN: in.camara.orbitar(0, -5); break;
        default: break;
        }
        break;
    }
    glutPostRedisplay();
}

// ---------------------------------------------------------------------------
// Ratón (selección por buffer de color, como en pr2c)
// ---------------------------------------------------------------------------

/**
 * Pulsación del ratón. Con el botón izquierdo (en modo ratón) se guarda el
 * píxel y se pasa a modo selección: la selección se hace en la siguiente
 * llamada a displayFunc. El botón derecho abre el menú.
 */
void igvInterfaz::mouseFunc(GLint boton, GLint estado, GLint x, GLint y)
{
    igvInterfaz& in = *_instancia;
    if (boton != GLUT_LEFT_BUTTON)
    {
        return;
    }

    if (in.modoArt != ARTICULACIONES_RATON)
    {
        if (estado == GLUT_DOWN)
        {
            printf("Para seleccionar con el raton: menu > Articulaciones > con raton (o tecla r)\n");
        }
        return;
    }

    if (estado == GLUT_DOWN)
    {
        in.boton_retenido = true;
        in.modo = IGV_SELECCIONAR;
        in.cursorX = x;
        in.cursorY = y;
        glutPostRedisplay();
    }
    else if (estado == GLUT_UP)
    {
        in.boton_retenido = false;
    }
}

/**
 * Arrastre con el botón pulsado: mueve la parte seleccionada según el
 * desplazamiento desde el último evento (dx a la derecha, dy hacia arriba)
 */
void igvInterfaz::motionFunc(GLint x, GLint y)
{
    igvInterfaz& in = *_instancia;
    // si la selección del clic aún no se ha hecho, se espera a la siguiente pasada
    if (!in.boton_retenido || in.modoArt != ARTICULACIONES_RATON || in.modo == IGV_SELECCIONAR)
    {
        return;
    }

    int parte = in.escena.getGrua().get_seleccionada();
    if (parte != NINGUNA)
    {
        int dx = x - in.cursorX;
        int dy = in.cursorY - y; // la y de GLUT crece hacia abajo
        in.escena.getGrua().arrastrar(parte, dx, dy);
    }
    in.cursorX = x;
    in.cursorY = y;
    glutPostRedisplay();
}

/**
 * Pasada de selección por buffer de color. Se dibuja, en el buffer trasero y
 * sin mostrarla, la vista en la que se ha hecho clic con su cámara y su
 * viewport, sin luz ni dithering y con cada parte de su color de
 * identificador; se lee el píxel del cursor y se traduce a la parte.
 */
void igvInterfaz::seleccionar()
{
    int yGL = alto_ventana - 1 - cursorY; // fila de OpenGL (origen abajo)

    igvViewport vps[4];
    int n = calcular_viewports(vps);
    int elegido = -1;
    for (int i = n - 1; i >= 0 && elegido < 0; --i) // el recuadro está encima: se mira primero
    {
        if (cursorX >= vps[i].x && cursorX < vps[i].x + vps[i].ancho
            && yGL >= vps[i].y && yGL < vps[i].y + vps[i].alto)
        {
            elegido = i;
        }
    }

    GLubyte pixel[4] = {0, 0, 0, 0};
    if (elegido >= 0)
    {
        igvViewport& v = vps[elegido];

        glDisable(GL_LIGHTING); // sin efectos que alteren el color
        glDisable(GL_DITHER);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_CULL_FACE);

        glClearColor(0, 0, 0, 1); // fondo = ninguna parte
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(v.x, v.y, v.ancho, v.alto);
        v.camara->aplicar((double) v.ancho / v.alto);
        escena.visualizar(true);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(cursorX, yGL, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

        glEnable(GL_LIGHTING);
        glEnable(GL_DITHER);
        glClearColor(FONDO[0], FONDO[1], FONDO[2], FONDO[3]);
    }

    int parte = igvGrua::parte_de_color(pixel);
    escena.getGrua().set_seleccionada(parte);
    printf("Seleccionada: %s\n", igvGrua::nombre_parte(parte));

    modo = IGV_VISUALIZAR;
    actualizar_titulo();
    glutPostRedisplay(); // pintar el resaltado sin esperar a mover el ratón
}

// ---------------------------------------------------------------------------
// Visualización
// ---------------------------------------------------------------------------

/**
 * Calcula los viewports de la ventana y la cámara de cada uno, en el orden
 * en que se dibujan:
 *  - normal: la vista principal en toda la ventana y la planta en un
 *    recuadro de la esquina superior derecha
 *  - cuatro vistas: planta, panorámica, alzado y perfil
 * @return Número de viewports
 */
int igvInterfaz::calcular_viewports(igvViewport vps[4])
{
    int w = ancho_ventana, h = alto_ventana;
    if (!cuatroVistas)
    {
        int lado = std::min(w, h) / 4;
        vps[0] = {0, 0, w, h, &camara, false};
        vps[1] = {w - lado, h - lado, lado, lado, &camaraPlanta, true};
        return 2;
    }
    int mw = w / 2, mh = h / 2;
    vps[0] = {0, mh, mw, h - mh, &camaraPlanta, false}; // superior izquierda
    vps[1] = {mw, mh, w - mw, h - mh, &camara, false}; // superior derecha
    vps[2] = {0, 0, mw, mh, &camaraAlzado, false}; // inferior izquierda
    vps[3] = {mw, 0, w - mw, mh, &camaraPerfil, false}; // inferior derecha
    return 4;
}

/**
 * Método para visualizar la escena. Si hay un clic pendiente se hace la
 * pasada de selección (sin intercambiar los buffers); si no, se dibuja cada
 * viewport con su cámara
 */
void igvInterfaz::displayFunc()
{
    igvInterfaz& in = *_instancia;

    if (in.menus_pendientes && !menu_en_uso)
    {
        in.actualizar_menus();
    }

    if (in.modo == IGV_SELECCIONAR)
    {
        in.seleccionar();
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    igvViewport vps[4];
    int n = in.calcular_viewports(vps);
    for (int i = 0; i < n; i++)
    {
        igvViewport& v = vps[i];
        if (v.ancho <= 0 || v.alto <= 0)
        {
            continue;
        }
        glViewport(v.x, v.y, v.ancho, v.alto);
        if (v.recuadro) // borra solo la zona del recuadro
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(v.x, v.y, v.ancho, v.alto);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_SCISSOR_TEST);
        }
        v.camara->aplicar((double) v.ancho / v.alto);
        in.escena.visualizar(false);
    }

    glutSwapBuffers();
}

/**
 * Método que se llama cuando cambia el tamaño de la ventana
 */
void igvInterfaz::reshapeFunc(int w, int h)
{
    _instancia->set_ancho_ventana(w);
    _instancia->set_alto_ventana(h > 0 ? h : 1);
}

// ---------------------------------------------------------------------------
// Animación
// ---------------------------------------------------------------------------

/**
 * Animación con tiempo real: dt es el tiempo transcurrido desde el paso
 * anterior, así la velocidad no depende del ordenador
 */
void igvInterfaz::idleFunc()
{
    igvInterfaz& in = *_instancia;
    int t = glutGet(GLUT_ELAPSED_TIME); // ms
    float dt = (t - in.t_prev) / 1000.0f;
    in.t_prev = t;
    if (dt > 0.1f) dt = 0.1f; // tras una pausa (p. ej. arrastrando la ventana)

    if (in.animar_modelo)
    {
        in.escena.animar(dt);
    }
    if (in.orbitar_camara)
    {
        in.camara.orbitar(20 * dt, 0); // 20 grados por segundo
    }
    glutPostRedisplay();
}

/**
 * Registra idleFunc solo si hay algo que animar: en reposo no se gasta CPU
 */
void igvInterfaz::actualizar_idle()
{
    t_prev = glutGet(GLUT_ELAPSED_TIME);
    glutIdleFunc(animar_modelo || orbitar_camara ? idleFunc : nullptr);
    menus_pendientes = true;
    actualizar_titulo();
}

// ---------------------------------------------------------------------------
// Menús
// ---------------------------------------------------------------------------

/**
 * Crea el menú (botón derecho) organizado en submenús
 */
void igvInterfaz::crear_menus()
{
    menuSombreado = glutCreateMenu(menuHandle);
    glutAddMenuEntry("Plano", M_PLANO);
    glutAddMenuEntry("Gouraud", M_GOURAUD);
    glutAddMenuEntry("Normales", M_NORMALES);

    menuInteraccion = glutCreateMenu(menuHandle);
    glutAddMenuEntry("Con teclado", M_ART_TECLADO);
    glutAddMenuEntry("Con raton", M_ART_RATON);

    menuTeclado = glutCreateMenu(menuHandle);
    glutAddMenuEntry("Transformar modelos", M_TEC_MODELOS);
    glutAddMenuEntry("Mover articulaciones", M_TEC_ARTICULACIONES);
    glutAddMenuEntry("Mover camara", M_TEC_CAMARA);

    menuAnimacion = glutCreateMenu(menuHandle);
    glutAddMenuEntry("Animar escena", M_ANIMAR);
    glutAddMenuEntry("Orbitar camara", M_ORBITAR);

    menuVistas = glutCreateMenu(menuHandle);
    glutAddMenuEntry("Siguiente vista (v)", M_SIGUIENTE_VISTA);
    glutAddMenuEntry("Cuatro vistas", M_CUATRO_VISTAS);
    glutAddMenuEntry("Proyeccion", M_PROYECCION);
    glutAddMenuEntry("Ejes", M_EJES);

    glutCreateMenu(menuHandle); // menú raíz
    glutAddSubMenu("Sombreado de la caracola", menuSombreado);
    glutAddSubMenu("Articulaciones de la grua", menuInteraccion);
    glutAddSubMenu("Modo del teclado", menuTeclado);
    glutAddSubMenu("Animacion", menuAnimacion);
    glutAddSubMenu("Vistas", menuVistas);
    glutAddMenuEntry("Ayuda (h)", M_AYUDA);
    glutAddMenuEntry("Salir", M_SALIR);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMenuStatusFunc(menuStatus);
    actualizar_menus();
}

/**
 * Cambia el texto de las opciones para que muestren su estado: (*) la opción
 * activa, y si / no en las que alternan. Las etiquetas van sin tildes
 */
void igvInterfaz::actualizar_menus()
{
    igvMallaTriangulos* m = escena.getMalla();
    std::string s;

    glutSetMenu(menuSombreado);
    s = std::string(marca(!m->getGouraud())) + "Plano";
    glutChangeToMenuEntry(1, s.c_str(), M_PLANO);
    s = std::string(marca(m->getGouraud())) + "Gouraud";
    glutChangeToMenuEntry(2, s.c_str(), M_GOURAUD);
    s = std::string("Array de normales: ") + (m->getUsarNormales() ? "si" : "no");
    glutChangeToMenuEntry(3, s.c_str(), M_NORMALES);

    glutSetMenu(menuInteraccion);
    s = std::string(marca(modoArt == ARTICULACIONES_TECLADO)) + "Con teclado (1-5 y cursores)";
    glutChangeToMenuEntry(1, s.c_str(), M_ART_TECLADO);
    s = std::string(marca(modoArt == ARTICULACIONES_RATON)) + "Con raton (clic y arrastrar)";
    glutChangeToMenuEntry(2, s.c_str(), M_ART_RATON);

    glutSetMenu(menuTeclado);
    s = std::string(marca(modoTec == TECLADO_MODELOS)) + "Transformar modelos";
    glutChangeToMenuEntry(1, s.c_str(), M_TEC_MODELOS);
    s = std::string(marca(modoTec == TECLADO_ARTICULACIONES)) + "Mover articulaciones";
    glutChangeToMenuEntry(2, s.c_str(), M_TEC_ARTICULACIONES);
    s = std::string(marca(modoTec == TECLADO_CAMARA)) + "Mover camara";
    glutChangeToMenuEntry(3, s.c_str(), M_TEC_CAMARA);

    glutSetMenu(menuAnimacion);
    s = std::string("Animar escena (a): ") + (animar_modelo ? "si" : "no");
    glutChangeToMenuEntry(1, s.c_str(), M_ANIMAR);
    s = std::string("Orbitar camara (o): ") + (orbitar_camara ? "si" : "no");
    glutChangeToMenuEntry(2, s.c_str(), M_ORBITAR);

    glutSetMenu(menuVistas);
    s = std::string("Cuatro vistas (w): ") + (cuatroVistas ? "si" : "no");
    glutChangeToMenuEntry(2, s.c_str(), M_CUATRO_VISTAS);
    s = std::string("Proyeccion (p): ") + (camara.getTipo() == IGV_PARALELA ? "paralela" : "perspectiva");
    glutChangeToMenuEntry(3, s.c_str(), M_PROYECCION);
    s = std::string("Ejes (e): ") + (escena.get_ejes() ? "si" : "no");
    glutChangeToMenuEntry(4, s.c_str(), M_EJES);

    menus_pendientes = false;
}

/**
 * Atiende las opciones de menú. Los textos de los menús no se pueden cambiar
 * mientras el menú está abierto: se actualizan en el siguiente displayFunc
 */
void igvInterfaz::menuHandle(int valor)
{
    igvInterfaz& in = *_instancia;
    switch (valor)
    {
    case M_PLANO: in.escena.getMalla()->setGouraud(false); break;
    case M_GOURAUD: in.escena.getMalla()->setGouraud(true); break;
    case M_NORMALES: in.escena.getMalla()->cambiarnormales(); break;
    case M_ART_TECLADO: in.cambiar_modo_articulaciones(ARTICULACIONES_TECLADO); break;
    case M_ART_RATON: in.cambiar_modo_articulaciones(ARTICULACIONES_RATON); break;
    case M_TEC_MODELOS: in.cambiar_modo_teclado(TECLADO_MODELOS); break;
    case M_TEC_ARTICULACIONES: in.cambiar_modo_teclado(TECLADO_ARTICULACIONES); break;
    case M_TEC_CAMARA: in.cambiar_modo_teclado(TECLADO_CAMARA); break;
    case M_ANIMAR:
        in.animar_modelo = !in.animar_modelo;
        in.actualizar_idle();
        break;
    case M_ORBITAR:
        in.orbitar_camara = !in.orbitar_camara;
        in.actualizar_idle();
        break;
    case M_SIGUIENTE_VISTA: in.siguiente_vista(); break;
    case M_CUATRO_VISTAS: in.cuatroVistas = !in.cuatroVistas; break;
    case M_PROYECCION: in.cambiar_proyeccion(); break;
    case M_EJES: in.escena.set_ejes(!in.escena.get_ejes()); break;
    case M_AYUDA: in.mostrar_ayuda(); break;
    case M_SALIR: exit(0);
    default: break;
    }
    in.menus_pendientes = true;
    in.actualizar_titulo();
    glutPostRedisplay();
}

// ---------------------------------------------------------------------------
// Cambios de estado
// ---------------------------------------------------------------------------

/**
 * Cambia la proyección entre paralela y perspectiva (Práctica 1). La cámara
 * guarda los parámetros de las dos, así que al cambiar no se pierde nada
 */
void igvInterfaz::cambiar_proyeccion()
{
    if (camara.getTipo() == IGV_PARALELA)
    {
        camara.set(IGV_PERSPECTIVA, camara.getP0(), camara.getR(), camara.getV()
                   , camara.getAngulo(), camara.getRaspecto(), camara.getZnear(), camara.getZfar());
    }
    else
    {
        camara.set(IGV_PARALELA, camara.getP0(), camara.getR(), camara.getV()
                   , camara.getXwmin(), camara.getXwmax(), camara.getYwmin(), camara.getYwmax()
                   , camara.getZnear(), camara.getZfar());
    }
    menus_pendientes = true;
}

/**
 * Recorre las vistas panorámica, planta, alzado y perfil (tecla v, P1)
 */
void igvInterfaz::siguiente_vista()
{
    vistaActual = (vistaActual + 1) % 4;
    switch (vistaActual)
    {
    case 0: camara.set(P0_PANORAMICA, CENTRO, igvPunto3D(0, 1, 0)); break; // panorámica
    case 1: camara.set(igvPunto3D(0, 15, 0), igvPunto3D(0, 0, 0), igvPunto3D(0, 0, -1)); break; // planta
    case 2: camara.set(igvPunto3D(0, CENTRO[Y], 15), CENTRO, igvPunto3D(0, 1, 0)); break; // alzado
    case 3: camara.set(igvPunto3D(15, CENTRO[Y], 0), CENTRO, igvPunto3D(0, 1, 0)); break; // perfil
    default: break;
    }
}

void igvInterfaz::cambiar_modo_teclado(modoTeclado m)
{
    modoTec = m;
    menus_pendientes = true;
    const char* nombres[] = {"transformar modelos", "mover articulaciones", "mover camara"};
    printf("Modo del teclado: %s (h para ver las teclas)\n", nombres[m]);
}

void igvInterfaz::cambiar_modo_articulaciones(modoArticulaciones m)
{
    modoArt = m;
    boton_retenido = false;
    menus_pendientes = true;
    printf("Articulaciones de la grua: %s\n", m == ARTICULACIONES_TECLADO
                                                ? "teclado (modo del teclado 'mover articulaciones', 1-5 y cursores)"
                                                : "raton (clic en una parte y arrastrar)");
}

/**
 * El estado de la interfaz, siempre a la vista en el título de la ventana
 */
void igvInterfaz::actualizar_titulo()
{
    const char* teclado[] = {"modelos", "articulaciones", "camara"};
    std::string t = "IGV P2 | Teclado: ";
    t += teclado[modoTec];
    if (modoTec == TECLADO_MODELOS)
    {
        t += escena.get_seleccionado() == MODELO_MALLA ? " (1: caracola)" : " (2: grua)";
    }
    t += " | Grua con ";
    t += modoArt == ARTICULACIONES_TECLADO ? "teclado" : "raton";
    t += ", parte: ";
    t += igvGrua::nombre_parte(escena.getGrua().get_seleccionada());
    if (animar_modelo) t += " | animando";
    if (orbitar_camara) t += " | orbitando";
    glutSetWindowTitle(t.c_str());
}

/**
 * Muestra en la consola las teclas, el ratón y el menú
 */
void igvInterfaz::mostrar_ayuda()
{
    printf("\n================ IGV Practica 2: caracola y grua torre ================\n"
           "Boton derecho: menu (sombreado, articulaciones teclado/raton, modo del\n"
           "teclado, animacion, vistas). El estado se ve en el titulo de la ventana.\n"
           "\n"
           "Teclas comunes a todos los modos\n"
           "  c        cambiar el modo del teclado: modelos > articulaciones > camara\n"
           "  r        articulaciones de la grua con teclado / con raton\n"
           "  a        animacion de la escena si/no      o   orbita de la camara si/no\n"
           "  g        sombreado plano / Gouraud de la caracola\n"
           "  p        proyeccion paralela / perspectiva +/- zoom\n"
           "  f F n N  plano cercano                     b B plano lejano\n"
           "  v        vista: panoramica, planta, alzado, perfil\n"
           "  w        cuatro vistas si/no               e   ejes si/no\n"
           "  h        esta ayuda                        ESC salir\n"
           "\n"
           "Modo 'transformar modelos' (Practica 1, sobre el modelo completo)\n"
           "  1 / 2    elegir la caracola / la grua\n"
           "  cursores traslacion en X (izq/der) y en Z (arr/abj)   u U  traslacion en Y\n"
           "  x X y Y z Z  rotaciones                               s S  escalado\n"
           "\n"
           "Modo 'mover articulaciones' (con la opcion de teclado)\n"
           "  1 base (avance)  2 pluma (giro)  3 carro  4 cable  5 gancho (giro)\n"
           "  cursores: mover la parte elegida (en el cable, arriba sube el gancho)\n"
           "\n"
           "Modo 'mover camara'\n"
           "  cursores: orbita (izq/der) y cabeceo (arr/abj)   y Y  paneo\n"
           "\n"
           "Con la opcion de raton: clic izquierdo en una parte de la grua para\n"
           "elegirla (se pone amarilla) y arrastrar: izquierda-derecha mueve base,\n"
           "pluma, carro y gira el gancho; arriba-abajo sube y baja cable y gancho.\n"
           "======================================================================\n\n");
    fflush(stdout);
}

/**
 * Método para inicializar los callbacks GLUT. idleFunc se registra solo
 * cuando hay animación (actualizar_idle)
 */
void igvInterfaz::inicializa_callbacks()
{
    glutKeyboardFunc(keyboardFunc);
    glutSpecialFunc(specialFunc);
    glutReshapeFunc(reshapeFunc);
    glutDisplayFunc(displayFunc);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);
}

int igvInterfaz::get_ancho_ventana()
{
    return ancho_ventana;
}

int igvInterfaz::get_alto_ventana()
{
    return alto_ventana;
}

void igvInterfaz::set_ancho_ventana(int _ancho_ventana)
{
    ancho_ventana = _ancho_ventana;
}

void igvInterfaz::set_alto_ventana(int _alto_ventana)
{
    alto_ventana = _alto_ventana;
}
