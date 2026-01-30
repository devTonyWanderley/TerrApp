#include <string>

// Em TerraCore/io_serial.h (C++ Independente)
class LinkSerialBase {
public:
    virtual bool abrirPorta(const std::string& nome) = 0;
    virtual void enviarProtocolo(const std::string& dado) = 0; // Seus 16/12/7 bytes
    virtual std::string receberProtocolo() = 0;
    virtual ~LinkSerialBase() = default;
};
