#pragma once
#include "geometria_base.h"
#include <map>
#include <set>
#include <vector>

class MalhaTopologica {
public:
    // O que já temos
    std::vector<Face> faces;
    std::vector<Aresta> arestas;

    // A Inteligência Adicional (Para lidar com Interseções e Ilhas)
    // Mapeia uma Aresta para as Faces que a compartilham (máximo 2 em malhas "manifold")
    std::map<size_t, std::vector<size_t>> adjacenciaArestaFace;

    // Métodos de Reflexão:
    void reconstruirTopologia() {
        // Varre as faces e preenche quem é vizinho de quem via ID da Aresta
        // Se uma aresta tiver apenas 1 face, ela é Borda (limite da ilha)
        // Se tiver 2, é uma conexão interna.
        // Se tiver 3 ou mais, temos um erro topológico (interseção inválida).
    }
};
