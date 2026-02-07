#pragma once
#include "geometria.h"
#include <cmath>
#include <cstdint>
#include <immintrin.h>

namespace TerraCore {
/**
     * @brief Validação de Nulidade (O "Zero do Mundo Real")
     * Usa o EPSILON definido no depósito para tratar ruídos numéricos.
     */
inline bool ehNulo(double valor)
{
    return (valor > -EPSILON) && (valor < EPSILON);
}

/**
 * @brief Predicado de Orientação (Sentido de Giro)
 * Calcula a posição relativa do ponto C em relação ao segmento orientado AB.
 *
 * @return > 0 : Giro Anti-horário (C está à ESQUERDA de AB)
 * @return < 0 : Giro Horário (C está à DIREITA de AB)
 * @return == 0: Pontos COLINEARES (Alinhados)
 */
inline int64_t orientar2d(const Ponto& a, const Ponto& b, const Ponto& c)
{
    // Determinante 2x2 usando promoção para 64 bits:
    // | (bx - ax)  (by - ay) |
    // | (cx - ax)  (cy - ay) |

    // (bx - ax) * (cy - ay) - (by - ay) * (cx - ax)


        return (static_cast<int64_t>(b._xl) - a._xl) * (static_cast<int64_t>(c._yl) - a._yl) -
               (static_cast<int64_t>(b._yl) - a._yl) * (static_cast<int64_t>(c._xl) - a._xl);
}

/**
 * @brief Predicado InCircle (Teste de Delaunay)
 * Verifica se o ponto D está dentro do círculo circunscrito ao triângulo ABC.
 *
 * Requisito: A, B, C devem estar em ordem ANTI-HORÁRIA (CCW).
 * @return > 0 se D está DENTRO (Ilegal -> Swap)
 * @return < 0 se D está FORA (Legal)
 * @return 0 se os 4 pontos são CO-CIRCULARES
 */
inline double inCircle(const Ponto& a, const Ponto& b, const Ponto& c, const Ponto& d)
{
    const double adx = static_cast<double>(a.xl()) - d.xl();
    const double ady = static_cast<double>(a.yl()) - d.yl();
    const double bdx = static_cast<double>(b.xl()) - d.xl();
    const double bdy = static_cast<double>(b.yl()) - d.yl();
    const double cdx = static_cast<double>(c.xl()) - d.xl();
    const double cdy = static_cast<double>(c.yl()) - d.yl();

    const double a_sq = adx * adx + ady * ady;
    const double b_sq = bdx * bdx + bdy * bdy;
    const double c_sq = cdx * cdx + cdy * cdy;

    return adx * (bdy * c_sq - cdy * b_sq) -
           ady * (bdx * c_sq - cdx * b_sq) +
           a_sq * (bdx * cdy - cdx * bdy);
}


/**
 * @brief Zíper de Morton (Intercalação de Bits via Hardware)
 * Transforma coordenadas X e Y em um único código de 64 bits.
 * Requer suporte a BMI2 no processador (-mbmi2).
 */
inline uint64_t gerarMortonBMI2(uint32_t x, uint32_t y) {
    uint64_t x_espalhado = _pdep_u64(static_cast<uint64_t>(x), 0x5555555555555555);
    uint64_t y_espalhado = _pdep_u64(static_cast<uint64_t>(y), 0xAAAAAAAAAAAAAAAA);
    return x_espalhado | y_espalhado;
}

} // namespace TerraCore
