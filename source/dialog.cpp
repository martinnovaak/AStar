#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

void Dialog::setRS(int m, int n)
{
    ui->spinBox->setValue(m);
    ui->spinBox_2->setValue(n);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    this->R = ui->spinBox->value();
    this->S = ui->spinBox_2->value();
    this->close();
}

