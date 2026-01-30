#pragma once
#include "banco_segmentos.h"
#include <optional>

// Organiza a sequência lógica e o encadeamento dos segmentos através da Distância Acumulada (DA).
// Define as entidades de alto nível como Eixos (abertos) e Contornos/Regiões (fechados).

struct ElementoHorizontal {
    double DA_ini;
    double DA_fim;
    bool travaTangenciaFinal = true;
    size_t idSegmento; // Aponta para o BancoSegmentos
};

class Tracado2D {
public:
    std::string nome;
    std::vector<ElementoHorizontal> elementos;
    double DA_inicial = 0.0;

    void anexarElemento(size_t idSeg, double comprimento) {
        double inicio = elementos.empty() ? DA_inicial : elementos.back().DA_fim;
        elementos.push_back({inicio, inicio + comprimento, true, idSeg});
    }
};

struct Contorno2D {
    std::string nome;
    Tracado2D perimetro;
    bool estaFechado = true;
};
