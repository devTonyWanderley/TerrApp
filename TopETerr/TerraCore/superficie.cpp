#include "superficie.h"

namespace TerraCore
{
uint32_t Ponto::xOrigem = 0;
uint32_t Ponto::yOrigem = 0;
    superficie::superficie(std::vector<Ponto> pontosProntos, BBox limitesIniciais):
        poolPontos(std::move(pontosProntos)), indexador(poolNos, poolPontos)
    {
        Quadtree::NoQuadtree raiz;
        raiz.limites = limitesIniciais;
        poolNos.push_back(raiz);

        poolFaces.push_back(Face(0, 1, 2));
    }


    superficie::superficie(const std::vector<Ponto>& pontos) {
        // 1. Recebe os pontos (Já devem estar ordenados por Morton pelo MotorIO)
        poolPontos = pontos;

        // 2. Reserva espaço para as faces (estimativa: 2 * n)
        poolFaces.reserve(poolPontos.size() * 2);
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
        uint32_t cx = pai.limites.centroX(),
            cy = pai.limites.centroY(),
            xMin = pai.limites.xMin,
            xMax = pai.limites.xMax,
            yMin = pai.limites.yMin,
            yMax = pai.limites.yMax;
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

    /**
 * @brief Localiza a melhor aresta da fronteira para conectar o ponto P.
 * @return Iterador para o primeiro vértice da aresta no colar, ou end() se for Interno.
 */
    std::list<size_t>::iterator superficie::localizarHorizonte(const Ponto& p) {
        auto melhorAresta = _fronteira.colar.end();
        double menorDistSq = 1e18; // Inicialização com "infinito"

        // 1. A RONDA NO MURO (Percorrendo o Colar de Contas)
        for (auto it = _fronteira.colar.begin(); it != _fronteira.colar.end(); ++it) {
            // Identifica o próximo vértice (fechamento circular)
            auto nextIt = std::next(it);
            if (nextIt == _fronteira.colar.end()) nextIt = _fronteira.colar.begin();

            const Ponto& v1 = poolPontos[*it];
            const Ponto& v2 = poolPontos[*nextIt];

            // 2. FILTRO DE VISIBILIDADE: P está à direita da aresta (lado de fora)?
            // Usamos o seu orientar2d com inteiros de 64 bits.
            if (orientar2d(v1, v2, p) < 0) {

                // 3. RANKING QUALITATIVO (O "Ímã" de Proximidade)
                // Calculamos a distância ao quadrado de P ao ponto médio da aresta
                double mx = (v1.x() + v2.x()) * 0.5;
                double my = (v1.y() + v2.y()) * 0.5;

                double dx = p.x() - mx;
                double dy = p.y() - my;
                double d2 = (dx * dx) + (dy * dy);

                if (d2 < menorDistSq) {
                    menorDistSq = d2;
                    melhorAresta = it;
                }
            }
        }

        // Se melhorAresta for end(), significa que P está à esquerda de TODAS as arestas.
        // Portanto, o ponto P está DENTRO do polígono.
        return melhorAresta;
    }

    void superficie::iniciarMalha()
    {
        if (poolPontos.size() < 3) return;

        // 1. Pegamos os 3 primeiros da fila de Morton
        size_t i0 = 0, i1 = 1, i2 = 2;

        // 2. O TESTE DO DIAPASÃO: Garantir sentido Anti-Horário (CCW)
        // Se orientar2d < 0, o giro é horário. Precisamos inverter dois vértices.
        int64_t sentido = orientar2d(poolPontos[i0], poolPontos[i1], poolPontos[i2]);

        if (sentido == 0)
        {
            // PONTO DE ATENÇÃO: Colinearidade inicial.
            // Se os 3 primeiros forem uma linha reta, precisamos buscar o 4º ponto.
            // Por enquanto, vamos assumir que Morton nos deu uma vizinhança triangular.
        }

        if (sentido < 0) std::swap(i1, i2); // Inverte para garantir CCW

        // 3. CRIAÇÃO DA PRIMEIRA FACE
        Face f0(i0, i1, i2);
        poolFaces.push_back(f0);

        // 4. ATIVAÇÃO DA FRONTEIRA (O Colar)
        // A fronteira nasce com os mesmos 3 pontos, na mesma ordem CCW
        Fronteira _fronteira;
        _fronteira.iniciar(i0, i1, i2);

        // 5. REGISTRO DAS PRIMEIRAS EMBAIXADAS
        // Já carimbamos o DNA desses 3 pontos no mapa de quadrantes
    }

    void superficie::expandirCasca(std::list<size_t>::iterator horizonte, size_t idPontoNovo) {
        const Ponto& P = poolPontos[idPontoNovo];

        // 1. Identificamos os vértices da Aresta Mestre (V1 -> V2)
        auto itV1 = horizonte;
        auto itV2 = std::next(itV1) == _fronteira.colar.end() ? _fronteira.colar.begin() : std::next(itV1);

        size_t v1 = *itV1;
        size_t v2 = *itV2;

        // 2. Criamos o Triângulo Mestre (Sentido CCW: v1 -> v2 -> P)
        Face fNova(v1, v2, idPontoNovo);
        poolFaces.push_back(fNova);
        size_t idFaceMestre = poolFaces.size() - 1;

        // 3. VINCULAÇÃO (Ponto Passivo):
        // Precisamos ligar essa face à face que já existia "dentro" da aresta v1-v2
        vincularVizinhosExternos(idFaceMestre, v1, v2);

        // 4. ATUALIZAÇÃO DO COLAR: P entra entre v1 e v2
        auto itP = _fronteira.colar.insert(itV2, idPontoNovo);

        // 5. O FECHAMENTO DO LEQUE (Zíper para os lados)
        // Verificamos se P também enxerga as arestas ANTERIORES e PRÓXIMAS no colar
        fecharLequeDireita(itP);
        fecharLequeEsquerda(itP);

        // 6. LEGALIZAÇÃO (O Ajuste de Contas)
        // Disparamos o Flip para garantir que a nova "pele" seja Delaunay
        legalizarArestas(idPontoNovo);
    }
    void superficie::vincularVizinhosExternos(size_t idNovaFace, size_t v1, size_t v2) {
        // 1. Precisamos achar qual face ANTIGA compartilha a aresta v1-v2.
        // Como v1-v2 era a borda, ela pertence a exatamente UMA face interna.

        for (size_t i = 0; i < poolFaces.size() - 1; ++i) { // -1 para não testar a si mesma
            Face& fAntiga = poolFaces[i];

            // Procuramos a aresta (v2, v1) na face antiga
            // (Note a inversão: v1-v2 na nova é v2-v1 na antiga para fechar o encaixe)
            for (int j = 0; j < 3; ++j) {
                size_t va = fAntiga.v[j];
                size_t vb = fAntiga.v[(j + 1) % 3];

                if (va == v2 && vb == v1) {
                    // ENCONTRAMOS O ENCAIXE!
                    Face& fNova = poolFaces[idNovaFace];

                    // A aresta v1-v2 na Face Nova é o índice 0 (pois v[0]=v1, v[1]=v2)
                    // Então o vizinho oposto ao v[2] (que é o Ponto Novo) é a Face Antiga.
                    fNova.f[2] = i;

                    // E a Face Antiga agora tem a Face Nova como vizinha naquela aresta
                    fAntiga.f[j] = idNovaFace;

                    return; // Missão cumprida
                }
            }
        }
    }
    void superficie::fecharLequeDireita(std::list<size_t>::iterator itP) {
        // No colar circular: ... V_anterior -> itP -> V_proximo -> V_depois ...

        while (true) {
            auto itV_prox = std::next(itP) == _fronteira.colar.end() ? _fronteira.colar.begin() : std::next(itP);
            auto itV_depois = std::next(itV_prox) == _fronteira.colar.end() ? _fronteira.colar.begin() : std::next(itV_prox);

            if (itV_prox == itP || itV_depois == itP) break; // Colar muito pequeno

            size_t id_P = *itP;
            size_t id_V_prox = *itV_prox;
            size_t id_V_depois = *itV_depois;

            // TESTE DE VISIBILIDADE: P enxerga a aresta (V_proximo -> V_depois)?
            // Se orientar2d < 0, P está à direita, logo enxerga o lado de fora.
            if (orientar2d(poolPontos[id_V_prox], poolPontos[id_V_depois], poolPontos[id_P]) < 0) {

                // 1. Cria o triângulo de fechamento (P -> V_prox -> V_depois)
                Face fNova(id_P, id_V_prox, id_V_depois);
                poolFaces.push_back(fNova);
                size_t idNova = poolFaces.size() - 1;

                // 2. Vincula com a face interna que estava naquela aresta
                vincularVizinhosExternos(idNova, id_V_prox, id_V_depois);

                // 3. ELIMINAÇÃO: O ponto V_proximo agora está "escondido" dentro da malha
                _fronteira.colar.erase(itV_prox);

                // O loop continua: P pode enxergar a próxima aresta ainda mais à direita!
            } else {
                break; // O leque fechou deste lado.
            }
        }
    }

    void superficie::fecharLequeEsquerda(std::list<size_t>::iterator itP) {
        // Lógica espelhada para o sentido anterior do colar
        while (true) {
            auto itV_ant = (itP == _fronteira.colar.begin()) ? std::prev(_fronteira.colar.end()) : std::prev(itP);
            auto itV_antes_dele = (itV_ant == _fronteira.colar.begin()) ? std::prev(_fronteira.colar.end()) : std::prev(itV_ant);

            if (itV_ant == itP || itV_antes_dele == itP) break;

            size_t id_P = *itP;
            size_t id_V_ant = *itV_ant;
            size_t id_V_antes = *itV_antes_dele;

            // Se P enxerga a aresta (V_antes -> V_ant)
            if (orientar2d(poolPontos[id_V_antes], poolPontos[id_V_ant], poolPontos[id_P]) < 0) {

                Face fNova(id_V_antes, id_V_ant, id_P);
                poolFaces.push_back(fNova);
                size_t idNova = poolFaces.size() - 1;

                vincularVizinhosExternos(idNova, id_V_antes, id_V_ant);

                _fronteira.colar.erase(itV_ant);
            } else {
                break;
            }
        }
    }

    void superficie::legalizarArestas(size_t idP) {
        // 1. Criamos uma pilha com todas as faces que tocam o ponto P
        // Para simplificar agora, vamos varrer e achar as que têm o idP
        std::vector<size_t> pilha;
        for(size_t i = 0; i < poolFaces.size(); ++i) {
            if(poolFaces[i].v[0] == idP || poolFaces[i].v[1] == idP || poolFaces[i].v[2] == idP) {
                pilha.push_back(i);
            }
        }

        while(!pilha.empty()) {
            size_t idF = pilha.back();
            pilha.pop_back();

            Face& f = poolFaces[idF];

            // 2. Localizamos qual vértice da face f é o nosso ponto P (idP)
            int iP = (f.v[0] == idP) ? 0 : (f.v[1] == idP ? 1 : 2);

            // 3. A aresta que queremos testar é a OPOSTA ao ponto P
            size_t idVizinho = f.f[iP];

            if(idVizinho == VAZIO) continue; // É borda, não tem o que legalizar

            Face& fv = poolFaces[idVizinho];

            // 4. Achamos o vértice "D" (o que está do outro lado da aresta)
            int iD = 0;
            if(fv.f[0] == idF) iD = 0;
            else if(fv.f[1] == idF) iD = 1;
            else iD = 2;

            size_t idD = fv.v[iD];

            // 5. O TESTE DO JUIZ (InCircle)
            // Se D está dentro do círculo formado pelos 3 vértices de f -> ILEGAL!
            if(inCircle(poolPontos[f.v[0]], poolPontos[f.v[1]], poolPontos[f.v[2]], poolPontos[idD]) > 0) {

                // --- O FLIP (O GIRO DA ARESTA) ---
                // Aqui a geometria "estala" e se reorganiza.
                // (Para o primeiro teste, você pode deixar o Flip para depois,
                // mas a malha não será Delaunay pura).

                // TODO: Implementar a troca de índices dos vértices e vizinhos.
                // Por enquanto, apenas identificar o conflito já é um grande passo.
            }
        }
    }


}
/*
// 1. Inicia o Ranking
double menorDistSq = 1e18; // Infinito inicial
auto melhorAresta = _fronteira.colar.end();

// 2. A Ronda (Onde o Morton encontra a Geometria)
for (auto it = _fronteira.colar.begin(); it != _fronteira.colar.end(); ++it) {
    auto nextIt = std::next(it) == _fronteira.colar.end() ? _fronteira.colar.begin() : std::next(it);

    // Teste de Visibilidade (Filtro Binário)
    if (orientar2d(pool[*it], pool[*nextIt], p) < 0) {
        // Cálculo Qualitativo (O Ranking)
        double dSq = distSqParaAresta(p, pool[*it], pool[*nextIt]);
        if (dSq < menorDistSq) {
            menorDistSq = dSq;
            melhorAresta = it;
        }
    }
}

*/
