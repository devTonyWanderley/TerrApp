#pragma once

// --- QT FRAMEWORK ---
#include <QWidget>      // Classe base do painel
#include <QTimer>       // Para a animação da serpente de Morton
#include <QPainter>     // Para o "formão" que desenha na tela
#include <QPaintEvent>  // Para capturar o momento de redesenhar
#include <QPointF>      // Para coordenadas de precisão no desenho

// --- ESTOQUE (SEU MOTOR) ---
#include <vector>       // Para armazenar o pool
#include "geometria.h"  // PARA RECONHECER: TerraCore::Ponto e TerraCore::Face

namespace TerraView {

class painelCad : public QWidget {
    Q_OBJECT // 1. OBRIGATÓRIO para usar Slots e Signals

public:
    explicit painelCad(QWidget* parent = nullptr);

    // Métodos normais (Não são slots)
    void setPontos(const std::vector<TerraCore::Ponto>& pontos);
    void setFaces(const std::vector<TerraCore::Face>& faces); // Para amanhã

public slots:
    // 2. APENAS o que o QTimer vai chamar deve estar aqui
    void animarPasso();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QTimer* _cronometro;
    std::vector<TerraCore::Ponto> _estoque;
    std::vector<TerraCore::Ponto> _visiveis;
    std::vector<TerraCore::Face> _faces;
    size_t _proximoIndice = 0;

    // Zoom e Offset para caber na tela
    float _zoom = 1.0f;
    float _offsetX = 0.0f;
    float _offsetY = 0.0f;
};

} // namespace TerraView
