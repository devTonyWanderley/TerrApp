#pragma once
#include "ponto.h"

namespace TerraLaboratorio {

/**
     * @brief O Olho do Francês (InCircle Test)
     * Traduzido para "Assembly" Mental:
     * 1. Subtrai o ponto D para centralizar no zero (Estabilidade Numérica).
     * 2. Calcula o determinante da matriz levantada (Paraboloide).
     */
inline double testeInCircle(const Ponto& a, const Ponto& b, const Ponto& c, const Ponto& d) {
    // Registradores locais (Delta em relação ao novo ponto D)
    const double ax = a.x - d.x;
    const double ay = a.y - d.y;
    const double bx = b.x - d.x;
    const double by = b.y - d.y;
    const double cx = c.x - d.x;
    const double cy = c.y - d.y;

    // Cálculo dos levantamentos (z = x² + y²) no parabolóide
    const double aa = ax * ax + ay * ay;
    const double bb = bx * bx + by * by;
    const double cc = cx * cx + cy * cy;

    // O Determinante 3x3 (Fórmulas de Sarrus/Laplace otimizadas)
    return ax * (by * cc - cy * bb) -
           ay * (bx * cc - cx * bb) +
           aa * (bx * cy - cx * by);
}
}
