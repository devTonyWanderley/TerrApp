#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QKeyEvent>
#include <vector>
#include "ponto.h" // O View "enxerga" o Core

namespace TerraView {

class PainelCad : public QGraphicsView {
    Q_OBJECT

public:
    explicit PainelCad(QWidget* parent = nullptr);

    // Alimenta a tela com os dados do motor geométrico
    void carregarPontos(const std::vector<Ponto>& pontos);

    // O "Z" do CAD (Zoom Extents)
    void zoomTotal();

protected:
    // Eventos de baixo nível para controle total
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QGraphicsScene* cena;
    const double fatorZoom = 1.15;
};

} // namespace TerraView
