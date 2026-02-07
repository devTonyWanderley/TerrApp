#pragma once
#include "geometria.h"
#include "calculo.h"
#include <vector>
#include <list>

namespace TerraCore
{

class superficie
{
public:
    // 1. O CONSTRUTOR: Recebe o pool de pontos (já ordenado por Morton)
    superficie(const std::vector<Ponto>& pontos);

    // 2. O COMANDO: Executa a triangulação incremental total
    void triangularTudo();

    // 3. A ENTREGA: Para o TerraView e MotorIO
    const std::vector<Face>& getFaces() const { return _poolFaces; }
    const std::vector<Ponto>& getPontos() const { return _poolPontos; }

    // ESTADO 0: O Big Bang (Cria o 1º triângulo real)
    void iniciarMalha();
    void processarPonto(size_t idPonto); // <-- ADICIONE ESTA LINHA

private:
    // --- ARMAZÉM ---
    std::vector<Ponto> _poolPontos;
    std::vector<Face>  _poolFaces;

    // --- A FRONTEIRA (O COLAR DE CONTAS) ---
    struct Fronteira
    {
        std::list<size_t> colar; // Índices dos pontos na borda

        // Método central: Ponto P enxerga alguma aresta?
        // Retorna o iterador para o início da aresta visível
        std::list<size_t>::iterator localizarHorizonte(const Ponto& p, const std::vector<Ponto>& pool);
    } _fronteira;   //  struct Fronteira

    // --- MÁQUINA DE ESTADOS (MÉTODOS PRIVADOS) ---

    // ESTADO 1: O Ponto é Externo (Estica a "Pele")
    void expandirCasca(std::list<size_t>::iterator horizonte, size_t idPonto);

    // ESTADO 2: O Ponto é Interno (Fura o "Tecido" - Ex: P800)
    void fissionarFace(size_t idFacePai, size_t idPonto);

    // O GPS: Caminhada de Visibilidade entre vizinhos
    size_t localizarFaceInterna(const Ponto& p, size_t faceInicial);

    // ESTADO 3: A Cura (O Ajuste de Delaunay)
    void legalizarArestas(size_t idPonto, size_t faceAfetada);

    // Utilitário de Colagem: Une duas faces pelas suas arestas
    void vincularVizinhos(size_t idA, int arestaA, size_t idB, int arestaB);

    std::list<size_t>::iterator localizarHorizonte(size_t idP);

    void fecharLequeDireita(std::list<size_t>::iterator itP);

    void fecharLequeEsquerda(std::list<size_t>::iterator itP);

    void vincularVizinhoInterno(size_t idNova, size_t v1, size_t v2);

    void executarFlip(size_t idA, size_t idB, int iP_A, int iD_B);

    void substituirVizinho(size_t idFaceAlvo, size_t idAntigo, size_t idNovo);

    // Certifique-se de que os tipos são size_t e size_t
    size_t localizarFaceInterna(size_t idP, size_t faceInicial);

    int buscarArestaOposta(size_t idFace, size_t vA, size_t vB);
};  //  Classe Superficie

} // namespace TerraCore
