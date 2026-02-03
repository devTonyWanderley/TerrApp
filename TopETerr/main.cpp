#include <QApplication>
#include "ponto.h"
#include "painelCad.h"

double Ponto::xOrigem = 5000.0; // Simulação de Marco Zero
double Ponto::yOrigem = 1000.0;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    TerraView::painelCad visualizador;
    visualizador.setWindowTitle("TerraView - Base");
    visualizador.resize(800, 600);

    visualizador.show();

    return app.exec();
}
