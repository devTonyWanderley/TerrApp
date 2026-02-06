#include "painelCad.h"

namespace TerraView {

painelCad::painelCad(QWidget* parent) : QWidget(parent) {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    _cronometro = new QTimer(this);
    connect(_cronometro, &QTimer::timeout, this, &painelCad::animarPasso);
}

void painelCad::setPontos(const std::vector<TerraCore::Ponto>& pontos) {
    _estoque = pontos; // Recebe os 531 ordenados
    _visiveis.clear();
    _proximoIndice = 0;
    _cronometro->start(50); // 10ms por ponto (Rápido!)
}

void painelCad::animarPasso() {
    if (_proximoIndice < _estoque.size()) {
        _visiveis.push_back(_estoque[_proximoIndice]);
        _proximoIndice++;
        update(); // Pede ao Qt para redesenhar (chama o paintEvent)
    } else {
        _cronometro->stop();
    }
}

void painelCad::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Desenhar os pontos que já "nasceram"
    painter.setPen(QPen(Qt::red, 1));

    for (const auto& p : _visiveis) {
        // Conversão: Metros -> Pixels + Inversão de Y
        // Usamos p.x() e p.y() que fazem a divisão por 10.000
        float px = (p.x() * _zoom) + _offsetX;
        float py = height() - ((p.y() * _zoom) + _offsetY);

        painter.drawPoint(QPointF(px, py));
        // Desenha uma cruzinha discreta
        painter.drawLine(px - 2, py, px + 2, py);
        painter.drawLine(px, py - 2, px, py + 2);
    }
}
}
