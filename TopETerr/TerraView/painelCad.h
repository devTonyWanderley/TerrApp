#pragma once
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <vector>
#include "geometria.h" // Para enxergar o Ponto

namespace TerraView {

class painelCad : public QWidget {
    Q_OBJECT

public:
    explicit painelCad(QWidget* parent = nullptr);

    // O comando que o main dará para iniciar a obra
    void setPontos(const std::vector<TerraCore::Ponto>& pontos);

protected:
    // Onde o "entalhe" visual acontece
    void paintEvent(QPaintEvent* event) override;

private slots:
    // A batida do relógio para a animação
    void animarPasso();

private:
    std::vector<TerraCore::Ponto> _estoque;
    std::vector<TerraCore::Ponto> _visiveis;

    QTimer* _cronometro;
    size_t _proximoIndice = 0;

    // Controle de Câmera (Simples para começar)
    double _zoom = 4.0;
    double _offsetX = 250.0;
    double _offsetY = 50.0;
};
}
