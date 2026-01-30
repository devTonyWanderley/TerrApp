#pragma once
#include <variant>
#include <string>
#include <utility>

// Define a unidade atômica do sistema com suporte a polimorfismo de dados (Levantamento vs. Projeto).
// Garante a sincronia entre coordenadas cartesianas (X,Y) e o sistema de via (DA, Offset).

struct idAmostra {
    std::string id;
    std::string attr;
};

struct vNoTracado {
    double estaca; // Internamente tratada como Distância Acumulada
    double offset;
};

struct Ponto {
    double x, y, z;
    std::variant<std::monostate, idAmostra, vNoTracado> dados;

    Ponto(double x, double y, double z)
        : x(x), y(y), z(z), dados(std::monostate{}) {}

    Ponto(double x, double y, double z, std::string id, std::string attr)
        : x(x), y(y), z(z), dados(idAmostra{std::move(id), std::move(attr)}) {}

    Ponto(double x, double y, double z, double estaca, double offset)
        : x(x), y(y), z(z), dados(vNoTracado{estaca, offset}) {}

    void atualizarPeloEixo(double da, double off, double nx, double ny) {
        if (auto* proj = std::get_if<vNoTracado>(&dados)) {
            proj->estaca = da;
            proj->offset = off;
        } else if (std::holds_alternative<std::monostate>(dados)) {
            dados = vNoTracado{da, off};
        }
        x = nx; y = ny;
    }
};
