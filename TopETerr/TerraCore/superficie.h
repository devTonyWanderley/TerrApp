#pragma once
#include "ponto.h"
#include "geometria_base.h"
#include <vector>
#include <string>
#include <optional>

class Superficie {
public:
    std::string nome;

    // Referências ao pool global de pontos (ou cópia local para isolamento)
    // Para terraplenagem, manter índices para o pool global é mais eficiente.
    std::vector<size_t> indicesPontos;
    std::vector<Aresta> arestas;
    std::vector<Face> faces;

    // Função vital para terraplenagem:
    // Interpola a cota Z em qualquer coordenada (X,Y) dentro da malha.
    std::optional<double> obterCotaZ(double x, double y) const {
        // 1. Localizar em qual Face (triângulo) o ponto (X,Y) cai.
        // 2. Calcular a interpolação linear dentro do plano do triângulo.
        return std::nullopt;
    }

    // Metadados para cálculo de volume
    double calcularAreaProjetada() const;
    double calcularArea3D() const;
};
