#include "terrapleno.h"
#include "ui_terrapleno.h"

Terrapleno::Terrapleno(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Terrapleno)
{
    ui->setupUi(this);
}

Terrapleno::~Terrapleno()
{
    delete ui;
}
