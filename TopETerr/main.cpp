#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include "motorArquivoFixo.h"
//#include "laboratorio.h"
#include "adaptadorPontos.h"
#include "ponto.h"

// Reserva o espaço físico para as variáveis estáticas
double Ponto::xOrigem = 0.0;
double Ponto::yOrigem = 0.0;


int main() {
    // 1. Configurações de Acesso
    std::string caminho = R"(C:\2026\Soft\Instâncias\Pontos.pdw)";
    TerraIO::LayoutConfig layout = {16, 16, 12, 12, 12};

    // 2.1 Importação da Matriz Bruta
    auto matrizBruta = TerraIO::motorArquivoFixo::importarParaMatriz(caminho, layout);

    if (matrizBruta.empty()) {
        std::cout << "Erro: Nenhum dado carregado do arquivo." << std::endl;
        return -1;
    }

    // 2.2 BUSCA DO MARCO ZERO (A primeira passada na matriz)
    double minX = 999999999.0; // Valores "infinitos" para começar
    double minY = 999999999.0;

    for (const auto& fatias : matrizBruta) {
        if (fatias.size() >= 5) {
            double x = TerraIO::fix12ParaDouble(fatias[2]);
            double y = TerraIO::fix12ParaDouble(fatias[3]);
            if (x < minX) minX = x;
            if (y < minY) minY = y;
        }
    }

    // 3.1 SETAGEM DAS CONSTANTES (A casa está pronta para os hóspedes)
    Ponto::xOrigem = minX;
    Ponto::yOrigem = minY;

    // 3.2 CARGA: Transformando a matriz em nosso Pool de Pontos (Variáveis de Trabalho)
    std::vector<Ponto> poolPontos;
    poolPontos.reserve(matrizBruta.size()); // Assembly mindset: aloca de uma vez

    for (const auto& fatias : matrizBruta) {
        // O Laboratório faz a ponte e o construtor do Ponto faz a limpeza
        //poolPontos.push_back(TerraLaboratorio::AdaptadorUniversal::criarPonto(fatias));
        poolPontos.push_back(TerraIO::AdaptadorPontos::criarPonto(fatias));
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
// 1. Importação da Matriz Bruta (Ainda não temos nenhum objeto Ponto)
auto matrizBruta = TerraIO::motorArquivoFixo::importarParaMatriz(caminho, layout);

// 2. BUSCA DO MARCO ZERO (A primeira passada na matriz)
double minX = 999999999.0; // Valores "infinitos" para começar
double minY = 999999999.0;

for (const auto& fatias : matrizBruta) {
    if (fatias.size() >= 5) {
        double x = TerraIO::fix12ParaDouble(fatias[2]);
        double y = TerraIO::fix12ParaDouble(fatias[3]);
        if (x < minX) minX = x;
        if (y < minY) minY = y;
    }
}

// 3. SETAGEM DAS CONSTANTES (A casa está pronta para os hóspedes)
Ponto::xOrigem = minX;
Ponto::yOrigem = minY;

// 4. CARGA DOS PONTOS (Agora sim, o construtor faz X_global - X_origem)
std::vector<Ponto> poolPontos;
poolPontos.reserve(matrizBruta.size());
for (const auto& fatias : matrizBruta) {
    poolPontos.push_back(TerraLaboratorio::AdaptadorUniversal::criarPonto(fatias));
}

*/

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
