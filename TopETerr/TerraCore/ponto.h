#pragma once
#include <variant>
#include <string>
#include <utility>
#include <algorithm> // Para std::move e comparações

// Define a unidade atômica do sistema com suporte a polimorfismo de dados (Levantamento vs. Projeto).
// Garante a sincronia entre coordenadas cartesianas (X,Y) e o sistema de via (DA, Offset).

struct idAmostra {
    std::string id;
    std::string attr;

    // Helper estático interno para limpeza (trim)
    static std::string limpar(std::string s) {
        size_t last = s.find_last_not_of(' ');
        if (std::string::npos == last) return "";
        s.erase(last + 1);
        return s;
    }

    // Construtor "Funil": Imutabilidade e limpeza no nascimento
    idAmostra(std::string i, std::string a)
        : id(limpar(std::move(i))), attr(limpar(std::move(a))) {}

    // Operadores de Comparação (Custo zero de manutenção futura)
    bool operator==(const idAmostra& outro) const {
        return id == outro.id && attr == outro.attr;
    }

    bool operator<(const idAmostra& outro) const {
        if (id != outro.id) return id < outro.id;
        return attr < outro.attr;
    }
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
