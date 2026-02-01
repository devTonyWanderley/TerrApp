#pragma once
#include <vector>
#include <array>
#include <memory>
#include "ponto.h"

namespace Quadtree {

struct BBox {
    double minX, minY, maxX, maxY;
    inline double centroX() const { return (minX + maxX) * 0.5; }
    inline double centroY() const { return (minY + maxY) * 0.5; }
};

struct NoQuadtree {
    BBox limites;
    int nivel;

    // Se idFilhos[0] == 0, o nó é uma FOLHA.
    // Armazenamos o ID (índice no pool) dos 4 filhos.
    std::array<size_t, 4> idFilhos = {0, 0, 0, 0};

    // Armazena os índices dos pontos que pertencem a este nó
    std::vector<size_t> indicesPontos;

    NoQuadtree() : nivel(0) {}
};

class Highlander {
private:
    // O "Coração de Memória": Todos os quadrados moram aqui
    std::vector<NoQuadtree> poolNos;

    // Regras de Ouro do Projeto
    const int MAX_NIVEL = 24;  // Travagem para precisão de ~0.6mm
    const double EPSILON = 0.001; // Tolerância de 1mm para fusão de pontos

public:
    Highlander(BBox limitesIniciais) {
        poolNos.reserve(4096); // Alocação antecipada para evitar realocações
        NoQuadtree raiz;
        raiz.limites = limitesIniciais;
        raiz.nivel = 0;
        poolNos.push_back(raiz);
    }

    // Método principal de inserção
    void inserir(size_t idPonto, const std::vector<Ponto>& pontosGlobais);

private:
    void subdividir(size_t idNo);
    int calcularQuadrante(const BBox& limites, const Ponto& p);
};

} // namespace Quadtree
