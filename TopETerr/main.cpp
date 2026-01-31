#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include "motorArquivoFixo.h"
#include "laboratorio.h"
#include "ponto.h"

int main() {
    // 1. Configurações de Acesso
    std::string caminho = R"(C:\2026\Soft\Instâncias\Pontos.pdw)";
    TerraIO::LayoutConfig layout = {16, 16, 12, 12, 12};

    // 2. Importação da Matriz Bruta
    auto matrizBruta = TerraIO::motorArquivoFixo::importarParaMatriz(caminho, layout);

    if (matrizBruta.empty()) {
        std::cout << "Erro: Nenhum dado carregado do arquivo." << std::endl;
        return -1;
    }

    // 3. CARGA: Transformando a matriz em nosso Pool de Pontos (Variáveis de Trabalho)
    std::vector<Ponto> poolPontos;
    poolPontos.reserve(matrizBruta.size()); // Assembly mindset: aloca de uma vez

    for (const auto& fatias : matrizBruta) {
        // O Laboratório faz a ponte e o construtor do Ponto faz a limpeza
        poolPontos.push_back(TerraLaboratorio::AdaptadorUniversal::criarPonto(fatias));
    }

    // 4. IMPRESSÃO: Validando os dados limpos e convertidos
    std::cout << "--- POOL DE PONTOS (Variaveis de Trabalho) ---" << std::endl;
    std::cout << "Total carregado: " << poolPontos.size() << " pontos.\n" << std::endl;

    for (const auto& p : poolPontos) {
        // Acesso seguro ao variant de idAmostra para imprimir ID e Attr
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, idAmostra>) {
                // Aqui veremos os dados SEM os espaços (Trimmed)
                std::cout << "ID:[" << arg.id << "] ATTR:[" << arg.attr << "]";
            }
        }, p.dados);

        // Imprime as coordenadas convertidas do décimo de milímetro para metro
        std::printf(" | X: %12.4f | Y: %12.4f | Z: %8.4f\n", p.x, p.y, p.z);
    }

    std::cout << "\n--- TESTE CONCLUIDO COM SUCESSO ---" << std::endl;
    return 0;
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
