#include <cmath>

#include "igvMallaTriangulos.h"

/**
 * Constructor parametrizado de una malla de triángulos sin normales
 * @param _num_vertices Número de vértices de la nueva malla
 * @param _vertices Coordenadas (X, Y, Z) de los vértices. Se copian en la malla
 * @param _num_triangulos Número de triángulos que forman la malla
 * @param _triangulos Índices (v1, v2, v3) de los vértices de cada triángulo.
 *        Se copian en la malla
 * @pre Se asume que todos los parámetros tienen valores válidos
 * @post La malla guarda copias de los arrays y calcula sus normales a partir
 *       de los triángulos
 */
igvMallaTriangulos::igvMallaTriangulos(long int _num_vertices, float* _vertices
                                       , long int _num_triangulos
                                       , unsigned int* _triangulos) :
    num_vertices(_num_vertices)
    , num_triangulos(_num_triangulos)
{
    vertices = new float[num_vertices * 3];
    for (long int i = 0; i < (num_vertices * 3); ++i)
    {
        vertices[i] = _vertices[i];
    }

    triangulos = new unsigned int[num_triangulos * 3];
    for (long int i = 0; i < (num_triangulos * 3); ++i)
    {
        triangulos[i] = _triangulos[i];
    }

    calcular_normales();
}

/**
 * Destructor: libera los arrays de la malla
 */
igvMallaTriangulos::~igvMallaTriangulos()
{
    delete[] vertices;
    vertices = nullptr;
    delete[] normales;
    normales = nullptr;
    delete[] triangulos;
    triangulos = nullptr;
}

/**
 * Calcula la normal de cada vértice a partir de los triángulos (método
 * genérico, válido para cualquier malla): se suma la normal de cada cara,
 * (b - a) x (c - a), en sus tres vértices y al final se normaliza.
 * Las caras grandes pesan más, lo que da un buen promedio.
 * @pre vertices y triangulos están rellenos. Los triángulos están en sentido
 *      antihorario vistos desde fuera, así la normal apunta hacia fuera
 * @post normales contiene una normal unitaria por vértice
 */
void igvMallaTriangulos::calcular_normales()
{
    delete[] normales;
    normales = new float[num_vertices * 3](); // inicializado a 0

    for (long int f = 0; f < num_triangulos; ++f)
    {
        unsigned int* t = &triangulos[3 * f];
        float* a = &vertices[3 * t[0]];
        float* b = &vertices[3 * t[1]];
        float* c = &vertices[3 * t[2]];

        float u[3], v[3];
        for (int k = 0; k < 3; ++k)
        {
            u[k] = b[k] - a[k];
            v[k] = c[k] - a[k];
        }

        float n[3] = {u[1] * v[2] - u[2] * v[1],
                      u[2] * v[0] - u[0] * v[2],
                      u[0] * v[1] - u[1] * v[0]};

        for (int s = 0; s < 3; ++s) // suma en sus tres vértices
        {
            for (int k = 0; k < 3; ++k)
            {
                normales[3 * t[s] + k] += n[k];
            }
        }
    }

    // normalizar (GL_NORMALIZE también lo haría al dibujar)
    for (long int i = 0; i < num_vertices; ++i)
    {
        float* n = &normales[3 * i];
        float l = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
        if (l > 1e-12f)
        {
            n[0] /= l;
            n[1] /= l;
            n[2] /= l;
        }
    }
}

/**
 * Método con las llamadas OpenGL para visualizar la malla de triángulos
 * con arrays de vértices y de normales (una sola llamada glDrawElements).
 * El sombreado es plano (GL_FLAT) o de Gouraud (GL_SMOOTH) según el atributo
 * gouraud; las normales son las mismas en los dos casos.
 */
void igvMallaTriangulos::visualizar()
{
    if (!gouraud)
    {
        glShadeModel(GL_FLAT);
    }
    else
    {
        glShadeModel(GL_SMOOTH);
    }

    glEnable(GL_NORMALIZE);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    if (usarnormales && normales != nullptr)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, normales);
    }

    glDrawElements(GL_TRIANGLES, num_triangulos * 3, GL_UNSIGNED_INT, triangulos);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glShadeModel(GL_SMOOTH); // el resto de la escena (la grúa) se dibuja siempre suavizado
}
