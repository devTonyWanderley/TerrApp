#include "superficie.h"

namespace TerraCore
{
double Ponto::xOrigem = 0.0;
double Ponto::yOrigem = 0.0;
    superficie::superficie(std::vector<Ponto> pontosProntos, BBox limitesIniciais):
        poolPontos(std::move(pontosProntos)), indexador(poolNos, poolPontos)
    {
        Quadtree::NoQuadtree raiz;
        raiz.limites = limitesIniciais;
        poolNos.push_back(raiz);

        poolFaces.push_back(Face(0, 1, 2));
    }

    void Quadtree::Highlander::inserir(size_t idPonto)
    {
        size_t idAtual = 0;
        const Ponto& pNovo = poolPontos[idPonto];

        while(true)
        {
            NoQuadtree& no = poolNos[idAtual];

            if(no.idFilhos[0] == 0)
            {
                if(no.indicePontos.empty())
                {
                    no.indicePontos.push_back(idPonto);
                    break;
                }
                else
                {
                    int quad = no.limites.calcularQuadrante(pNovo);
                    idAtual = no.idFilhos[quad];
                }
            }
        }
    }

    void Quadtree::Highlander::subdividir(size_t idNo)
    {
        NoQuadtree& pai = poolNos[idNo];
        double cx = pai.limites.centroX(),
            cy = pai.limites.centroY(),
               xMin = pai.limites.minX,
               xMax = pai.limites.maxX,
               yMin = pai.limites.minY,
            yMax = pai.limites.maxY;
        std::array<BBox, 4> bboxes =
        {
            BBox{cx, cy, xMax, yMax},
            BBox{xMin, cy, cx, yMax},
            BBox{xMin, yMin, cx, cy},
            BBox{cx, yMin, xMax, cy}
        };

        size_t idPrimeiroFilho = poolNos.size();
        int nivelPai = pai.nivel;

        for(int i = 0; i < 4; i++)
        {
            NoQuadtree filho;
            filho.limites = bboxes[i];
            filho.nivel = nivelPai + 1;
            poolNos.push_back(filho);

            poolNos[idNo].idFilhos[i] = idPrimeiroFilho + i;
        }

        if(!poolNos[idNo].indicePontos.empty())
        {
            size_t idPontoAntigo = poolNos[idNo].indicePontos[0];
            poolNos[idNo].indicePontos.clear();
            inserir(idPontoAntigo);
        }
        //  ...
    }
}


/*
#include "superficie.h"

namespace TerraCore {

    superficie::superficie(std::vector<Ponto> pontosProntos)
        : poolPontos(std::move(pontosProntos)) {

        // Nascimento da Malha: Face #0 liga os 3 vértices fantasmas
        Face super(0, 1, 2);
        poolFaces.push_back(super);
    }


    void Quadtree::Highlander::inserir(size_t idPonto, const std::vector<Ponto>& pontos) {
        size_t idAtual = 0; // Começa sempre na Raiz
        const Ponto& pNovo = pontos[idPonto];

        while (true) {
            NoQuadtree& no = poolNos[idAtual];

            // 1. Verificamos se o nó é uma FOLHA (não tem filhos)
            if (no.idFilhos[0] == 0) {
                if (no.indicesPontos.empty()) {
                    // Canteiro vazio: o ponto toma posse
                    no.indicesPontos.push_back(idPonto);
                    break;
                } else {
                    // Canteiro ocupado: chamamos o duelo!
                    const Ponto& pAntigo = pontos[no.indicesPontos[0]];

                    // Teste de Proximidade (Usando a álgebra de vetores)
                    double distSq = std::pow(pNovo.x - pAntigo.x, 2) +
                                    std::pow(pNovo.y - pAntigo.y, 2);

                    if (distSq < (EPSILON * EPSILON)) {
                        // SÓ PODE HAVER UM! (O Highlander vence)
                        // Ignoramos o novo ponto para manter a malha limpa.
                        break;
                    } else if (no.nivel < MAX_NIVEL) {
                        // São diferentes: Subdivide para dar espaço aos dois.
                        subdividir(idAtual);
                        // Não damos break: o while(true) vai reavaliar o nó
                        // e empurrar o pNovo e o pAntigo para os novos filhos.
                    } else {
                        // Atingimos o limite do silício (0.6mm): funde por força bruta.
                        break;
                    }
                }
            } else {
                // 2. O nó já é um galpão dividido: Descemos um andar.
                int quadrante = no.limites.calcularQuadrante(pNovo);
                idAtual = no.idFilhos[quadrante];
            }
        }
    }

    void Quadtree::Highlander::subdividir(size_t idNo) {
        // ... Lógica de criar os 4 filhos e mover o ponto antigo para o quadrante certo ...
        // 1. Criamos os 4 novos filhos no estoque (poolNos)
        // Pegamos o índice onde os novos filhos começarão
        size_t idPrimeiroFilho = poolNos.size();
        NoQuadtree& pai = poolNos[idNo];

        // Calculamos os limites de cada quadrante usando a ferramenta BBox
        double cx = pai.limites.centroX();
        double cy = pai.limites.centroY();
        double minX = pai.limites.minX;
        double maxX = pai.limites.maxX;
        double minY = pai.limites.minY;
        double maxY = pai.limites.maxY;

        // 2. Definimos as 4 novas "caixas" (BBox)
        // Ordem: 0:NE, 1:NW, 2:SW, 3:SE
        std::array<BBox, 4> limitesFilhos = {
            BBox{cx, cy, maxX, maxY}, // NE
            BBox{minX, cy, cx, maxY}, // NW
            BBox{minX, minY, cx, cy}, // SW
            BBox{cx, minY, maxX, cy}  // SE
        };

        // 3. Populamos o pool com os novos nós
        for (int i = 0; i < 4; ++i) {
            NoQuadtree filho;
            filho.limites = limitesFilhos[i];
            filho.nivel = pai.nivel + 1;
            poolNos.push_back(filho);

            // O pai guarda o "contato" do filho
            pai.idFilhos[i] = idPrimeiroFilho + i;
        }

        // 4. MOVIMENTAÇÃO DE CARGA: O ponto que estava no pai agora tem que descer
        // Como o pai agora é um nó interno, ele não pode mais segurar pontos.
        if (!pai.indicesPontos.empty()) {
            size_t idPontoAntigo = pai.indicesPontos[0];
            pai.indicesPontos.clear(); // O pai agora é apenas um "guia"

            // Re-inserimos o ponto antigo para que ele ache seu novo quadrante
            // Isso garante que o ponto antigo e o novo fiquem em folhas separadas.
            inserir(idPontoAntigo, mae.poolPontos);
        }
    }

    void superficie::atualizarVizinho(size_t idVizinho, size_t idAntigo, size_t idNovo) {
        if (idVizinho == VAZIO) return;
        Face& v = poolFaces[idVizinho];
        for (int i = 0; i < 3; ++i) {
            if (v.f[i] == idAntigo) {
                v.f[i] = idNovo;
                return;
            }
        }
    }

    // Os métodos LocalizarPonto, SplitFace e LegalizarAresta
    // serão escritos aqui, um por um, com calma, nos próximos passos.
}
*/
