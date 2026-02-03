#pragma once
#include <array>
#include <cstddef>

// Define as primitivas matemáticas (Reta, Curva, Espiral) e estruturas topológicas (Aresta, Face).
// É a camada de geometria pura, sem regras de negócio ou vinculação a estacas.

struct SegmentoReta {
    double x_ini, y_ini;
    double azimute_ini;
    double comprimento;
};

struct SegmentoCurva {
    double x_ini, y_ini;
    double azimute_tang_ini;
    double raio;
    double delta_angulo;
};

enum class DirecaoEspiral { Entrada, Saida };

struct SegmentoEspiral {
    double x_tangente, y_tangente;
    double azimute_tangente;
    double raio_projeto;
    double comprimento_total;
    DirecaoEspiral direcao;
};

struct Aresta {
    std::array<size_t, 2> v;
    bool isBreakline = false;

    Aresta(size_t a, size_t b, bool breakline = false) : isBreakline(breakline) {
        v[0] = (a < b) ? a : b;
        v[1] = (a < b) ? b : a;
    }
    bool operator<(const Aresta& other) const { return v < other.v; }
};

namespace TerraCore {

struct Face {
    // 1. Vértices (Os donos da geometria)
    // Guardamos os índices do pool de pontos para economizar RAM.
    // v[0], v[1], v[2] sempre em ordem ANTI-HORÁRIA (CCW).
    std::array<size_t, 3> v;

    // 2. Vizinhos (A rede de navegação)
    // f[i] é o índice da Face vizinha oposta ao vértice v[i].
    // Se f[i] == 9999999, significa que é uma borda (vácuo).
    std::array<size_t, 3> f;

    // Construtor simples
    Face(size_t v0, size_t v1, size_t v2)
        : v({v0, v1, v2}), f({9999999, 9999999, 9999999}) {}

    // Função auxiliar para saber se a face toca o Super-Triângulo
    // Supondo que os 3 primeiros pontos do pool (0, 1, 2) sejam os fantasmas.
    bool ehFantasma() const {
        return (v[0] < 3 || v[1] < 3 || v[2] < 3);
    }
};
}
