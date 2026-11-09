#include <cmath>

#include "igvCaracola.h"

/**
 * Construye la malla de la caracola.
 *
 * Parametrización (v es el ángulo recorrido por la espiral, V = 2pi * vueltas):
 *    e(v)   = crecimiento^((v - V) / 2pi)    factor de escala; vale 1 en la boca
 *    c(v)   = ( R0 e cos v,  H0 (1 - e),  R0 e sin v )   eje del tubo (espiral)
 *    rho(v) = r0 e                                        radio del tubo
 *    P(u,v) = c(v) + rho(v) ( cos u (cos v, 0, sin v) + sin u (0, 1, 0) )
 *
 * La sección del tubo está en el plano formado por la dirección radial y la
 * vertical. El vértice de la concha (v = 0) queda arriba y la boca (v = V)
 * abajo y es la parte más ancha.
 *
 * @param R0 Radio de la espiral en la boca
 * @param r0 Radio del tubo en la boca
 * @param H0 Altura del vértice de la concha sobre la boca
 * @param crecimiento Cuánto crece el tubo en cada vuelta (p. ej. 1.8)
 * @param vueltas Número de vueltas de la espiral
 * @param divU Divisiones de la sección del tubo (dirección u)
 * @param divPorVuelta Divisiones por cada vuelta de la espiral (dirección v)
 * @post Quedan rellenos vertices, triangulos y normales
 */
igvCaracola::igvCaracola(float R0, float r0, float H0, float crecimiento
                         , int vueltas, int divU, int divPorVuelta)
{
    if (divU < 3) divU = 3;
    if (vueltas < 1) vueltas = 1;
    if (divPorVuelta < 3) divPorVuelta = 3;
    const int divV = vueltas * divPorVuelta;

    const float V = 2 * M_PI * vueltas;                  // ángulo total de la espiral
    const float k = std::log(crecimiento) / (2 * M_PI);  // e(v) = exp(k (v - V))

    // u es cerrada (la costura reutiliza los vértices i = 0); v es abierta:
    // hay divV + 1 anillos
    num_vertices = divU * (divV + 1);
    num_triangulos = 2 * divU * divV;
    vertices = new float[num_vertices * 3];
    triangulos = new unsigned int[num_triangulos * 3];

    // Vértices: el vértice (i, j) tiene el índice j * divU + i
    for (int j = 0; j <= divV; ++j)
    {
        float v = V * j / divV;
        float e = std::exp(k * (v - V));
        float cx = R0 * e * std::cos(v);   // eje del tubo
        float cy = H0 * (1 - e);
        float cz = R0 * e * std::sin(v);
        float rho = r0 * e;                // radio del tubo

        for (int i = 0; i < divU; ++i)
        {
            float u = 2 * M_PI * i / divU;
            int base = (j * divU + i) * 3;
            vertices[base + 0] = cx + rho * std::cos(u) * std::cos(v);
            vertices[base + 1] = cy + rho * std::sin(u);
            vertices[base + 2] = cz + rho * std::cos(u) * std::sin(v);
        }
    }

    // Triángulos: la celda (i, j) con esquinas a, b, c, d da dos triángulos.
    // Con esta parametrización, (a, b, d) y (b, c, d) van en sentido
    // antihorario vistos desde fuera (comprobado: la normal de cara apunta
    // hacia fuera del tubo en todos los vértices).
    int t = 0;
    for (int j = 0; j < divV; ++j)
    {
        for (int i = 0; i < divU; ++i)
        {
            unsigned int a = j * divU + i;
            unsigned int b = j * divU + (i + 1) % divU;
            unsigned int c = (j + 1) * divU + (i + 1) % divU;
            unsigned int d = (j + 1) * divU + i;

            triangulos[t++] = a;
            triangulos[t++] = b;
            triangulos[t++] = d;

            triangulos[t++] = b;
            triangulos[t++] = c;
            triangulos[t++] = d;
        }
    }

    // Normales: no hay una fórmula sencilla (el tubo se curva y crece a la vez),
    // así que se calculan a partir de los triángulos
    calcular_normales();
}
