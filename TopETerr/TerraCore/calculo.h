#pragma once
#include "geometria.h"
#include <cmath>
#include <cstdint>
#include <immintrin.h>

namespace TerraCore {

// Dentro do namespace TerraCore em calculo.h
struct BBox {
    double minX, minY, maxX, maxY;
    inline double centroX() const { return (minX + maxX) * 0.5; }
    inline double centroY() const { return (minY + maxY) * 0.5; }

    // Ferramenta cirúrgica para o Highlander
    inline int calcularQuadrante(const Ponto& p) const {
        bool norte = p.y >= centroY();
        bool leste = p.x >= centroX();
        if (norte) return leste ? 0 : 1; // NE : NW
        return leste ? 3 : 2;           // SE : SW
    }
};

/**
     * @brief Validação de Nulidade (O "Zero do Mundo Real")
     * Usa o EPSILON definido no depósito para tratar ruídos numéricos.
     */
inline bool ehNulo(double valor) {
    return (valor > -EPSILON) && (valor < EPSILON);
}

/**
     * @brief Predicado de Orientação (Sentido de Giro / Produto Vetorial 2D)
     * @return > 0: Anti-horário (Esquerda)
     * @return < 0: Horário (Direita)
     * @return 0: Colinear (Atenção: Raro, mas perigoso!)
     */
inline double orientar2d(const Ponto& a, const Ponto& b, const Ponto& c) {
    // Determinante 2x2: (Bx-Ax)(Cy-Ay) - (By-Ay)(Cx-Ax)
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

/**
     * @brief Predicado InCircle (O Juiz de Delaunay)
     * Verifica se o ponto D "invade" o círculo formado por A, B e C.
     * Requisito: A, B, C devem estar em ordem Anti-Horária (CCW).
     */
inline double inCircle(const Ponto& a, const Ponto& b, const Ponto& c, const Ponto& d) {
    // Coordenadas relativas ao ponto D para aumentar a precisão numérica
    const double adx = a.x - d.x;
    const double ady = a.y - d.y;
    const double bdx = b.x - d.x;
    const double bdy = b.y - d.y;
    const double cdx = c.x - d.x;
    const double cdy = c.y - d.y;

    // Termos quadráticos das distâncias relativas
    const double ab_sq = adx * adx + ady * ady;
    const double bd_sq = bdx * bdx + bdy * bdy;
    const double cd_sq = cdx * cdx + cdy * cdy;

    // Determinante 3x3 expandido (Regra de Sarrus)
    // Se > 0: D está dentro (Precisa de SWAP)
    return adx * (bdy * cd_sq - cdy * bd_sq) -
           ady * (bdx * cd_sq - cdx * bd_sq) +
           ab_sq * (bdx * cdy - cdx * bdy);
}

    /**
     * @brief Zíper de Morton (Intercalação de Bits via Hardware)
     * Transforma coordenadas X e Y em um único código de 64 bits.
     * Requer suporte a BMI2 no processador (-mbmi2).
     */
    inline uint64_t gerarMortonBMI2(uint32_t x, uint32_t y) {
        // Máscaras mágicas: 0x5555... seleciona bits intercalados (010101...)
        // O _pdep_u64 espalha os bits do seu número nas posições da máscara.
        uint64_t x_espalhado = _pdep_u64(static_cast<uint64_t>(x), 0x5555555555555555);
        uint64_t y_espalhado = _pdep_u64(static_cast<uint64_t>(y), 0xAAAAAAAAAAAAAAAA);

        // O "Zíper": Funde os dois em um só DNA espacial
        return x_espalhado | y_espalhado;
    }

} // namespace TerraCore
