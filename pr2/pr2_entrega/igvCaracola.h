#ifndef __IGVCARACOLA
#define __IGVCARACOLA

#include "igvMallaTriangulos.h"

/**
 * Los objetos de esta clase representan una caracola (concha de caracol):
 * un tubo que se enrolla en una espiral logarítmica, cuyo radio crece una
 * razón constante en cada vuelta. Es una superficie paramétrica P(u, v):
 *  - u en [0, 2pi): vuelta alrededor de la sección del tubo (cerrada).
 *  - v en [0, 2pi * vueltas]: avance a lo largo de la espiral (abierta).
 *
 * Como igvCilindro en pr2a, hereda de igvMallaTriangulos y solo cambia el
 * constructor: visualizar() no se toca.
 */
class igvCaracola : public igvMallaTriangulos
{
public:
    igvCaracola(float R0, float r0, float H0, float crecimiento
                , int vueltas, int divU, int divPorVuelta);
    ~igvCaracola() override = default;
};

#endif   // __IGVCARACOLA
