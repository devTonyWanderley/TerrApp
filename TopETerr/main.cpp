#include <iostream>
#include <vector>
//#include <string>
//#include <variant>
#include "motorArquivoFixo.h"
#include "laboratorio.h"
#include "adaptadorPontos.h"
#include "ponto.h"
#include <algorithm> // Onde mora o mestre std::sort
#include "geometria_base.h"
#include "painelCad.h"
#include <QApplication> // Necessário para gerenciar a janela


// Reserva o espaço físico para as variáveis estáticas
double Ponto::xOrigem = 0.0;
double Ponto::yOrigem = 0.0;

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    TerraView::PainelCad visualizador;
    visualizador.setWindowTitle("TerraView - Motor Gráfico [531 Pontos]");
    visualizador.resize(1024, 768); // Tamanho inicial de conforto



    // 1. Configurações de Acesso
    std::string caminho = u8"C:/2026/Soft/Instâncias/Pontos.pdw";
    TerraIO::LayoutConfig layout = {16, 16, 12, 12, 12};

    // 2.1 Importação da Matriz Bruta
    auto matrizBruta = TerraIO::motorArquivoFixo::importarParaMatriz(caminho, layout);

    if (matrizBruta.empty()) {
        std::cout << "Erro: Nenhum dado carregado do arquivo." << std::endl;
        return -1;
    }

    double minX = 1e18, minY = 1e18, maxX = -1e18, maxY = -1e18;
    for (const auto& fatias : matrizBruta) {
        if (fatias.size() >= 3) {
            double x = TerraIO::fix12ParaDouble(fatias[1]);
            double y = TerraIO::fix12ParaDouble(fatias[2]);
            if (x < minX) minX = x; if (x > maxX) maxX = x;
            if (y < minY) minY = y; if (y > maxY) maxY = y;
        }
    }

    // Define o Referencial de Inércia da Obra
    Ponto::xOrigem = minX;
    Ponto::yOrigem = minY;

    // 3. SEGUNDA PASSADA: CARGA E ORDENAÇÃO DE MORTON
    std::vector<Ponto> poolPontos;
    poolPontos.reserve(matrizBruta.size() + 3); // +3 para o Super-Triângulo
    for (const auto& fatias : matrizBruta) {
        poolPontos.push_back(TerraIO::AdaptadorPontos::criarPonto(fatias));
    }

    // Aplica o "Zíper de Morton" para vizinhança física na RAM
    std::sort(poolPontos.begin(), poolPontos.end(), [](const Ponto& a, const Ponto& b) {
        return Quadtree::gerarMortonBMI2(static_cast<uint32_t>(a.x * 10000),
                                         static_cast<uint32_t>(a.y * 10000)) <
               Quadtree::gerarMortonBMI2(static_cast<uint32_t>(b.x * 10000),
                                         static_cast<uint32_t>(b.y * 10000));
    });

    // 4. NASCIMENTO DA MALHA: SUPER-TRIÂNGULO
    auto sup = TerraLaboratorio::SuperficieInicial::criar(0, 0, maxX - minX, maxY - minY);

    // Adicionamos os vértices fantasmas ao pool e criamos a face mestra
    poolPontos.insert(poolPontos.begin(), sup.vertices.begin(), sup.vertices.end());
    std::vector<Face> faces;
    faces.push_back(sup.faceMestra);
    visualizador.carregarPontos(poolPontos);
    visualizador.show(); // Exibe a tela negra

    // 5. DIAGNÓSTICO VISUAL (O Olho do Engenheiro)
    TerraLaboratorio::exportarParaSVG("diagnostico.svg", poolPontos, faces,
                                      0, 0, maxX - minX, maxY - minY);

    std::cout << "Carga concluída: " << poolPontos.size() - 3 << " pontos reais." << std::endl;
    std::cout << "DNA de Morton aplicado. Diagnostico SVG gerado." << std::endl;
    return app.exec();

    //return 0;
}

/*
#include "terrapleno.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TopETerr_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    Terrapleno w;
    w.show();
    return a.exec();
}
*/
