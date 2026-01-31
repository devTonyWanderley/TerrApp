#pragma once
#include <string>
#include <iomanip>
#include <cmath>
#include <sstream>

namespace TerraIO
{
// Metros (double) -> Décimos de mm (String 12 chars)
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

// String livre -> String tamanho fixo (N chars)
inline std::string stringParaFix(const std::string& texto, size_t largura)
{
    std::string s = texto;
    s.resize(largura, ' ');
    return s.substr(0, largura);
}

inline std::string stringParaFix16(const std::string& texto) {
    return stringParaFix(texto, 16);
}
}
