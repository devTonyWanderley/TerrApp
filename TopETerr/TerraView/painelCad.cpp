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

    // Fundo escuro para destacar a obra
    painter.fillRect(rect(), Qt::black);

    // Calibração Automática (Gambiarra temporária para o teste de hoje)
    // Se não tivermos zoom, vamos chutar um valor para ver os 531 pontos
    if (_zoom == 1.0f) _zoom = 3.5f;
    _offsetX = 100; // Afasta da borda
    _offsetY = 100;

    // --- DESENHAR AS FACES (LINHAS VERMELHAS) ---
    painter.setPen(QPen(Qt::red, 1, Qt::SolidLine));

    for (const auto& f : _faces) {
        // Pegamos os 3 pontos do pool global usando os índices da Face
        const auto& p0 = _estoque[f.v[0]];
        const auto& p1 = _estoque[f.v[1]];
        const auto& p2 = _estoque[f.v[2]];

        // Conversão para Pixels
        auto tela0 = QPointF((p0.x() * _zoom) + _offsetX, height() - ((p0.y() * _zoom) + _offsetY));
        auto tela1 = QPointF((p1.x() * _zoom) + _offsetX, height() - ((p1.y() * _zoom) + _offsetY));
        auto tela2 = QPointF((p2.x() * _zoom) + _offsetX, height() - ((p2.y() * _zoom) + _offsetY));

        // Desenha o triângulo
        painter.drawLine(tela0, tela1);
        painter.drawLine(tela1, tela2);
        painter.drawLine(tela2, tela0);
    }

    // --- DESENHAR OS PONTOS (CRUZES BRANCAS) ---
    painter.setPen(QPen(Qt::white, 1));
    for (const auto& p : _visiveis) {
        float px = (p.x() * _zoom) + _offsetX;
        float py = height() - ((p.y() * _zoom) + _offsetY);
        painter.drawPoint(QPointF(px, py));
    }
}
}

/*
void painelCad::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Fundo escuro para destacar a obra
    painter.fillRect(rect(), Qt::black);

    // Calibração Automática (Gambiarra temporária para o teste de hoje)
    // Se não tivermos zoom, vamos chutar um valor para ver os 531 pontos
    if (_zoom == 1.0f) _zoom = 0.5f;
    _offsetX = 100; // Afasta da borda
    _offsetY = 100;

    // --- DESENHAR AS FACES (LINHAS VERMELHAS) ---
    painter.setPen(QPen(Qt::red, 1, Qt::SolidLine));

    for (const auto& f : _faces) {
        // Pegamos os 3 pontos do pool global usando os índices da Face
        const auto& p0 = _estoque[f.v[0]];
        const auto& p1 = _estoque[f.v[1]];
        const auto& p2 = _estoque[f.v[2]];

        // Conversão para Pixels
        auto tela0 = QPointF((p0.x() * _zoom) + _offsetX, height() - ((p0.y() * _zoom) + _offsetY));
        auto tela1 = QPointF((p1.x() * _zoom) + _offsetX, height() - ((p1.y() * _zoom) + _offsetY));
        auto tela2 = QPointF((p2.x() * _zoom) + _offsetX, height() - ((p2.y() * _zoom) + _offsetY));

        // Desenha o triângulo
        painter.drawLine(tela0, tela1);
        painter.drawLine(tela1, tela2);
        painter.drawLine(tela2, tela0);
    }

    // --- DESENHAR OS PONTOS (CRUZES BRANCAS) ---
    painter.setPen(QPen(Qt::white, 1));
    for (const auto& p : _visiveis) {
        float px = (p.x() * _zoom) + _offsetX;
        float py = height() - ((p.y() * _zoom) + _offsetY);
        painter.drawPoint(QPointF(px, py));
    }
}

*/
