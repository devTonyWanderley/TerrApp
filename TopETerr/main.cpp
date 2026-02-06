
#include <QApplication>
#include "leitor.h"      // TerraIO
#include "superficie.h"  // TerraCore
#include "painelCad.h"   // TerraView
#include <algorithm>

int main() {
    // 1. CARGA: O MotorIO lê o arquivo e define o Marco Zero
    auto entrega = TerraIO::MotorIO::processar("C:/Tony/Soft/soft2026/Instâncias/Pontos.pdw");

    // 2. GALPÃO: Criamos a superfície e entregamos o pool de pontos
    TerraCore::superficie malha(entrega.pontos);

    // 3. O NASCIMENTO: Iniciamos os primeiros 3 pontos (O Triângulo Semente)
    malha.iniciarMalha();

    // 4. A EVOLUÇÃO: O loop que percorre do 4º ao 531º ponto
    for (size_t i = 3; i < entrega.pontos.size(); ++i) {
        malha.processarPonto(i);
    }

    // 5. A LENTE: Passamos o resultado para o seu painelCad
    visualizador.setDados(malha.getPontos(), malha.getFaces());

    return app.exec();
}

/*
#include <QApplication>
#include "ponto.h"
#include "painelCad.h"

double Ponto::xOrigem = 5000.0; // Simulação de Marco Zero
double Ponto::yOrigem = 1000.0;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    TerraView::painelCad visualizador;
    visualizador.setWindowTitle("TerraView - Base");
    visualizador.resize(800, 600);

    visualizador.show();

    return app.exec();
}
*/

/*
#include <QApplication>
#include "leitor.h"      // TerraIO
#include "superficie.h"  // TerraCore
#include "painelCad.h"   // TerraView
#include <algorithm>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 1. CARGA (TerraIO)
    auto entrega = TerraIO::MotorIO::processar("C:/Tony/Soft/soft2026/Instâncias/Pontos.pdw");

    // 2. ORDENAÇÃO (O DNA de Morton que acabamos de validar)
    std::sort(entrega.pontos.begin(), entrega.pontos.end(), [](const TerraCore::Ponto& a, const TerraCore::Ponto& b) {
        return a.dna < b.dna;
    });

    // 3. INTERFACE (TerraView)
    TerraView::painelCad visualizador;

    // Aqui injetamos os pontos no painel para ele saber o que desenhar
    // (Precisaremos criar esse método 'setPontos' no seu painel)
    visualizador.setPontos(entrega.pontos);

    visualizador.setWindowTitle("TerraView - 531 Pontos Ordenados");
    visualizador.resize(1024, 768);
    visualizador.show();

    return app.exec();
}
*/

/*
#include "leitor.h"
#include "superficie.h"
#include <iostream>
#include <algorithm>

int main() {
    // 1. O Gerente pede a carga
    auto entrega = TerraIO::MotorIO::processar("C:/Tony/Soft/soft2026/Instâncias/Pontos.pdw");

    if (entrega.pontos.empty()) {
        std::cerr << "Erro: Nenhum ponto carregado!" << std::endl;
        return -1;
    }

    // No main.cpp, logo após a carga:
    std::sort(entrega.pontos.begin(), entrega.pontos.end(), [](const TerraCore::Ponto& a, const TerraCore::Ponto& b) {
        return a.dna < b.dna;
    });


    // 2. Cabeçalho da Inspeção
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "--- INSPECAO DE COORDENADAS LOCAIS ---" << std::endl;
    std::cout << "Marco Zero (Minimos): X=" << TerraCore::Ponto::xOrigem
              << " | Y=" << TerraCore::Ponto::yOrigem << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << std::left << std::setw(6)  << "ID"
              << std::setw(15) << "LOCAL X"
              << std::setw(15) << "LOCAL Y"
              << std::setw(15) << "LOCAL Z"
              << "ATRIBUTO" << std::endl;

    // 3. Listagem dos Pontos (A prova real)
    for (size_t i = 0; i < entrega.pontos.size(); ++i) {
        const auto& p = entrega.pontos[i];

        // Se p.x ou p.y forem negativos aqui, nosso MotorIO falhou na varredura!
        std::cout << std::left << std::setw(6) << i
                  << std::setw(15) << p.x()
                  << std::setw(15) << p.y()
                  << std::setw(15) << p.z();

        // Acesso ao atributo via std::get (assumindo que seja idAmostra)
        if (auto* dados = std::get_if<TerraCore::idAmostra>(&p.dados)) {
            std::cout << dados->attr << '\t' << dados->id;
        }

        std::cout << std::endl;
    }

    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "Total de pontos em memoria: " << entrega.pontos.size() << std::endl;

    return 0;
}
*/
