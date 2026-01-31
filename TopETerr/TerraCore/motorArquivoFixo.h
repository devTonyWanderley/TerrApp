#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

namespace TerraIO {

// Configuração puramente numérica: [comprimento1, comprimento2, ...]
using LayoutConfig = std::vector<int>;

class motorArquivoFixo {
public:
    // O Fatiador (A ferramenta atômica que já criamos)
    static std::vector<std::string> fatiarLinha(const std::string& linha, const LayoutConfig& layout) {
        std::vector<std::string> fatias;
        fatias.reserve(layout.size());
        size_t cursor = 0;
        for (int largura : layout) {
            if (cursor < linha.length()) {
                fatias.push_back(linha.substr(cursor, largura));
            } else {
                fatias.push_back("");
            }
            cursor += largura;
        }
        return fatias;
    }

    // O Importador (O procedimento que usa a ferramenta acima)
    static std::vector<std::vector<std::string>> importarParaMatriz(
        const std::string& caminhoUTF8,
        const LayoutConfig& layout)
    {
        std::vector<std::vector<std::string>> matriz;
        std::filesystem::path pathLegivel(caminhoUTF8);
        std::ifstream arquivo(pathLegivel);
        std::string linha;

        if (!arquivo.is_open()) return matriz;

        while (std::getline(arquivo, linha)) {
            // 1. Remove o '\r' (CRLF) se o arquivo veio do Windows/Excel
            if (!linha.empty() && linha.back() == '\r') linha.pop_back();

            // 2. Se a linha estiver vazia após o pop_back, pula
            if (linha.empty()) continue;

            // 3. Se o PRIMEIRO caractere útil for ';', é comentário (Bypass)
            if (linha[0] == ';') continue;

            // Se chegou aqui, a linha tem carne. Fatiar!
            matriz.push_back(fatiarLinha(linha, layout));
        }

        return matriz;
    }

    // Exportação: Une as fatias de volta em uma linha de comprimento fixo
    static std::string montarLinha(const std::vector<std::string>& fatias, const LayoutConfig& layout) {
        std::string linhaBuffer;
        // Otimização: reserva o tamanho total da linha de uma vez
        size_t total = 0;
        for(int l : layout) total += l;
        linhaBuffer.reserve(total);

        for (size_t i = 0; i < layout.size(); ++i) {
            std::string fatia = (i < fatias.size()) ? fatias[i] : "";
            fatia.resize(layout[i], ' '); // Ajuste mecânico de largura
            linhaBuffer += fatia.substr(0, layout[i]);
        }
        return linhaBuffer;
    }
};
}
