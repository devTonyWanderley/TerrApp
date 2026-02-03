#pragma once
#include "calculo.h"
#include <vector>

namespace TerraCore
{
    namespace Quadtree
    {
        struct NoQuadtree
        {
            BBox limites;
            int nivel = 0;
            std::array<size_t, 4> idFilhos = {0, 0, 0, 0};
            std::vector<size_t> indicePontos;
        };
        class Highlander
        {
        public:
            std::vector<NoQuadtree>& poolNos;
            const std::vector<Ponto>& poolPontos;

            Highlander(std::vector<NoQuadtree>& nos, const std::vector<Ponto>& pontos): poolNos(nos), poolPontos(pontos) {}

            void inserir(size_t idPonto);
            void subdividir(size_t idNo);
        };
    }

    class superficie
    {
    public:
        std::vector<Ponto> poolPontos;
        std::vector<Face> poolFaces;
        std::vector<Quadtree::NoQuadtree> poolNos;
        Quadtree::Highlander indexador;
        superficie(std::vector<Ponto> pontosProntos, BBox limitesIniciais);
        void processarTudo();
    };
}


/*
#pragma once
#include "calculo.h"  // Já traz geometria.h e as ferramentas
#include <stack>
#include <vector>

namespace TerraCore {

// --- FERRAMENTA AUXILIAR ---
namespace Quadtree {
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
}

class superficie {
public:
    // O Estoque de Materiais
    std::vector<Ponto> poolPontos;
    std::vector<Face> poolFaces;

    // O Highlander agora guarda uma referência para a superfície "mãe"
    struct Highlander {
        superficie& mae; // O binóculo para enxergar o poolPontos

        Highlander(superficie& s) : mae(s) {}

        void inserir(size_t idPonto);
        void subdividir(size_t idNo);
    };

    // Instância do Highlander que vive na superfície
    Highlander indexador;

    // A Ferramenta de Ajuste Fino
    std::stack<size_t> pilhaLegalizacao;

    // Construtor: Inicia a obra com os pontos e o Super-Triângulo
    superficie(std::vector<Ponto> pontosProntos);

    // Ações de Construção (As assinaturas)
    size_t localizarPonto(const Ponto& p);
    void splitFace(size_t idFacePai, size_t idPontoNovo);
    void legalizarAresta(size_t idFaceCentral, size_t idFaceVizinha);

private:
    // Manutenção de rede: Atualiza quem é vizinho de quem
    void atualizarVizinho(size_t idVizinho, size_t idAntigo, size_t idNovo);
};

} // namespace TerraCore
*/
