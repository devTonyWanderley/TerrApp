#pragma once
#include <cstddef>
#include "geometria_base.h"
#include <vector>
#include <variant>
#include "ponto.h"
#include "superficie.h"

// Repositório central (Pool) de geometrias para permitir o compartilhamento de segmentos.
// É o motor da topologia paramétrica, permitindo que mudanças em um eixo propaguem para bordos e lotes.

using GeoVariant = std::variant<SegmentoReta, SegmentoCurva, SegmentoEspiral>;

class BancoSegmentos {
public:
    struct Item {
        size_t id;
        GeoVariant geo;
        size_t idMestre = 0; // Para dependências (Topologia)
        double offsetRelativo = 0.0;
    };

    size_t adicionar(GeoVariant g, size_t mestre = 0, double off = 0.0) {
        size_t novoId = pool.size() + 1;
        pool.push_back({novoId, std::move(g), mestre, off});
        return novoId;
    }

    Item& obter(size_t id) { return pool.at(id - 1); }

private:
    std::vector<Item> pool;
};

/**
 * @brief Predicado InCircle (Teste de Delaunay)
 * Verifica se o ponto D está dentro do círculo circunscrito ao triângulo ABC.
 *
 * Requisito: Os pontos A, B, C devem estar em ordem ANTI-HORÁRIA (CCW).
 *
 * @return > 0 se D está DENTRO do círculo (Invalida Delaunay -> Precisa de SWAP)
 * @return < 0 se D está FORA do círculo (Delaunay OK)
 * @return = 0 se os 4 pontos são CO-CIRCULARES
 */
double inCircle(const Ponto& a, const Ponto& b, const Ponto& c, const Ponto& d) {
    // Calculamos as coordenadas de A, B e C em relação a D (D vira a nossa origem 0,0)
    // Isso simplifica o determinante 4x4 para um 3x3 e melhora a precisão numérica.
    const double adx = a.x - d.x;
    const double ady = a.y - d.y;
    const double bdx = b.x - d.x;
    const double bdy = b.y - d.y;
    const double cdx = c.x - d.x;
    const double cdy = c.y - d.y;

    // Calculamos os termos quadráticos (x² + y²) de cada vetor resultante
    // Note que esses valores podem crescer rápido, por isso nossa proteção de 50km é vital.
    const double ab_sq = adx * adx + ady * ady;
    const double bd_sq = bdx * bdx + bdy * bdy;
    const double cd_sq = cdx * cdx + cdy * cdy;

    // Calculamos o determinante da matriz 3x3:
    // | adx  ady  ab_sq |
    // | bdx  bdy  bd_sq |
    // | cdx  cdy  cd_sq |

    // Usando a Regra de Sarrus expandida para performance:
    return adx * (bdy * cd_sq - cdy * bd_sq) -
           ady * (bdx * cd_sq - cdx * bd_sq) +
           ab_sq * (bdx * cdy - cdx * bdy);
}

/**
 * @brief Predicado Orientar2D
 * Calcula a orientação do ponto C em relação ao segmento orientado AB.
 *
 * @return > 0 se C está à ESQUERDA de AB (Giro Anti-horário / CCW) - O ideal para nós.
 * @return < 0 se C está à DIREITA de AB (Giro Horário / CW).
 * @return = 0 se os pontos são COLINEARES (Um perigo para a topologia).
 */
double orientar2d(const Ponto& a, const Ponto& b, const Ponto& c) {
    // Calculamos o determinante de uma matriz 2x2 formada pelos vetores AB e AC:
    // | (bx - ax)  (by - ay) |
    // | (cx - ax)  (cy - ay) |

    // Isso equivale ao Produto Vetorial em 2D.
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

namespace TerraCore {

// A autoridade máxima da nulidade no sistema
// Baseado na nossa precisão de 0,1mm (10E-4)
// Usamos um valor que é a ordem de grandeza do erro de arredondamento
// esperado para as nossas operações de 50km de raio.
constexpr double EPSILON_GEOMETRICO = 1e-12;

/**
     * @brief Função de conveniência para testar nulidade
     * "Zero no mundo real é um intervalo" - aqui definimos esse intervalo.
     */
inline bool ehNulo(double valor) {
    return (valor > -EPSILON_GEOMETRICO) && (valor < EPSILON_GEOMETRICO);
}
}
