#pragma once
#include <variant>
#include <string>
#include <utility>
#include <algorithm>

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

struct Ponto {
    // Registradores Estáticos da Obra
    static double xOrigem;
    static double yOrigem;

    // Dados da Instância (Ocupa o mínimo de espaço: 24 bytes para coordenadas)
    double x, y, z;

    std::variant<std::monostate, idAmostra, vNoTracado> dados;

    // Construtor Padrão (Entra Global -> Nasce Local)
    Ponto(double xG, double yG, double zG)
        : x(xG - xOrigem), y(yG - yOrigem), z(zG), dados(std::monostate{}) {}

    // Construtor de Levantamento (Entra Global -> Nasce Local + Limpeza)
    Ponto(double xG, double yG, double zG, std::string id, std::string attr)
        : x(xG - xOrigem), y(yG - yOrigem), z(zG),
        dados(idAmostra{std::move(id), std::move(attr)}) {}

    // Métodos de Devolução (Recomposição para Ambiente Externo)
    inline double xGlobal() const { return x + xOrigem; }
    inline double yGlobal() const { return y + yOrigem; }
    inline double zGlobal() const { return z; }
};
