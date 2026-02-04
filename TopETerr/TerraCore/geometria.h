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
    static double xOrigem, yOrigem;

    // Chave primária
    uint64_t dna = 0;

    // Coordenadas Locais (Alta Precisão)
    double x, y;
    int32_t zFix;

    // A Versatilidade: Monostate (vazio), Amostra (campo) ou Traçado (projeto)
    std::variant<std::monostate, idAmostra, vNoTracado> dados;

    // Construtor: Nasce Local, gera DNA e fixa Cota
    Ponto(double xG, double yG, double zG, std::string id = "", std::string attr = "")
        : x(xG - xOrigem), y(yG - yOrigem)
    {
        // 1. Fixar a Cota
        zFix = static_cast<int32_t>(std::round(zG * 10000.0));

        // 3. Atribuir Metadados (se houver)
        if (!id.empty()) {
            dados = idAmostra{id, attr};
        }
    }

    // Métodos de conveniência
    inline double z() const { return zFix / 10000.0; }
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
