#ifndef TERRAPLENO_H
#define TERRAPLENO_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Terrapleno;
}
QT_END_NAMESPACE

class Terrapleno : public QMainWindow
{
    Q_OBJECT

public:
    Terrapleno(QWidget *parent = nullptr);
    ~Terrapleno();

private:
    Ui::Terrapleno *ui;
};
#endif // TERRAPLENO_H
