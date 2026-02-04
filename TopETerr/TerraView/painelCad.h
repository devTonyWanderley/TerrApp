#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QKeyEvent>

namespace TerraView {
class painelCad : public QGraphicsView {
    Q_OBJECT
public:
    explicit painelCad(QWidget* parent = nullptr);
    void carregarTeste(); // Apenas para validar a cruz e o ponto
protected:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void recalcularHorizonte();
    void mouseReleaseEvent(QMouseEvent* event) override;
    void zoomLimites();
private:
    QGraphicsScene* cena;
};
}
