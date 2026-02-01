#include "painelCad.h"
#include <QGraphicsEllipseItem>

namespace TerraView {

PainelCad::PainelCad(QWidget* parent) : QGraphicsView(parent) {
    cena = new QGraphicsScene(this);
    setScene(cena);

    // Estética CAD: Fundo preto absoluto
    setBackgroundBrush(Qt::black);
    setRenderHint(QPainter::Antialiasing);

    // Permite arrastar o desenho com o mouse
    setDragMode(QGraphicsView::ScrollHandDrag);

    // Remove as barras de rolagem (limpeza visual)
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // Zoom foca no mouse
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
}

void PainelCad::carregarPontos(const std::vector<Ponto>& pontos) {
    cena->clear();
    for (const auto& p : pontos) {
        // Adiciona um ponto vermelho (0.2m de diâmetro)
        cena->addEllipse(p.x - 5.0, p.y - 5.0, 10.0, 10.0, QPen(Qt::red), QBrush(Qt::red));
    }

    // Define que o 'papel' tem exatamente o tamanho da obra
    QRectF limites = cena->itemsBoundingRect();
    cena->setSceneRect(limites);

    // Força o enquadramento total
    fitInView(limites, Qt::KeepAspectRatio);
    zoomTotal();
}

void PainelCad::zoomTotal() {
    fitInView(cena->itemsBoundingRect(), Qt::KeepAspectRatio);
    // No construtor PainelCad
    // Se quiser que o Y cresça para cima (Norte), descomente a linha abaixo:
    scale(1, -1);
}

void PainelCad::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) scale(fatorZoom, fatorZoom);
    else scale(1.0 / fatorZoom, 1.0 / fatorZoom);
}

void PainelCad::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Z) zoomTotal();
    QGraphicsView::keyPressEvent(event);
}

} // namespace TerraView
