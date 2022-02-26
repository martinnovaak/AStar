#ifndef ASTAR_H
#define ASTAR_H
#include <QString>
#include <queue>
using namespace std;

struct Souradnice
{
    int x; // x-ová souradnice
    int y; // y-ová souradnice

    Souradnice() { this->x = -2; this->y = -2; };
    Souradnice(int x, int y) { this->x = x; this->y = y; };
    bool operator==(Souradnice & neco) {return (this->x == neco.x && this->y == neco.y);};
};

struct Pole
{
    Souradnice xy; // souradnice predchudce (Souradnice pole z ktereho vede do aktualniho pole nejkratsi cesta)
    int f, g, h; // f = g + h ;

    Pole() { this->xy = Souradnice(); this->f = INT_MAX; this->g = INT_MAX; this->h = INT_MAX; };
    Pole(Souradnice xy, int f, int g, int h) { this->xy = xy; this->f = f; this->g = g; this->h = h; };
};

//Prvek, který bude vkládán do fronty - rozdíl s Polem je ten, že pole obsahuje souradnici predka, kdežto Prvek má vlastní souradnici
class Prvek
{
private:
    Souradnice xy; // souradnice bodu
    int f;
    //friend bool operator<(const Prvek & p1, const Prvek & p2){ return !(p1.getF() < p2.getF());}; // operator který bude použit jako compare v priority_queue
    // vrací opak protože prioritní fronta řadí od největšího do nejmenšího (já potřebuju na top nejmenší prvek)
public:
    Souradnice getBod() const { return this->xy;}; // getter na souradnici bodu
    unsigned int getF() const {return this->f;} ; // getter na f
    Prvek(Souradnice xy, unsigned int f): xy(xy), f(f) {}; // konstruktor
};

//operator pro prioritni frontu
inline bool operator<(const Prvek & p1, const Prvek & p2){ return !(p1.getF() < p2.getF());}

class AStar
{
private:
    int R; // Počet řádků bludiště
    int S; // Počet sloupců bludiště

    bool ** bludiste;    // Matice reprezentujici bludiste - je false pokud se jedna o prekazku jinak true
    bool ** prozkoumano; // Množina prozkoumaných polí - true, byl-li jiz prvek prozkouman
    Pole ** mapa;

    bool astar;         // jedna-li se o astar -> true , Dijkstra -> false

    Souradnice Start;   // souradnice startu
    Souradnice Cil;     // souradnice cile

    void aktualizujHranici(Souradnice bod, priority_queue<Prvek> & hranice);
    void vypoctiHeurestiku();                          // vypočte heurestiku h
    void inicializuj(priority_queue<Prvek> & hranice); // inicializuje hranici a prozkoumanou oblast
public:
    AStar(int R, int S, bool astar);
    ~AStar();

    bool & operator()(unsigned int i, unsigned int j);
    void setStart(unsigned int i, unsigned int j){this->Start.x = i; this->Start.y = j;}; //setter na start
    void setCil(unsigned int i, unsigned int j){this->Cil.x = i; this->Cil.y = j;};       //setter na cil

    void vyres(QVector<int>& cesta, QVector<int>& oblast); // A* algoritmus
    void cesta(Souradnice bod, QVector<int>& cesta, QVector<int>& oblast); // nacte nejkratší cestu z polí
};



#endif // ASTAR_H
