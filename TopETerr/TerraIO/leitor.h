#pragma once
#include <vector>
#include <iomanip>
#include <cmath>
#include <sstream>
#include <string>
#include <fstream>
#include <filesystem>
#include "geometria.h"

namespace TerraIO
{

//  --FILTROS E CONVERSÕES--
inline std::string doubleParaFix12(double valor)
{
    long long decimos = std::round(valor * 10000.0);
    std::stringstream ss;
    ss << std::setw(12) << std::setfill('0') << decimos;
    return ss.str().substr(0, 12);
}

inline double fix12ParaDouble(const std::string& s)
{
    try
    {
        return std::stoll(s) / 10000.0;
    }
    catch (...)
    {
        return 0.0;
    }
}

inline std::string stringParaFix(const std::string& texto, size_t largura)
{
    std::string s = texto;
    s.resize(largura, ' ');
    return s.substr(0, largura);
}

inline std::string stringParaFix16(const std::string& texto)
{
    return stringParaFix(texto, 16);
}

using layoutConfig = std::vector<int>;

//  --INTERPRETAR ARQUIVOS DE COMPRIMENTO FIXO--
class motorArqFix
{
public:
    static std::vector<std::string> fatiarLinha(const std::string& linha, const layoutConfig& layout)
    {
        std::vector<std::string> fatias;
        fatias.reserve(layout.size());
        size_t cursor = 0;
        for(int largura : layout)
        {
            if(cursor < linha.length())
                fatias.push_back(linha.substr(cursor, largura));
            else
                fatias.push_back("");
            cursor += largura;
        }
        return fatias;
    }

    static std::vector<std::vector<std::string>> importarFixo(const std::string& idArq, const layoutConfig& layout)
    {
        std::vector<std::vector<std::string>> leituras;
        std::filesystem::path Path(idArq);
        std::ifstream arquivo(Path);
        std::string linha;
        if(!arquivo.is_open()) return leituras;
        while(std::getline(arquivo, linha))
        {
            if(linha.empty()) continue;
            if(linha.back() == '\r') linha.pop_back();
            if(linha.empty()) continue;
            if(linha[0] == ';') continue;
            leituras.push_back(fatiarLinha(linha, layout));
        }
        return leituras;
    }

    static std::string montarLinha(const std::vector<std::string>& dados, const layoutConfig& layout)
    {
        std::string linha;
        size_t tam = 0;
        for(int l : layout) tam += l;
        linha.reserve(tam);
        for(size_t i = 0; i < layout.size(); i++)
        {
            std::string reg = (i < dados.size() ? dados[i] : "");
            reg.resize(layout[i], ' ');
            linha += reg.substr(0, layout[i]);
        }
        return linha;
    }
};  //  class motorArqFix

//  --PONTE TERRACORE--
class MotorIO
{
public:
    // Estrutura para entregar o serviço completo
    struct Entrega
    {
        std::vector<TerraCore::Ponto> pontos;
        std::vector<std::string> estranhezas;
    };

    // O método que o main.cpp vai chamar
    static Entrega processar(const std::string& caminho)
    {
        Entrega resultado;
        auto matriz = motorArqFix::importarFixo(caminho, {16, 16, 12, 12, 12});

        if(matriz.empty()) return resultado;

        double minX = 1e18, minY = 1e18;
        for(const auto& linha : matriz)
        {
            if(linha.size() < 5) continue;
            double yG = fix12ParaDouble(linha[2]), xG = fix12ParaDouble(linha[3]);
            if(xG < minX) minX = xG;
            if(yG < minY) minY = yG;
        }

        TerraCore::Ponto::xOrigem = minX;
        TerraCore::Ponto::yOrigem = minY;

        resultado.pontos.reserve(matriz.size());
        for(const auto& linha : matriz)
        {
            if(linha.size() < 5) continue;
            double yG = fix12ParaDouble(linha[2]);
            double xG = fix12ParaDouble(linha[3]);
            double zG = fix12ParaDouble(linha[4]);
            resultado.pontos.emplace_back(xG, yG, zG, linha[0], linha[1]);
        }

        return resultado;
    }
};  //  class MotorIO

}   //  namespace TerraIO
