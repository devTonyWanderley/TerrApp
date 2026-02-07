#include "superficie.h"

namespace TerraCore
{

superficie::superficie(const std::vector<Ponto>& pontos)
{
    // 1. Recebe o estoque (Cópia ou Movimentação do pool ordenado)
    _poolPontos = pontos;

    // 2. Logística de Memória: Estimativa de Euler para Delaunay (2n - 2 - k)
    // Reservar espaço evita que o vetor fique "pulando" de lugar na RAM
    _poolFaces.reserve(_poolPontos.size() * 2);
}   //  --superficie--

/**
 * @brief Vincula duas faces como vizinhas através de suas arestas específicas.
 * @param idA Índice da primeira face no pool.
 * @param arestaA Índice da aresta na Face A (0, 1 ou 2) oposta ao vértice v[arestaA].
 * @param idB Índice da segunda face no pool.
 * @param arestaB Índice da aresta na Face B (0, 1 ou 2) oposta ao vértice v[arestaB].
 */
void superficie::vincularVizinhos(size_t idA, int arestaA, size_t idB, int arestaB)
{
    // 1. A Face A passa a conhecer a B naquela aresta
    _poolFaces[idA].f[arestaA] = idB;

    // 2. A Face B passa a conhecer a A naquela aresta
    _poolFaces[idB].f[arestaB] = idA;

    // Nota: Esta operação é O(1). É o "clique" do encaixe.
}   //  --vincularVizinhos--

void superficie::iniciarMalha()
{
    if (_poolPontos.size() < 3) return;

    // 1. Pegamos os 3 primeiros da fila de Morton (Os vizinhos mais próximos)
    size_t i0 = 0, i1 = 1, i2 = 2;

    // 2. O AFINADOR (Garantir CCW)
    // Se orientar2d < 0, os pontos estão em sentido horário. Invertemos.
    if (orientar2d(_poolPontos[i0], _poolPontos[i1], _poolPontos[i2]) < 0) std::swap(i1, i2);

    // 3. O NASCIMENTO: Primeira Face (Vizinhança nasce VAZIO)
    _poolFaces.emplace_back(i0, i1, i2);

    // 4. A PELE: Iniciamos o Colar de Borda com esses mesmos 3 pontos
    _fronteira.colar.push_back(i0);
    _fronteira.colar.push_back(i1);
    _fronteira.colar.push_back(i2);

    // Agora o canteiro tem seu primeiro "tijolo" e sua primeira "cerca".
}   //  --iniciarMalha--

/**
 * @brief Varre o colar de borda para encontrar a melhor aresta visível.
 * @return Iterador para o início da aresta no colar, ou colar.end() se for interno.
 */
std::list<size_t>::iterator superficie::localizarHorizonte(size_t idP)
{
    const Ponto& p = _poolPontos[idP];
    auto melhorAresta = _fronteira.colar.end();
    int64_t menorDistSq = 9223372036854775807LL; // Max int64_t

    // A RONDA NO MURO
    for (auto it = _fronteira.colar.begin(); it != _fronteira.colar.end(); ++it)
    {
        // Identifica v1 e v2 (fechamento circular)
        auto nextIt = std::next(it);
        if (nextIt == _fronteira.colar.end()) nextIt = _fronteira.colar.begin();

        const Ponto& v1 = _poolPontos[*it];
        const Ponto& v2 = _poolPontos[*nextIt];

        // TESTE DO MURO: Se orientar2d < 0, P está à direita (LADO DE FORA)
        if (orientar2d(v1, v2, p) < 0)
        {

            // RANKING QUALITATIVO: Distância ao ponto médio da aresta
            // Usamos o método que criamos dentro da struct Ponto
            int64_t d2 = p.distSqParaAresta(v1, v2);

            if (d2 < menorDistSq)
            {
                menorDistSq = d2;
                melhorAresta = it;
            }
        }
    }

    // Se retornar end(), a Máquina de Estados sabe: o ponto é INTERNO (Fissão).
    return melhorAresta;
}   //  --localizarHorizonte--

/**
 * @brief Expande a malha para fora, conectando um ponto externo ao horizonte.
 * @param horizonte Iterador para a aresta mestre no colar de borda.
 * @param idP Índice do novo ponto vindo da fila de Morton.
 */
void superficie::expandirCasca(std::list<size_t>::iterator horizonte, size_t idP)
{
    // 1. Identifica os vértices da aresta que o Radar escolheu (v1 -> v2)
    auto itV1 = horizonte;
    auto itV2 = std::next(itV1) == _fronteira.colar.end() ? _fronteira.colar.begin() : std::next(itV1);

    size_t v1 = *itV1;
    size_t v2 = *itV2;

    // 2. Cria a Face Mestre (Sentido CCW: v1 -> v2 -> P)
    _poolFaces.emplace_back(v1, v2, idP);
    size_t idNovaFace = _poolFaces.size() - 1;

    // 3. O APERTO DE MÃO (Nível 1): Vincula com a face interna da obra
    // Como a aresta v1-v2 era a borda, ela pertence a exatamente uma face interna.
    vincularVizinhoInterno(idNovaFace, v1, v2);

    // 4. ATUALIZAÇÃO DO COLAR: P entra no polígono entre v1 e v2
    auto itP = _fronteira.colar.insert(itV2, idP);

    // 5. O ZÍPER LATERAL: Fecha o leque para a esquerda e para a direita
    // Se P enxerga as arestas vizinhas, ele as devora para manter a convexidade.
    fecharLequeDireita(itP);
    fecharLequeEsquerda(itP);

    // 6. A CURA (Delaunay): Valida se as novas arestas internas precisam de Flip
    legalizarArestas(idP, idNovaFace);
}   //  --expandirCasca--

void superficie::fecharLequeDireita(std::list<size_t>::iterator itP)
{
    while (true)
    {
        // ... V_p -> V_prox -> V_depois ...
        auto itV_prox = std::next(itP) == _fronteira.colar.end() ? _fronteira.colar.begin() : std::next(itP);
        auto itV_depois = std::next(itV_prox) == _fronteira.colar.end() ? _fronteira.colar.begin() : std::next(itV_prox);

        if (itV_prox == itP || itV_depois == itP) break;

        size_t id_P = *itP;
        size_t id_V_prox = *itV_prox;
        size_t id_V_depois = *itV_depois;

        // Se P enxerga a aresta (V_prox -> V_depois)
        if (orientar2d(_poolPontos[id_V_prox], _poolPontos[id_V_depois], _poolPontos[id_P]) < 0)
        {

            // 1. Cria triângulo de fechamento (P -> V_prox -> V_depois)
            _poolFaces.emplace_back(id_P, id_V_prox, id_V_depois);
            size_t idNova = _poolFaces.size() - 1;

            // 2. Vincula com a face que estava na borda (vincularInterno)
            vincularVizinhoInterno(idNova, id_V_prox, id_V_depois);

            // 3. O ponto V_prox agora está "escondido" (entra para o miolo)
            _fronteira.colar.erase(itV_prox);

            // O loop continua: P pode enxergar a próxima aresta ainda mais à direita!
        } else break;
    }
}   //  --fecharLequeDireita--

void superficie::vincularVizinhoInterno(size_t idNova, size_t v1, size_t v2)
{
    // Procuramos a face que tinha a aresta v1-v2 na borda (vizinho == VAZIO)
    // Para otimizar agora, varremos as faces recentes.
    for (int i = static_cast<int>(_poolFaces.size()) - 2; i >= 0; --i)
    {
        Face& fOld = _poolFaces[i];

        // Na face antiga, a aresta de borda é v2->v1 (invertida para CCW)
        for (int j = 0; j < 3; ++j)
        {
            if (fOld.v[j] == v2 && fOld.v[(j+1)%3] == v1)
            {
                // ENCAIXE PERFEITO!
                _poolFaces[idNova].f[0] = i; // Aresta 0 da nova olha para a antiga
                fOld.f[j] = idNova;          // Aresta j da antiga olha para a nova
                return;
            }
        }
    }
}   //  --vincularVizinhoInterno--

/**
 * @brief Caminha pela malha de vizinho em vizinho até encontrar a face que contém P.
 * @param idP Índice do ponto vindo da fila de Morton.
 * @param faceInicial Índice da face onde a busca começa (geralmente a última criada).
 * @return Índice da face que contém o ponto P.
 */
size_t superficie::localizarFaceInterna(size_t idP, size_t faceInicial)
{
    const Ponto& p = _poolPontos[idP];
    size_t idAtual = faceInicial;

    // Proteção contra loop infinito (máximo de faces no pool)
    for (size_t i = 0; i < _poolFaces.size(); ++i)
    {
        Face& f = _poolFaces[idAtual];
        bool pulou = false;

        // Testamos as 3 arestas da face atual
        for (int j = 0; j < 3; ++j)
        {
            size_t v1 = f.v[j];
            size_t v2 = f.v[(j + 1) % 3];

            // Se P está à DIREITA da aresta (v1 -> v2), ele não está nesta face.
            // Ele deve estar na face vizinha f[j].
            if (orientar2d(_poolPontos[v1], _poolPontos[v2], p) < 0)
            {
                size_t proxima = f.f[j];

                // Se bater na borda (VAZIO), o radar errou: o ponto é externo.
                if (proxima == VAZIO) return VAZIO;

                idAtual = proxima;
                pulou = true;
                break; // Pula para a próxima iteração com a nova face
            }
        }

        // Se testou as 3 arestas e não pulou, P está à ESQUERDA de todas.
        // BINGO: Achamos a face que contém o ponto!
        if (!pulou) return idAtual;
    }

    return VAZIO; // Caso de erro catastrófico ou malha degenerada
}   //  --localizarFaceInterna--

/**
 * @brief Divide uma face existente em três para acomodar um ponto interno.
 * @param idFacePai Índice da face que contém o ponto P.
 * @param idP Índice do novo ponto vindo da fila de Morton.
 */
void superficie::fissionarFace(size_t idFacePai, size_t idP)
{
    // 1. Clonamos os dados da FacePai antes de modificá-la
    Face antiga = _poolFaces[idFacePai];

    // 2. Os vértices da face original
    size_t v0 = antiga.v[0];
    size_t v1 = antiga.v[1];
    size_t v2 = antiga.v[2];

    // 3. Criamos as 2 novas faces (a 3ª será a própria FacePai reaproveitada)
    // Face A: (v1, v2, idP)
    _poolFaces.emplace_back(v1, v2, idP);
    size_t idFaceA = _poolFaces.size() - 1;

    // Face B: (v2, v0, idP)
    _poolFaces.emplace_back(v2, v0, idP);
    size_t idFaceB = _poolFaces.size() - 1;

    // 4. Reaproveitamos o índice da FacePai para a Face C: (v0, v1, idP)
    // Isso mantém os ponteiros de quem já apontava para idFacePai estáveis.
    _poolFaces[idFacePai].v[0] = v0;
    _poolFaces[idFacePai].v[1] = v1;
    _poolFaces[idFacePai].v[2] = idP;

    // 5. O APERTO DE MÃO (Vincular Vizinhos Externos)
    // As novas faces precisam herdar os vizinhos que a antiga tinha
    vincularVizinhos(idFaceA, 0, antiga.f[0], buscarArestaOposta(antiga.f[0], v1, v2));
    vincularVizinhos(idFaceB, 0, antiga.f[1], buscarArestaOposta(antiga.f[1], v2, v0));
    vincularVizinhos(idFacePai, 0, antiga.f[2], buscarArestaOposta(antiga.f[2], v0, v1));

    // 6. O APERTO DE MÃO (Vincular Vizinhos Internos entre as 3 novas)
    vincularVizinhos(idFaceA, 1, idFaceB, 2);
    vincularVizinhos(idFaceB, 1, idFacePai, 2);
    vincularVizinhos(idFacePai, 1, idFaceA, 2);

    // 7. A CURA (Legalização)
    // Testamos as 3 arestas originais da face pai para ver se precisam de Flip
    legalizarArestas(idP, idFaceA);
    legalizarArestas(idP, idFaceB);
    legalizarArestas(idP, idFacePai);
}   //  --fissionarFace--

void superficie::legalizarArestas(size_t idP, size_t idFaceAtual)
{
    // Usamos uma pilha para processar o efeito cascata dos Flips
    std::vector<size_t> pilha;
    pilha.push_back(idFaceAtual);

    while (!pilha.empty())
    {
        size_t idF = pilha.back();
        pilha.pop_back();

        Face& f = _poolFaces[idF];

        // 1. Localizamos qual vértice da face é o nosso ponto P
        int iP = (f.v[0] == idP) ? 0 : (f.v[1] == idP ? 1 : 2);

        // 2. A aresta a ser testada é a OPOSTA ao ponto P (aresta f[iP])
        size_t idVizinho = f.f[iP];

        if (idVizinho == VAZIO) continue; // Borda não sofre Flip

        Face& fv = _poolFaces[idVizinho];

        // 3. Identificamos o vértice D (o "espião" do triângulo vizinho)
        // D é o vértice que não toca a aresta compartilhada
        int iD = 0;
        if (fv.f[0] == idF) iD = 0;
        else if (fv.f[1] == idF) iD = 1;
        else iD = 2;

        size_t idD = fv.v[iD];

        // 4. O TESTE DO JUIZ (InCircle)
        // Se D está dentro do círculo de f(A,B,P) -> ILEGAL!
        if (inCircle(_poolPontos[f.v[0]], _poolPontos[f.v[1]], _poolPontos[f.v[2]], _poolPontos[idD]) > 0)
        {

            // 5. O FLIP (Giro da Aresta)
            executarFlip(idF, idVizinho, iP, iD);

            // 6. EFEITO CASCATA: As novas arestas vizinhas agora precisam ser checadas
            pilha.push_back(idF);
            pilha.push_back(idVizinho);
        }
    }
}   //  --legalizarArestas--

void superficie::executarFlip(size_t idA, size_t idB, int iP_A, int iD_B)
{
    Face& fA = _poolFaces[idA];
    Face& fB = _poolFaces[idB];

    // Vértices do quadrilátero
    size_t P = fA.v[iP_A];
    size_t D = fB.v[iD_B];
    size_t V1 = fA.v[(iP_A + 1) % 3];
    size_t V2 = fA.v[(iP_A + 2) % 3];

    // Vizinhos externos
    size_t fA1 = fA.f[(iP_A + 1) % 3];
    size_t fA2 = fA.f[(iP_A + 2) % 3];
    size_t fB1 = fB.f[(iD_B + 1) % 3];
    size_t fB2 = fB.f[(iD_B + 2) % 3];

    // REORGANIZAÇÃO CCW
    // Face A agora é (P, V1, D)
    fA.v = {P, V1, D};
    fA.f = {fB2, idB, fA2};

    // Face B agora é (P, D, V2)
    fB.v = {P, D, V2};
    fB.f = {fA1, idA, fB1};

    // ATUALIZAR VIZINHOS EXTERNOS (O aperto de mão final)
    if (fA1 != VAZIO) substituirVizinho(fA1, idA, idB);
    if (fB2 != VAZIO) substituirVizinho(fB2, idB, idA);
}   //  --executarFlip--

/**
 * @brief Informa a uma face que um de seus vizinhos mudou de índice.
 * @param idFaceAlvo A face que terá o vizinho atualizado.
 * @param idAntigo O índice do vizinho que saiu (para busca).
 * @param idNovo O índice do novo vizinho que entrou no lugar.
 */
void superficie::substituirVizinho(size_t idFaceAlvo, size_t idAntigo, size_t idNovo)
{
    Face& f = _poolFaces[idFaceAlvo];
    if (f.f[0] == idAntigo) f.f[0] = idNovo;
    else if (f.f[1] == idAntigo) f.f[1] = idNovo;
    else if (f.f[2] == idAntigo) f.f[2] = idNovo;
}   //  --substituirVizinho--

/**
 * @brief Identifica qual das 3 arestas de uma face é formada pelos vértices vA e vB.
 * @return O índice (0, 1 ou 2) da aresta oposta ao vértice que NÃO toca vA e vB.
 */
int superficie::buscarArestaOposta(size_t idFace, size_t vA, size_t vB)
{
    if (idFace == VAZIO) return -1;
    Face& f = _poolFaces[idFace];

    // A aresta f[i] é oposta ao vértice v[i].
    // Portanto, a aresta f[i] é formada pelos vértices v[(i+1)%3] e v[(i+2)%3].
    for (int i = 0; i < 3; ++i)
    {
        size_t v_prox = f.v[(i + 1) % 3];
        size_t v_depois = f.v[(i + 2) % 3];

        // Na malha CCW, se a nova face vai de vA->vB, a antiga deve ser vB->vA
        if ((v_prox == vB && v_depois == vA))
            return i;
    }
    return -1;
}   //  --buscarArestaOposta--

void superficie::fecharLequeEsquerda(std::list<size_t>::iterator itP)
{
    // No colar circular: ... V_antes_dele -> V_ant -> itP -> V_prox ...
    while (true)
    {
        // Navegação para trás no colar
        auto itV_ant = (itP == _fronteira.colar.begin()) ? std::prev(_fronteira.colar.end()) : std::prev(itP);
        auto itV_antes_dele = (itV_ant == _fronteira.colar.begin()) ? std::prev(_fronteira.colar.end()) : std::prev(itV_ant);

        if (itV_ant == itP || itV_antes_dele == itP) break;

        size_t id_P = *itP;
        size_t id_V_ant = *itV_ant;
        size_t id_V_antes = *itV_antes_dele;

        // Se o ponto P enxerga a aresta (V_antes_dele -> V_ant)
        // Lembre-se: orientar2d < 0 significa que P está à direita (lado de fora)
        if (orientar2d(_poolPontos[id_V_antes], _poolPontos[id_V_ant], _poolPontos[id_P]) < 0)
        {

            // 1. Cria triângulo de fechamento (V_antes_dele -> V_ant -> P)
            _poolFaces.emplace_back(id_V_antes, id_V_ant, id_P);
            size_t idNova = _poolFaces.size() - 1;

            // 2. Vincula com a face interna que estava naquela borda
            vincularVizinhoInterno(idNova, id_V_antes, id_V_ant);

            // 3. ELIMINAÇÃO: O ponto V_ant agora está "protegido" e sai da borda
            _fronteira.colar.erase(itV_ant);

            // Continua testando a próxima aresta à esquerda
        }
        else
        {
            break; // O leque fechou deste lado
        }
    }
}   //  --fecharLequeEsquerda--

/**
 * @brief Orquestra a inserção de um novo ponto na malha.
 * @param idPonto Índice do ponto no poolPontos (ordenado por Morton).
 */
void superficie::processarPonto(size_t idPonto) {
    // 1. O RADAR: Pergunta ao polígono da borda se o ponto está "fora"
    // Retorna o iterador para a aresta mestre ou colar.end() se for interno
    auto horizonte = localizarHorizonte(idPonto);

    if (horizonte != _fronteira.colar.end()) {
        // --- ESTADO: EXPANSÃO (Ponto Externo) ---
        // O ponto estica a "pele" da malha
        expandirCasca(horizonte, idPonto);
    }
    else {
        // --- ESTADO: FISSÃO (Ponto Interno / Salto de Morton / P800) ---
        // O ponto fura o "tecido" da malha.
        // Começamos a busca a partir da última face criada para maior velocidade.
        size_t idFacePai = localizarFaceInterna(idPonto, _poolFaces.size() - 1);

        if (idFacePai != VAZIO) {
            fissionarFace(idFacePai, idPonto);
        }
    }
}

}   //  namespace TerraCore
