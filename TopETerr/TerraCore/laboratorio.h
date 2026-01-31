#pragma once
#include "motorArquivoFixo.h"
#include "ponto.h"
#include "formatEstrito.h" // <--- Faltava este para reconhecer o fix12ParaDouble

namespace TerraLaboratorio {
// Centraliza as pontes aqui até que a abordagem se torne "Imutável"
class AdaptadorUniversal {
public:
    // Adaptador para Ponto
    static Ponto criarPonto(const std::vector<std::string>& fatias) {
        // Índices fixos da sua instância .pdw
        return Ponto(
            TerraIO::fix12ParaDouble(fatias[2]), // X
            TerraIO::fix12ParaDouble(fatias[3]), // Y
            TerraIO::fix12ParaDouble(fatias[4]), // Z
            fatias[0], fatias[1]                // ID, Attr (limpeza automática)
            );
    }

    // Futuramente: static Aresta criarAresta(...)
};
}
