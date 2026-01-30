#include "terrapleno.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TopETerr_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    Terrapleno w;
    w.show();
    return a.exec();
}
/*
 * #include <iostream>
#include "ponto.h"

int main() {
    Ponto p(100.0, 200.0, 50.0, "P1", "BORDO");
    std::cout << "Ponto estruturado com sucesso em C++17!" << std::endl;
    return 0;
}
*/
