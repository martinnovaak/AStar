#include "AStar.h"
#include <QVector>

AStar::AStar(int R, int S, bool astar)//konstruktor
{
    this->R = R;
    this->S = S;
    this->astar = astar;

    this->bludiste = new bool*[R];
    for (int i = 0; i < R; i++)
        this->bludiste[i] = new bool[S];
    this->prozkoumano = new bool*[R];
    for (int i = 0; i < R; i++)
        this->prozkoumano[i] = new bool[S];

    this->mapa = new Pole*[R];
    for (int i = 0; i < this->R; i++)
        this->mapa[i] = new Pole[this->S];
}

AStar::~AStar()//destruktor
{
    for (int i = 0; i < R; i++)
        delete[] bludiste[i];
    delete[] bludiste;
    for (int k = 0; k < R; k++)
        delete[] prozkoumano[k];
    delete[] prozkoumano;
    for (int i = 0; i < R; i++)
        delete[] mapa[i];
    delete[] mapa;
}

bool & AStar::operator()(unsigned int i, unsigned int j)
{
    return bludiste[i][j];
}

//Cesta bude mít podobu retezce souradnic dále vypíše prozkoumaný stavový prostor opět v souradnicích
//jako oddělovač používám charakter x
//posloupnost pak má podobu: "i_1,j_1,i_2,j_2,...,i_n,j_n,x,i_1,j_1,...,i_k,j_k"
void AStar::cesta(Souradnice bod, QVector<int> & cesta, QVector<int> & oblast)
{
    int i = bod.x;
    int j = bod.y;
    if (bod == this->Cil) // Pokud jsme se nedostali do cíle =>
    {
        while (true)
        {
            int q = mapa[i][j].xy.x;
            j = mapa[i][j].xy.y;
            i = q;
            if (Souradnice(i,j) == this->Start)
                break;
            cesta.append(i);
            cesta.append(j);
            prozkoumano[i][j] = false; // smažu V z E, aby se pak nevypisoval v prozkoumaném stavovém prostoru
        }
    }
    prozkoumano[Start.x][Start.y] = false;
    prozkoumano[Cil.x][Cil.y] = false;
    for (int t = 0; t < this->R; t++)
    {
        for (int u = 0; u < this->S; u++)
        {
            // v E jsou i nepristupne body, proto navíc používám podmínku, že body musí mít predchudce
            if (mapa[t][u].xy.x != -2 && mapa[t][u].xy.y != -2 && prozkoumano[t][u])
            {
                oblast.append(t);
                oblast.append(u);
            }
        }
    }
}

void AStar::aktualizujHranici(Souradnice bod, priority_queue<Prvek> & hranice)
{
    int i = bod.x;
    int j = bod.y;
    //Osmiokolí
    for (int m = i - 1; m <= i + 1; m++)
    {
        if (m < 0 || m >= R)
            continue;
        for (int n = j - 1; n <= j + 1; n++)
        {
            if (n < 0 || n >= S || prozkoumano[m][n] || (m == i && n == j))
                continue;
            int vzdalenost = (i == m || j == n) ? 10 : 14; // x-okolí (diagonála) vzdálenost 14 + okolí vzdálenost 10
            if (mapa[m][n].f > mapa[i][j].g + vzdalenost + mapa[m][n].h) // je-li nová cesta do vrcholu (m,n) kratší než ta stará
            { //=> vypocti nove hodnoty f,g
                mapa[m][n].g = mapa[i][j].g + vzdalenost;
                mapa[m][n].f = mapa[m][n].g + mapa[m][n].h;                
                mapa[m][n].xy = Souradnice(i, j); //novým predchudcem vrcholu (m,n) je (i,j)
                hranice.emplace(Prvek(Souradnice(m, n), mapa[m][n].f)); //vloží vrchol (m,n) do hranice
            }
        }
    }
}

void AStar::vypoctiHeurestiku()
{
    int k;
    int l;
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < S; j++)
        {
            if(astar)
            {
                l = (Cil.x > i) ? Cil.x - i : i - Cil.x;
                k = (Cil.y > j) ? Cil.y - j : j - Cil.y;
                mapa[i][j].h = (l+k) * 10;
            }
            else
                mapa[i][j].h = 0;
        }
    }
}

void AStar::inicializuj(priority_queue<Prvek> & hranice)
{
    //1)
    //prozkoumano = {}
    for (int k = 0; k < R; k++)
    {
        for (int l = 0; l < S; l++)
        {
            prozkoumano[k][l] = (bludiste[k][l] == true) ? 0 : 1;
        }
    }


    int x = Start.x;
    int y = Start.y;

    mapa[x][y].f = mapa[x][y].h;
    mapa[x][y].g = 0;

    // hranice = {s_0}
    hranice.emplace(Prvek(Souradnice(x,y), mapa[x][y].f));
}

void AStar::vyres(QVector<int>& cesta, QVector<int>& oblast)
{
    vypoctiHeurestiku();
    //1) inicializace
    priority_queue<Prvek> hranice;
    inicializuj(hranice);
    Souradnice bod;

    //2 opakuj
    while (!hranice.empty()) //2a) je-li F = {} => return ""
    {
        //2b,c) vybere vrchol V z hranice
        bod = hranice.top().getBod();
        hranice.pop();
        if (prozkoumano[bod.x][bod.y]) //byl-li jiz bod navstiven, existuje jiz kratsi cesta do bodu -> jdi na dalsi iteraci
            continue;
        //2d) E = E u {V}
        prozkoumano[bod.x][bod.y] = true;

        //2e) je-li V == t (cíl) => return cesta
        if (bod == Cil)
            break;
        aktualizujHranici(bod, hranice);
    }
    this->cesta(bod, cesta, oblast);
}
