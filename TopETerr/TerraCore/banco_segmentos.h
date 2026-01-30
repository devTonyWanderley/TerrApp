#pragma once
#include <cstddef>
#include "geometria_base.h"
#include <vector>
#include <variant>

// Repositório central (Pool) de geometrias para permitir o compartilhamento de segmentos.
// É o motor da topologia paramétrica, permitindo que mudanças em um eixo propaguem para bordos e lotes.

using GeoVariant = std::variant<SegmentoReta, SegmentoCurva, SegmentoEspiral>;

class BancoSegmentos {
public:
    struct Item {
        size_t id;
        GeoVariant geo;
        size_t idMestre = 0; // Para dependências (Topologia)
        double offsetRelativo = 0.0;
    };

    size_t adicionar(GeoVariant g, size_t mestre = 0, double off = 0.0) {
        size_t novoId = pool.size() + 1;
        pool.push_back({novoId, std::move(g), mestre, off});
        return novoId;
    }

    Item& obter(size_t id) { return pool.at(id - 1); }

private:
    std::vector<Item> pool;
};
