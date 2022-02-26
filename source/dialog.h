#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    void setRS(int a, int b);
    ~Dialog();
    unsigned int getR() { return this->R;};
    unsigned int getS() { return this->S;};
private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;

    unsigned int R;
    unsigned int S;
};

#endif // DIALOG_H
