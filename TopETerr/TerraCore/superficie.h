#pragma once
#include "calculo.h"
#include <vector>
#include <list>

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
    class Fronteira
    {
    public:
        // O Colar: Lista sequencial de índices de pontos que formam a borda
        // Ex: {10, 5, 8, 20} -> Arestas: 10-5, 5-8, 8-20, 20-10
        std::list<size_t> colar;

        // --- AÇÕES DO MESTRE DE OBRAS ---

        // 1. O Big Bang: Inicia o colar com os 3 primeiros pontos (CCW)
        void iniciar(size_t a, size_t b, size_t c)
        {
            colar.push_back(a);
            colar.push_back(b);
            colar.push_back(c);
            // Atualiza embaixadas iniciais aqui...
        }

        // 3. A Costura: Insere P no colar e remove os vértices que ficaram "escondidos"
        void atualizar(std::list<size_t>::iterator horizonte, size_t idPontoNovo)
        {
            // Insere P após o primeiro vértice do horizonte
            auto novoIt = colar.insert(std::next(horizonte), idPontoNovo);

            // Aqui executamos o "Fechamento de Cova" (Eliminação)
            // para manter a Casca Convexa e a ordem numérica estável.
            limparConcavidades(novoIt);
        }

    private:

        void limparConcavidades(std::list<size_t>::iterator novoPonto)
        {
            // Lógica de "Zigue-Zague" para trás e para frente
            // removendo pontos onde o ângulo interno ficou > 180 graus.
        }
    };

    class superficie
    {
    public:
        std::vector<Ponto> poolPontos;
        std::vector<Face> poolFaces;
        std::vector<Quadtree::NoQuadtree> poolNos;
        Quadtree::Highlander indexador;
        superficie(std::vector<Ponto> pontosProntos, BBox limitesIniciais);
        // Construtor que aceita o pool de pontos vindo do MotorIO
        superficie(const std::vector<Ponto>& pontos);
        void processarTudo();
        void iniciarMalha();
        std::list<size_t>::iterator localizarHorizonte(const Ponto& p);
        void expandirCasca(std::list<size_t>::iterator horizonte, size_t idPontoNovo);
        void vincularVizinhosExternos(size_t idNovaFace, size_t v1, size_t v2);
    private:
        Fronteira _fronteira;
        void fecharLequeDireita(std::list<size_t>::iterator itP);
        void fecharLequeEsquerda(std::list<size_t>::iterator itP);
        void legalizarArestas(size_t idPontoNovo);
    };
}
