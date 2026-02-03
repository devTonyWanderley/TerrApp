#pragma once
#include "geometria_base.h" // Onde mora a struct Face
#include <map>
#include <set>
#include <vector>
#include <stack>
#include "ponto.h"

class MalhaTopologica {
public:
    // O que já temos
    std::vector<TerraCore::Face> faces;
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

class superficie {
public:
    // O "Cérebro" da Malha
    std::vector<Ponto> poolPontos; // Seus 531 pontos + 3 fantasmas
    std::vector<TerraCore::Face> poolFaces;   // Onde a mágica acontece
    std::stack<size_t> pilhaLegalizacao; // A fila para o Edge Swap

    // Construtor enxuto:
    // 1. Recebe o pool de pontos já preparado (com os 3 fantasmas no início).
    // 2. Inicia o pool de faces com o Super-Triângulo (Face #0).
    superficie(const std::vector<Ponto>& pontosProntos) : poolPontos(pontosProntos) {

        // A Face #0 sempre liga os vértices 0, 1 e 2 (nossos fantasmas)
        TerraCore::Face faceMestra(0, 1, 2);

        // No início, os 3 vizinhos da Face Mestra são o "vazio" (9999999)
        faceMestra.f = {9999999, 9999999, 9999999};

        poolFaces.push_back(faceMestra);
    }

    // Métodos que você já implementou (mova-os para dentro da classe)
    void splitFace(size_t idFacePai, size_t idPontoNovo);
    void legalizarAresta(size_t idFace);
    size_t localizarPonto(const Ponto& p);

private:
    // Função auxiliar vital para a cirurgia de vizinhos
    void atualizarVizinho(size_t idVizinho, size_t idAntigo, size_t idNovo);
};

void superficie::atualizarVizinho(size_t idVizinho, size_t idAntigo, size_t idNovo) {
    if (idVizinho == 9999999) return; // É borda, não faz nada

    TerraCore::Face& v = poolFaces[idVizinho];
    for (int i = 0; i < 3; ++i) {
        if (v.f[i] == idAntigo) {
            v.f[i] = idNovo;
            return;
        }
    }
}
