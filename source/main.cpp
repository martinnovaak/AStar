#include "bludiste.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Bludiste w;
    w.show();
    w.setWindowIcon(QIcon(":/blud.PNG"));
    return a.exec();
}
