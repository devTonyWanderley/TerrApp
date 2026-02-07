#pragma once
#include <string>
#include <variant>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cmath>

namespace TerraCore {

// --- INFRAESTRUTURA ---
constexpr double EPSILON = 1e-12;
constexpr size_t VAZIO = 999999999;
uint64_t gerarMortonBMI2(uint32_t x, uint32_t y);

// --- TIPOS DE DADOS AUXILIARES ---

struct idAmostra
{
    std::string id;
    std::string attr;

    static std::string limpar(std::string s)
    {
        size_t last = s.find_last_not_of(' ');
        if (std::string::npos == last) return "";
        s.erase(last + 1);
        return s;
    }

    idAmostra(std::string i, std::string a): id(limpar(std::move(i))), attr(limpar(std::move(a))) {}

    bool operator==(const idAmostra& outro) const
    {
        return id == outro.id && attr == outro.attr;
    }
};

struct vNoTracado
{
    //  Revisitar quando for trabalhar com traçado
    uint32_t estaca;
    int32_t offset;
};

// --- O ÁTOMO: PONTO ---

struct Ponto
{
    // Registradores Globais (Agora guardados como Inteiros Puros)
    static uint32_t xOrigem, yOrigem;

    std::variant<std::monostate, idAmostra, vNoTracado> dados;

    // --- CONSTRUTORES DE INTEIRO ---

    // Construtor Base (Usado pelo MotorIO após converter as strings)
    Ponto(uint32_t xG, uint32_t yG, uint32_t zG): _xl(xG - xOrigem), _yl(yG - yOrigem), _zl(zG), dados(std::monostate{})
    {}

    // Construtor com Metadados (Amostra de Campo)
    Ponto(uint32_t xG, uint32_t yG, uint32_t zG, idAmostra amostra): _xl(xG - xOrigem), _yl(yG - yOrigem), _zl(zG), dados(std::move(amostra))
    {}

    // Acesso rápido à unidade bruta (0.1mm) para o motor de cálculo
    inline uint32_t xl() const { return _xl; }
    inline uint32_t yl() const { return _yl; }
    inline uint32_t zl() const { return _zl; }

    // Acesso em Metros (Double) para o TerraView / painelCad
    inline double x() const { return _xl / 10000.0; }
    inline double y() const { return _yl / 10000.0; }
    inline double z() const { return _zl / 10000.0; }

    // O MÉTODO PUBLICO (A "Janela" de Vidro)
    // Note que o nome do método é 'dna' e o membro privado é '_dna'
    inline uint64_t dna() const { return _dna; }

    inline int64_t distSqParaAresta(const Ponto& v1, const Ponto& v2) const
    {
        // 1. Ponto médio da aresta em escala inteira (0.1mm)
        // Usamos int64_t para evitar qualquer estouro na soma antes da divisão
        int64_t mx = (static_cast<int64_t>(v1._xl) + v2._xl) / 2;
        int64_t my = (static_cast<int64_t>(v1._yl) + v2._yl) / 2;

        // 2. Diferença para este ponto (this)
        int64_t dx = static_cast<int64_t>(this->_xl) - mx;
        int64_t dy = static_cast<int64_t>(this->_yl) - my;

        // 3. Retorna a distância quadrada
        return (dx * dx) + (dy * dy);
    }

    // --- OS AMIGOS DA FAMÍLIA (Acesso aos membros private) ---
    friend int64_t orientar2d(const Ponto& a, const Ponto& b, const Ponto& c);
    friend double inCircle(const Ponto& a, const Ponto& b, const Ponto& c, const Ponto& d);

private:
    uint64_t _dna = 0;
    uint32_t _xl, _yl, _zl;
};


// --- A CÉLULA: FACE ---

struct Face
{
    std::array<size_t, 3> v; // Vértices: v[0], v[1], v[2] (Sempre CCW)
    std::array<size_t, 3> f; // Vizinhos: f[0] oposta a v[0], etc.

    Face(size_t v0, size_t v1, size_t v2): v({v0, v1, v2}), f({VAZIO, VAZIO, VAZIO}){}

    // Agora, "ser borda" é ter pelo menos um vizinho VAZIO
    inline bool naBorda() const
    {
        return (f[0] == VAZIO || f[1] == VAZIO || f[2] == VAZIO);
    }

    // Acha qual índice (0, 1 ou 2) contém o vértice 'idV'
    // Útil para o 'LegalizarArestas' encontrar a aresta oposta
    inline int indexDe(size_t idV) const
    {
        if (v[0] == idV) return 0;
        if (v[1] == idV) return 1;
        if (v[2] == idV) return 2;
        return -1; // Não pertence a esta face
    }

    // Retorna o vizinho oposto ao vértice que está no índice 'i'
    inline size_t vizinhoOpostoA(int i) const { return f[i]; }

};

}
