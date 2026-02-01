//#pragma once
#include "quadTree.h"
#include <cmath>

void Quadtree::Highlander::inserir(size_t idPonto, const std::vector<Ponto>& pontosGlobais) {
    size_t idAtual = 0;
    const Ponto& pNovo = pontosGlobais[idPonto];

    while (true) {
        NoQuadtree& no = poolNos[idAtual];

        if (no.idFilhos[0] == 0) { // Sou folha
            if (no.indicesPontos.empty()) {
                no.indicesPontos.push_back(idPonto);
                break;
            } else {
                // Já existe um ponto aqui (O "Antigo")
                const Ponto& pAntigo = pontosGlobais[no.indicesPontos[0]];

                // TESTE CRÍTICO: Eles são o mesmo ponto? (Highlander)
                if (std::hypot(pNovo.x - pAntigo.x, pNovo.y - pAntigo.y) < EPSILON) {
                    // SÃO O MESMO! Não subdivide.
                    // Aqui decidimos: ignora o novo ou faz merge (ex: média de Z).
                    break;
                } else {
                    // SÃO DIFERENTES! Redivide para separá-los.
                    subdividir(idAtual);
                    // O loop continua para empurrar ambos para baixo.
                }
            }
        }
        // ... lógica de descer para o quadrante ...
    }
}
