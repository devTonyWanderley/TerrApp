#include <iomanip>
#include <cmath>

namespace TerraIO {

// Helper para converter double (metros) para décimo de milímetro (long long)
// Ex: 1.2345 metros -> 12345 décimos
inline std::string formatarNumero(double valor) {
    long long decimos = std::round(valor * 10000.0);
    std::stringstream ss;
    ss << std::setw(12) << std::setfill('0') << decimos;
    return ss.str();
}

// Formata string para exatos 16 caracteres (trunca ou preenche com espaços)
inline std::string formatarString(const std::string& texto) {
    std::string s = texto;
    s.resize(16, ' ');
    return s;
}

// Graus Minutos Segundos (GGMMSSs) - 7 caracteres
// Ex: 125° 30' 15" -> 1253015
inline std::string formatarAnguloGMS(double anguloGraus) {
    // Lógica de conversão para GMS...
    // std::setw(7) << std::setfill('0') << gms;
    return "0000000"; // Placeholder
}
}
