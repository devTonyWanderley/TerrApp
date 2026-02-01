#pragma once
#include "ponto.h"
#include "formatEstrito.h"
#include <vector>

namespace TerraIO {
class AdaptadorPontos {
public:
    static Ponto criarPonto(const std::vector<std::string>& fatias) {
        if (fatias.size() < 5) return Ponto(0,0,0);
        return Ponto(
            TerraIO::fix12ParaDouble(fatias[2]), // X
            TerraIO::fix12ParaDouble(fatias[3]), // Y
            TerraIO::fix12ParaDouble(fatias[4]), // Z
            fatias[0], fatias[1]                 // ID, Attr
            );
    }
};
}
