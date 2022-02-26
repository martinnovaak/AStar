#ifndef BLUDISTE_H
#define BLUDISTE_H

#include <QMainWindow>
#include <QVector>
#include <QPushButton>
#include <QToolBar>
#include <QAction>
#include <QStatusBar>


class Bludiste : public QMainWindow
{
    Q_OBJECT

    void nastavFormular();
    void nastavBludiste();

    void nactiNastaveni();
    void ulozNastaveni();

    void nactiPBM(std::string cesta);

    void stisknuto_start(QPushButton *tl);
    void stisknuto_cil(QPushButton *tl);
    void stisknuto_prekazka(QPushButton *tl);
    void setColor(QPushButton *tl, QColor col);
public:
    Bludiste(QWidget *parent = nullptr);
    ~Bludiste();

private:
    unsigned int m = 10;
    unsigned int n = 10;
    unsigned int o = 0;

    bool povoleni; // povoleno nastavovat start, cil a prekazky

    QVector <QPushButton *> tlacitka;
    QPushButton* start;
    QPushButton* cil;

    QMenuBar* menubar;
    QAction* pocatek;
    QAction* konec;
    QAction* prekazky;
    QAction* vyhledat;
    QAction* vyhledatDijkstrou;
    QAction* vycistit;
    QAction* odstranVysledek;
    QAction* nastaveni;
    QAction* nacistPbm;
    QAction* invertovat;
private slots:
    void akceStart();
    void akceCil();
    void akcePrekazky();
    void akceVyhledat(bool astar);
    void akceVycistit();
    void akceOdstranVysledek();
    void akceNastaveni();
    void akceNacist();
    void akceInvertovat();
    void stisknuto();
};
#endif // BLUDISTE_H
