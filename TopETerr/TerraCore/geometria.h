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

struct idAmostra {
    std::string id;
    std::string attr;

    static std::string limpar(std::string s) {
        size_t last = s.find_last_not_of(' ');
        if (std::string::npos == last) return "";
        s.erase(last + 1);
        return s;
    }

    idAmostra(std::string i, std::string a)
        : id(limpar(std::move(i))), attr(limpar(std::move(a))) {}

    bool operator==(const idAmostra& outro) const {
        return id == outro.id && attr == outro.attr;
    }
};

struct vNoTracado {
    uint32_t estaca;
    int32_t offset;
};

// --- O ÁTOMO: PONTO ---

struct Ponto
{
    // Registradores Globais (Agora guardados como Inteiros Puros)
    static uint32_t xOrigem, yOrigem;

    uint64_t dna = 0;
    uint32_t xl, yl, zl;

    std::variant<std::monostate, idAmostra, vNoTracado> dados;

    // --- CONSTRUTORES DE INTEIRO ---

    // Construtor Base (Usado pelo MotorIO após converter as strings)
    Ponto(uint32_t xG, uint32_t yG, uint32_t zG): xl(xG - xOrigem), yl(yG - yOrigem), zl(zG), dados(std::monostate{})
    {
        dna = gerarMortonBMI2(xl, yl);
    }

    // Construtor com Metadados (Amostra de Campo)
    Ponto(uint32_t xG, uint32_t yG, uint32_t zG, idAmostra amostra): xl(xG - xOrigem), yl(yG - yOrigem), zl(zG), dados(std::move(amostra))
    {
        dna = gerarMortonBMI2(xl, yl);
    }

    // --- MÉTODOS DE CONVENIÊNCIA (Para o mundo externo) ---
    inline double x() const { return xl / 10000.0; }
    inline double y() const { return yl / 10000.0; }
    inline double z() const { return zl / 10000.0; }
};


// --- A CÉLULA: FACE ---

struct Face
{
    std::array<size_t, 3> v; // Vértices: v[0], v[1], v[2] (Sempre CCW)
    std::array<size_t, 3> f; // Vizinhos: f[0] oposta a v[0], etc.

    Face(size_t v0, size_t v1, size_t v2): v({v0, v1, v2}), f({VAZIO, VAZIO, VAZIO}){}

    // Agora, "ser borda" é ter pelo menos um vizinho VAZIO
    inline bool naBorda() const {
        return (f[0] == VAZIO || f[1] == VAZIO || f[2] == VAZIO);
    }
};

}
