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

struct Face {
    std::array<size_t, 3> v;
    // Implementar rotação cíclica no construtor futuramente
};
