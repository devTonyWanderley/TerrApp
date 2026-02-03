#include "superficie.h"



TerraCore::Face* superficie::localizarPonto(Ponto p, TerraCore::Face* faceInicial) {
    TerraCore::Face* atual = faceInicial;

    while (true) {
        bool pulou = false;
        for (int i = 0; i < 3; ++i) {
            // Se o ponto está à DIREITA da aresta i, o alvo está para lá
            if (orientar2d(atual->v[i], atual->v[(i+1)%3], p) < -EPSILON_GEOMETRICO) {
                atual = atual->vizinhos[i]; // SALTO!
                pulou = true;
                break; // Sai do for e continua o while no novo triângulo
            }
        }
        if (!pulou) return atual; // Se não está à direita de nenhuma, está DENTRO.
    }
}


void superficie::subdividir(size_t idFacePai, size_t idPontoNovo) {
    TerraCore::Face pai = poolFaces[idFacePai]; // A face que será "furada"

    // 1. Criamos as 3 novas faces (A, B, C)
    // Mantendo a ordem CCW: (Pai.v0, Pai.v1, P), (Pai.v1, Pai.v2, P), (Pai.v2, Pai.v0, P)
    size_t idA = poolFaces.size();
    size_t idB = idA + 1;
    size_t idC = idA + 2;

    // 2. Conectamos as entranhas (Vizinhos internos)
    // A vizinha da Face A na aresta que vai para o centro é a B e a C...

    // 3. Herança de Sangue (Vizinhos externos)
    // A Face A herda o vizinho que o Pai tinha na aresta v0-v1.
}

void superficie::splitFace(size_t idFacePai, size_t idPontoNovo) {
    // 1. Resgatamos a face que será destruída
    TerraCore::Face pai = poolFaces[idFacePai];
    size_t P = idPontoNovo;

    // 2. Criamos as três filhas
    // v0, v1, v2 são os vértices originais da face pai
    size_t v0 = pai.v[0];
    size_t v1 = pai.v[1];
    size_t v2 = pai.v[2];

    // 3. Instanciamos as novas faces no pool
    size_t idA = idFacePai; // A primeira filha REOCUPA o lugar da mãe
    size_t idB = poolFaces.size();
    size_t idC = idB + 1;

    TerraCore::Face filhaA(v0, v1, P);
    TerraCore::Face filhaB(v1, v2, P);
    TerraCore::Face filhaC(v2, v0, P);

    // 4. GESTÃO DE VIZINHOS (O segredo da Opção A)

    // Vizinhos Externos: Herdam o que a mãe tinha
    filhaA.f[0] = pai.f[0]; // Oposto ao P (lado v0-v1)
    filhaB.f[0] = pai.f[1]; // Oposto ao P (lado v1-v2)
    filhaC.f[0] = pai.f[2]; // Oposto ao P (lado v2-v0)

    // Vizinhos Internos: As irmãs se dão as mãos
    filhaA.f[1] = idC; filhaA.f[2] = idB;
    filhaB.f[1] = idA; filhaB.f[2] = idC;
    filhaC.f[1] = idB; filhaC.f[2] = idA;

    // 5. ATUALIZAÇÃO DOS VIZINHOS EXTERNOS (Avisar os vizinhos da mãe)
    // O vizinho original da mãe precisa saber que agora o vizinho dele mudou!
    atualizarVizinho(pai.f[0], idFacePai, idA);
    atualizarVizinho(pai.f[1], idFacePai, idB);
    atualizarVizinho(pai.f[2], idFacePai, idC);

    // 6. EFETIVAÇÃO
    poolFaces[idA] = filhaA;
    poolFaces.push_back(filhaB);
    poolFaces.push_back(filhaC);

    // 7. LEGALIZAÇÃO (A "Onda" de Delaunay)
    // Colocamos as 3 arestas externas na pilha para o Edge Swap
    pilhaLegalizacao.push(idA);
    pilhaLegalizacao.push(idB);
    pilhaLegalizacao.push(idC);
}


void superficie::legalizarAresta(size_t idFace, size_t idVizinho) {
    // 1. Identificamos os 4 pontos do quadrilátero
    // 2. Chamamos inCircle(A, B, C, D)
    // 3. Se resultado > 0, executamos o SWAP das diagonais
    // 4. Se houve SWAP, devemos testar recursivamente os novos vizinhos
}
