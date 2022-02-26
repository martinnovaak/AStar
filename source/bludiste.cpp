#include "bludiste.h"
#include <QVariant>
#include "AStar.h"
#include "dialog.h"
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QFileDialog>
#include <fstream>
#include <QMessageBox>
#include <QDebug>

Bludiste::Bludiste(QWidget *parent)
    : QMainWindow(parent), start(nullptr), cil(nullptr)
{
    setWindowTitle("Bludiště");
    nactiNastaveni();
    nastavFormular();
}

void Bludiste::nastavFormular()
{
    povoleni = true;

    pocatek = new QAction("start");
    konec = new QAction("cíl");
    prekazky = new QAction("prekážky");
    vyhledat = new QAction("A*");
    vyhledatDijkstrou = new QAction("Dijkstra");
    vycistit = new QAction("vyčistit");
    odstranVysledek = new QAction("odstraň výsledek");
    nastaveni = new QAction("nastavení");
    nacistPbm = new QAction("načíst bludiště");
    invertovat = new QAction("zaměnit bílou a černou");

    menubar = new QMenuBar;
    QMenu * upravy = new QMenu("Úpravy");
    upravy->addAction(pocatek);
    upravy->addAction(konec);
    upravy->addAction(prekazky);
    upravy->addSeparator();
    upravy->addAction(vyhledat);
    upravy->addAction(vyhledatDijkstrou);
    upravy->addAction(vycistit);
    upravy->addAction(odstranVysledek);
    menubar->addMenu(upravy);

    QMenu * nastroje = new QMenu("Nástroje");
    nastroje->addAction(nastaveni);
    nastroje->addAction(nacistPbm);
    nastroje->addAction(invertovat);
    menubar->addMenu(nastroje);
    setMenuBar(menubar);

    vyhledat->setEnabled(false);
    vyhledatDijkstrou->setEnabled(false);

    connect(pocatek, SIGNAL(triggered()), this, SLOT(akceStart()));
    connect(konec, SIGNAL(triggered()), this, SLOT(akceCil()));
    connect(prekazky, SIGNAL(triggered()), this, SLOT(akcePrekazky()));
    connect(vyhledat, &QAction::triggered, this, [this]{akceVyhledat(true);});
    connect(vyhledatDijkstrou, &QAction::triggered, this, [this]{akceVyhledat(false);});
    connect(vycistit, SIGNAL(triggered()), this, SLOT(akceVycistit()));
    connect(odstranVysledek, &QAction::triggered, this, &Bludiste::akceOdstranVysledek);
    connect(nastaveni, SIGNAL(triggered()), this, SLOT(akceNastaveni()));
    connect(nacistPbm, SIGNAL(triggered()), this, SLOT(akceNacist()));
    connect(invertovat, SIGNAL(triggered()), this, SLOT(akceInvertovat()));

    nastavBludiste();
}

void Bludiste::nastavBludiste()
{
    for(int i = 0; i < tlacitka.size(); i++)
    {
        tlacitka[i]->deleteLater();
    }
    tlacitka.clear();
    for (unsigned int i = 0;i<this->m;i++)
    {
       for (unsigned int j = 0;j<this->n;j++)
       {
           QPushButton *tl = new QPushButton(this);
           tl->setGeometry(j*30,i*30+27,30,30);
           //
           setColor(tl, QColor(Qt::white));
           //tl->setStyleSheet("background-color: rgb(255, 255, 255)");
           tl->setText("");
           tl->setProperty("barva", 0);
           tl->setVisible(true);
           connect(tl,&QPushButton::clicked,this,&Bludiste::stisknuto);
           tlacitka.push_back(tl);
       }
    }
    this->setFixedSize(n*30,m*30+27);
}

void Bludiste::setColor(QPushButton * tl, QColor col)
{
    if(col.isValid()) {
       QString qss = QString("background-color: %1").arg(col.name());
       tl->setStyleSheet(qss);
    }
}

void Bludiste::stisknuto()
{
    if (!povoleni)
        return;
    QPushButton *tl = dynamic_cast<QPushButton *>(sender());
    tl->setAutoFillBackground(true);
    if (o == 1) // start
    {
        stisknuto_start(tl);
    }
    else if (o == 2) // cíl
    {
        stisknuto_cil(tl);
    }
    else // překážka
    {
        stisknuto_prekazka(tl);
    }
}

void Bludiste::stisknuto_start(QPushButton *tl)
{
    if (start == tl)
        return;
    if (tl == cil)
        cil = nullptr;
    //tl->setStyleSheet("background-color: rgb(255, 0, 0)");
    setColor(tl, QColor(Qt::red));
    tl->setProperty("barva", o);
    if (start)
    {
        //start->setStyleSheet("background-color: rgb(255, 255, 255)");
        setColor(start, QColor(Qt::white));
        start->setProperty("barva", 0);
    }
    start = tl;
    if (cil != nullptr)
    {
        this->vyhledat->setEnabled(true);
        this->vyhledatDijkstrou->setEnabled(true);
    }
}

void Bludiste::stisknuto_cil(QPushButton *tl)
{
    if (cil == tl)
        return;
    if (start == tl)
        start = nullptr;
    //tl->setStyleSheet("background-color: rgb(0, 0, 255)");
    setColor(tl, QColor(Qt::blue));
    tl->setProperty("barva", o);
    if (cil)
    {
        //cil->setStyleSheet("background-color: rgb(255, 255, 255)");
        setColor(cil, QColor(Qt::white));
        cil->setProperty("barva",0);
    }
    cil = tl;
    if (start != nullptr)
    {
        this->vyhledat->setEnabled(true);
        this->vyhledatDijkstrou->setEnabled(true);
    }
}

void Bludiste::stisknuto_prekazka(QPushButton *tl)
{
    int barva = tl->property("barva").toInt();
    if (barva == 1)
    {
        start = nullptr;
        //tl->setStyleSheet("background-color: rgb(0, 0, 0)");
        setColor(tl, QColor(Qt::black));
        tl->setProperty("barva", -1);
        this->vyhledat->setEnabled(false);
        this->vyhledatDijkstrou->setEnabled(false);
    }
    else if(barva == 2)
    {
        cil = nullptr;
        //tl->setStyleSheet("background-color: rgb(0, 0, 0)");
        setColor(tl, QColor(Qt::black));
        tl->setProperty("barva", -1);
        this->vyhledat->setEnabled(false);
        this->vyhledatDijkstrou->setEnabled(false);
    }
    else if (barva == 0)
    {
        //tl->setStyleSheet("background-color: rgb(0, 0, 0)");
        setColor(tl, QColor(Qt::black));
        tl->setProperty("barva", -1);
    }
    else
    {
        //tl->setStyleSheet("background-color: rgb(255, 255, 255)");
        setColor(tl, QColor(Qt::white));
        tl->setProperty("barva", 0);
    }
}

void Bludiste::akceStart()
{
    this->pocatek->setEnabled(false);
    this->konec->setEnabled(true);
    this->prekazky->setEnabled(true);

    povoleni = true;

    o = 1;
}

void Bludiste::akceCil()
{
    this->pocatek->setEnabled(true);
    this->konec->setEnabled(false);
    this->prekazky->setEnabled(true);

    povoleni = true;

    o = 2;
}

void Bludiste::akcePrekazky()
{
    this->pocatek->setEnabled(true);
    this->konec->setEnabled(true);
    this->prekazky->setEnabled(false);

    povoleni = true;

    o = 0;
}

// bool astar je true, pokud v pripade Astaru v pripade dijkstry je false
void Bludiste::akceVyhledat(bool astar)
{
    povoleni = false; // zakaz nastavovani startu, cile a prekazek

    QVector<int> cesta, oblast; // do vektoru cesta bude ulozena nejkratsi cesta, do vektoru oblast prozkoumana oblast v algoritmu
    //this->vyhledat->setEnabled(true);
    this->pocatek->setEnabled(false);
    this->prekazky->setEnabled(false);
    this->konec->setEnabled(false);

    AStar a(m,n, astar);
    int barva;
    for (unsigned int i = 0; i < m; i++)
    {
        for (unsigned int j = 0; j < n; j++)
        {
            barva = this->tlacitka.at(i*n+j)->property("barva").toInt();
            if (barva  == 1)
            {
                a.setStart(i,j);
                a(i, j) = 1;
            }
            else if (barva == 2)
            {
                a.setCil(i,j);
                a(i, j) = 1;
            }
            else if (barva == 0)
            {
                setColor(tlacitka[i*n+j], QColor(Qt::white));
                a(i, j) = 1;
            }
            else
                a(i, j) = 0;
        }
    }
    a.vyres(cesta, oblast);
    int i,j;
    int l = 0;
    while (l < cesta.size())
    {
        i = cesta.at(l);
        j = cesta.at(l+1);
        //this->tlacitka.at(i*n+j)->setStyleSheet("background-color: rgb(0, 255, 0)");
        setColor(this->tlacitka.at(i*n+j), QColor(Qt::green));
        l+=2;
    }
    l=0;
    while (l < oblast.size())
    {
        i = oblast[l];
        j = oblast[l+1];
        //this->tlacitka.at(i*n+j)->setStyleSheet("background-color: rgb(255, 255, 0)");
        setColor(this->tlacitka.at(i*n+j), QColor(Qt::yellow));
        l += 2;
    }
}

void Bludiste::akceVycistit()
{
    this->pocatek->setEnabled(true);
    this->prekazky->setEnabled(true);
    this->konec->setEnabled(true);

    for (unsigned int i = 0; i < m * n; i++)
    {
        //this->tlacitka.at(i)->setStyleSheet("background-color: rgb(255, 255, 255)");
        setColor(this->tlacitka.at(i), QColor(Qt::white));
        this->tlacitka.at(i)->setProperty("barva", 0);
    }

    this->vyhledat->setEnabled(false);
    this->vyhledatDijkstrou->setEnabled(false);
    this->start = nullptr;
    this->cil = nullptr;
}

void Bludiste::akceOdstranVysledek()
{
    this->pocatek->setEnabled(true);
    this->konec->setEnabled(true);
    this->prekazky->setEnabled(true);
    for (unsigned int i = 0; i < m * n; i++)
    {
        int barva = this->tlacitka.at(i)->property("barva").toInt();
        if (barva  == 0)
        {
            setColor(this->tlacitka.at(i), QColor(Qt::white));
        }
    }
}

void Bludiste::akceNastaveni()
{
    Dialog * ui = new Dialog;
    ui->setRS(m,n);
    ui->exec();
    this->m = ui->getR();
    this->n = ui->getS();
    nastavBludiste();
    ulozNastaveni();
}

void Bludiste::akceNacist()
{
    QString cestaSouboru;
    QString nazevSouboru = QFileDialog::getOpenFileName(this, "Otevři soubor", cestaSouboru, "PBM File (*.pbm)");
    QFileInfo i(nazevSouboru);
    if(nazevSouboru.isEmpty())
        return;
    nactiPBM(i.absoluteFilePath().toStdString());
}

void Bludiste::ulozNastaveni()
{
    QSettings settings("FNSPE CTU", "Bludiste");
    settings.setValue("m", this->m);
    settings.setValue("n", this->n);
}

void Bludiste::nactiNastaveni()
{
    QSettings settings("FNSPE CTU", "Bludiste");
    this->m = settings.value("m", this->n).toInt();
    this->n = settings.value("n", this->n).toInt();
}

void Bludiste::akceInvertovat()
{
    for( unsigned int i = 0; i < this->m * this->n; i++)
    {
        if (tlacitka.at(i)->property("barva").toInt() == 0)
        {
            //tlacitka.at(i)->setStyleSheet("background-color: rgb(0,0,0)");
            setColor(this->tlacitka.at(i), QColor(Qt::black));

            tlacitka.at(i)->setProperty("barva", -1);
        }
        else if(tlacitka.at(i)->property("barva").toInt() == -1)
        {
            //tlacitka.at(i)->setStyleSheet("background-color: rgb(255,255,255)");
            setColor(this->tlacitka.at(i), QColor(Qt::white));
            tlacitka.at(i)->setProperty("barva", 0);
        }
    }
}

Bludiste::~Bludiste()
{
}

//___________________________________________________________________________________________________

void Bludiste::nactiPBM(std::string cesta)
{
    //1. otevreni souboru pro cteni
        std::fstream f(cesta, std::ios_base::in); // ios base in rezim pro cteni
        if (f)
        {
            //2. precteni identifikatoru P1
            std::string word;
            f >> word;
            if (word != "P1")
            {
                QMessageBox::warning(this, "Error","Nesprávný formát PBM souboru.");
                f.close(); // neni povinny zavrel by se v destruktoru
                return;
            }

            //3. nacteni rozmeru a alokace pameti
            unsigned int x, y;
            f >> x >> y;
            //close();
            unsigned int counter = 0;
            int * blud = new int[x*y];
            //4. nacitani dat
            while (f >> word)
            {
                for (unsigned int i = 0; i < word.length(); i++)
                {
                    switch(word[i])
                    {
                    case '0' :
                        blud[counter] = -1;
                        break;
                    case '1' :
                        blud[counter] = 0;
                        break;
                    default:        
                        delete[] blud;
                        f.close();
                        QMessageBox::warning(this, "Error","PBM soubor se napodařilo načíst");
                        return;
                    }
                    counter++;
                    if (counter > x*y)
                    {                      
                        delete[] blud;
                        f.close();
                        QMessageBox::warning(this, "Error","Moc dat v souboru");
                        return;
                    }
                }
            }
            if (counter < x*y)
            {
                delete[] blud;
                f.close();
                QMessageBox::warning(this, "Error","Málo dat v souboru");
                return;
            }

            this->m = y;//
            this->n = x;//
            nastavBludiste();
            for (unsigned int i = 0; i < x*y; i++)
            {
                if (blud[i])
                {
                    //this->tlacitka.at(i)->setStyleSheet("background-color: rgb(0, 0, 0)");
                    setColor(this->tlacitka.at(i), QColor(Qt::black));
                    this->tlacitka.at(i)->setProperty("barva", -1);
                }
            }
            delete[] blud;
        }
        else
        {
            QMessageBox::warning(this, "Error","PBM soubor se napodařilo načíst");
            return;
        }
}
