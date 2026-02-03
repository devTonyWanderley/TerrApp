#pragma once
#include <string>
#include <variant>
#include <array>
#include <cstddef>

namespace TerraCore {

// --- INFRAESTRUTURA ---
constexpr double EPSILON = 1e-12;
constexpr size_t VAZIO = 999999999;

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
    double estaca;
    double offset;
};

// --- O ÁTOMO: PONTO ---

struct Ponto {
    // Registradores Estáticos da Obra (O Marco Zero)
    static double xOrigem;
    static double yOrigem;

    // Coordenadas Locais (Alta Precisão)
    double x, y, z;

    // A Versatilidade: Monostate (vazio), Amostra (campo) ou Traçado (projeto)
    std::variant<std::monostate, idAmostra, vNoTracado> dados;

    // Construtor Básico
    Ponto(double xG = 0, double yG = 0, double zG = 0)
        : x(xG - xOrigem), y(yG - yOrigem), z(zG), dados(std::monostate{}) {}

    // Construtor de Levantamento (Com limpeza de strings)
    Ponto(double xG, double yG, double zG, std::string id, std::string attr)
        : x(xG - xOrigem), y(yG - yOrigem), z(zG),
        dados(idAmostra{std::move(id), std::move(attr)}) {}

    // Interface com o Mundo Externo (UTM/Global)
    inline double xGlobal() const { return x + xOrigem; }
    inline double yGlobal() const { return y + yOrigem; }
    inline double zGlobal() const { return z; }
};

// --- A CÉLULA: FACE ---

struct Face {
    std::array<size_t, 3> v; // Índices dos vértices
    std::array<size_t, 3> f; // Índices das faces vizinhas (Opção A)

    Face(size_t v0, size_t v1, size_t v2)
        : v({v0, v1, v2}), f({VAZIO, VAZIO, VAZIO}) {}

    inline bool ehFantasma() const {
        return (v[0] < 3 || v[1] < 3 || v[2] < 3);
    }
};
}
