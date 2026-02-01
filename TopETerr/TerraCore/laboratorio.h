#include <immintrin.h> // Acesso aos Intrinsics (Assembly disfarçado)
#include <cstdint>
#include "ponto.h"
#include "geometria_base.h"

namespace Quadtree {
/**
     * @brief Gera o Código de Morton usando a instrução PDEP (BMI2).
     * @param x, y Coordenadas locais em décimos de milímetro (até $2^{32}$).
     * @return uint64_t O "DNA" espacial intercalado (Z-Order).
     */

// A VACINA: Força o compilador a aceitar o BMI2 nesta função
__attribute__((target("bmi2")))
inline uint64_t gerarMortonBMI2(uint32_t x, uint32_t y) {
    // Máscara de bits intercalados: 010101... (X ocupa as posições ímpares)
    const uint64_t maskX = 0x5555555555555555;
    // Máscara de bits intercalados: 101010... (Y ocupa as posições pares)
    const uint64_t maskY = 0xAAAAAAAAAAAAAAAA;

    // _pdep_u64(src, mask): Pega os bits contíguos de 'src' e os deposita
    // nas posições onde a 'mask' possui bits definidos como 1.
    uint64_t xEspalhado = _pdep_u64(static_cast<uint64_t>(x), maskX);
    uint64_t yEspalhado = _pdep_u64(static_cast<uint64_t>(y), maskY);

    // O resultado final é a união dos dois pentes de bits
    return xEspalhado | yEspalhado;
}

}
// No seu main.cpp ou laboratorio.h
struct ComparadorMorton {
    bool operator()(const Ponto& a, const Ponto& b) const {
        // 1. Convertemos as coordenadas locais (double) para unidades inteiras (0.1mm)
        // O static_cast para uint32_t é seguro até 429km
        uint32_t ax = static_cast<uint32_t>(a.x * 10000);
        uint32_t ay = static_cast<uint32_t>(a.y * 10000);
        uint32_t bx = static_cast<uint32_t>(b.x * 10000);
        uint32_t by = static_cast<uint32_t>(b.y * 10000);

        // 2. Geramos os códigos usando o nosso "Zíper de Hardware" (BMI2)
        uint64_t mortonA = Quadtree::gerarMortonBMI2(ax, ay);
        uint64_t mortonB = Quadtree::gerarMortonBMI2(bx, by);

        // 3. A regra de ouro: quem tem o menor código de Morton vem primeiro
        return mortonA < mortonB;
    }
};

void exportarParaSVG(const std::string& nome, const std::vector<Ponto>& pontos, const std::vector<Face>& faces) {
    std::ofstream arq(nome);
    arq << "<svg xmlns='http://www.w3.org' viewBox='0 0 1000 1000'>\n";

    // Trecho corrigido dentro do loop de faces no laboratorio.h
    for (const auto& f : faces) {
        arq << "  <polygon points='"
            << pontos[f.v[0]].x << "," << pontos[f.v[0]].y << " "
            << pontos[f.v[1]].x << "," << pontos[f.v[1]].y << " "
            << pontos[f.v[2]].x << "," << pontos[f.v[2]].y << "' "
            << "style='fill:none;stroke:blue;stroke-width:0.5' />\n";
    }

    arq << "</svg>";
}

namespace TerraLaboratorio {

struct SuperficieInicial {
    std::vector<Ponto> vertices;
    // Face #0: conecta os índices 0, 1 e 2
    // Usamos índices 0, 1, 2 porque estes serão os primeiros no pool
    Face faceMestra{0, 1, 2};

    // Gerador do Super-Triângulo baseado no Cercado (Bounding Box)
    static SuperficieInicial criar(double minX, double minY, double maxX, double maxY) {
        SuperficieInicial si;
        double dx = maxX - minX;
        double dy = maxY - minY;
        double dMax = (dx > dy) ? dx : dy;

        // Fator de escala 10x para garantir que o triângulo "abraça" tudo com folga
        double margem = dMax * 10.0;

        // Vértices gigantes (Geometria Protetora)
        // P1: Bem abaixo e à esquerda
        si.vertices.emplace_back(minX - margem, minY - margem, 0.0);
        // P2: Bem à direita
        si.vertices.emplace_back(maxX + margem * 2.0, minY - margem, 0.0);
        // P3: Bem acima
        si.vertices.emplace_back(minX - margem, maxY + margem * 2.0, 0.0);

        return si;
    }
};



inline void exportarParaSVG(const std::string& nome,
                            const std::vector<Ponto>& pontos,
                            const std::vector<Face>& faces,
                            double minX, double minY, double maxX, double maxY) {
    std::ofstream arq(nome);
    if (!arq.is_open()) return;

    // 1. Cabeçalho XML e Namespace
    arq << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n";

    // 2. Cálculo do Zoom: Focamos na Bounding Box dos pontos REAIS
    double margem = (maxX - minX) * 0.1; // 10% de folga
    if (margem <= 0) margem = 10.0; // Evita divisão por zero em teste vazio

    double larguraVista = (maxX - minX) + (margem * 2.0);
    double alturaVista = (maxY - minY) + (margem * 2.0);

    // O ViewBox define a "janela" do navegador. Focamos no seu levantamento.
    arq << "<svg xmlns='http://www.w3.org' width='1000' height='1000' viewBox='"
        << (minX - margem) << " " << (minY - margem) << " " << larguraVista << " " << alturaVista << "'>\n";

    // 3. Inversão de Y (Topografia: Norte para cima)
    // O translate compensa a inversão da escala para manter os pontos na tela
    arq << "  <g transform='scale(1, -1) translate(0, -" << (minY + maxY) << ")'>\n";

    // 4. Desenhar as Faces (Linhas Azuis)
    for (const auto& f : faces) {
        // Verificação de segurança para não acessar índices inexistentes
        if (f.v[0] < pontos.size() && f.v[1] < pontos.size() && f.v[2] < pontos.size()) {
            arq << "    <polygon points='"
                << pontos[f.v[0]].x << "," << pontos[f.v[0]].y << " "
                << pontos[f.v[1]].x << "," << pontos[f.v[1]].y << " "
                << pontos[f.v[2]].x << "," << pontos[f.v[2]].y << "' "
                << "style='fill:none;stroke:blue;stroke-width:" << margem * 0.05 << "' />\n";
        }
    }

    // 5. Desenhar os Pontos (Círculos Vermelhos)
    for (const auto& p : pontos) {
        // O raio (r) é proporcional à margem para ser visível em qualquer escala
        arq << "    <circle cx='" << p.x << "' cy='" << p.y
            << "' r='" << margem * 0.02 << "' fill='red' />\n";
    }

    arq << "  </g>\n</svg>";
    arq.close();
}
}
