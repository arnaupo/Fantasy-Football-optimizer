#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <cmath>
using namespace std;


string outputFile;

class Player
{
public:
    int id;
    string name;
    string position;
    int price;
    string club;
    int points;

    Player(int ident, const string &n, const string &pos, int pr, const string &c, int p) : id(ident), name(n), position(pos), price(pr), club(c), points(p) {}
};

class Individual
{
public:
    vector<Player> por;
    int ppor=0;
    int prpor=0;
    vector<Player> def;
    int pdef=0;
    int prdef=0;
    vector<Player> mig;
    int pmig=0;
    int prmig=0;
    vector<Player> dav;
    int pdav=0;
    int prdav=0;
    int price;
    int points;
    Individual(vector<Player> a, vector<Player> b, vector<Player> c, vector<Player> d, int e, int f) : por(a), def(b), mig(c), dav(d), price(e), points(f) {}
    bool operator<(const Individual &other) const
        {
            //more points in a subformation with less price
            if (ppor >= other.ppor && prpor < other.prpor) return true;
            if (pdef >= other.pdef && prdef < other.prdef) return true;
            if (pmig >= other.pmig && prmig < other.prmig) return true;
            if (pdav >= other.pdav && prdav < other.prdav) return true;
            return false;
        }
};


// parameters
int N0 = 1;
int N1;
int N2;
int N3;
int T; // total budget
int J; // max budget per player
vector<Player> P;
vector<int> pointlog;
int por = 0;
int def = 0;
int mig = 0;
int dav = 0;
double t0, t1;

Individual best_ind = Individual(vector<Player>(), vector<Player>(), vector<Player>(), vector<Player>(), 0, 0);


bool sorting(const Player &p1, const Player &p2)
{

    if (p1.points != 0 && p2.points == 0)
        return true;
    if (p1.points == 0 && p2.points != 0)
        return false;
    return (p1.points)*sqrt(p1.points)/log10(p1.points)*sqrt(double(p1.points)/double(p1.price)) > (p2.points)*sqrt(p2.points)/log10(p2.points)*sqrt(double(p2.points)/double(p2.price));
}

bool sorting2(const int &a, const int &b)
{
    return a > b;
}

/*
Reads the players data
*/
void read(int argc, char **argv)
{

    outputFile = argv[3];
    // read query
    ifstream in2(argv[2]);
    while (not in2.eof())
    {
        in2 >> N1 >> N2 >> N3 >> T >> J;
    }
    in2.close();

    ifstream in(argv[1]);
    int nextId = 0;
    while (not in.eof())
    {
        string nom, posicio, club;
        int punts, preu;
        getline(in, nom, ';');
        if (nom == "")
            break;
        getline(in, posicio, ';');
        in >> preu;
        char aux;
        in >> aux;
        getline(in, club, ';');
        in >> punts;
        string aux2;
        getline(in, aux2);

        if (preu <= J and !(preu > 0 and punts == 0))//valid players (players with no points and price are discarded, thus fakeplayers are a better option)
        {
            P.push_back(Player(nextId++, nom, posicio, preu, club, punts));
            pointlog.push_back(punts);
            if (posicio == "por") ++por;
            if (posicio == "def") ++def;
            if (posicio == "mig") ++mig;
            if (posicio == "dav") ++dav;
        }
    }
    in.close();
    sort(P.begin(), P.end(), sorting);
    sort(pointlog.begin(), pointlog.end(), sorting2);
}

/*
Adds a new player to the solution
*/
void add(int &points, int &price, const int &idx, vector<Player> &p, int &n, int& subpoints, int& subprice)
{
    price += P[idx].price; subprice += P[idx].price;
    points += P[idx].points; subpoints += P[idx].points;
    p.push_back(P[idx]);
    --n;
}

/*
Deletes the last player of the solution
*/
void del(int &points, int &price, int &idx, vector<Player> &p, int &n, int& subpoints, int& subprice)
{
    --idx;
    ++n;
    p.pop_back();
    price -= P[idx].price; subprice -= P[idx].price;
    points -= P[idx].points; subpoints -= P[idx].points;
}

/*
Writes the solution at the output file
*/
void write_sol(Individual &sol)
{
    t1 = clock();
    double ex_t = (t1 - t0) / CLOCKS_PER_SEC;
    ofstream file;
    file.open(outputFile.c_str(), fstream::out);
    file.setf(ios::fixed);
    file.setf(ios::showpoint);
    file.precision(1);
    file << ex_t << endl;
    file << "POR: ";
    for (int i = 0; i < 1; ++i)
    {
        file << sol.por[i].name << endl;
    }
    bool first = true;
    file << "DEF: ";
    for (uint j = 0; j < sol.def.size(); ++j)
    {
        if (first)
        {
            file << sol.def[j].name;
            first = false;
        }
        else
            file << ';' << sol.def[j].name;
    }
    file << endl;
    first = true;
    file << "MIG: ";
    for (uint k = 0; k < sol.mig.size(); ++k)
    {
        if (first)
        {
            file << sol.mig[k].name;
            first = false;
        }
        else
            file << ';' << sol.mig[k].name;
    }
    file << endl;
    first = true;
    file << "DAV: ";
    for (uint l = 0; l < sol.dav.size(); ++l)
    {
        if (first)
        {
            file << sol.dav[l].name;
            first = false;
        }
        else
            file << ';' << sol.dav[l].name;
    }
    file << endl;
    file << "Punts: " << sol.points << endl;
    file << "Preu: " << sol.price << endl;
}

/*
Recursive function to find the best solution.
*/
void find_rec(Individual &sol, int idx, int i, int j, int k, int l)
{

    if (N0 == 0 && N1 == 0 && N2 == 0 && N3 == 0 && sol.points > best_ind.points)
    {
        best_ind = sol;
        write_sol(best_ind);
        return;
    }

    if (best_ind < sol) return;

    if (unsigned(sol.points) + pointlog[idx-1] * (N0 + N1 + N2 + N3) * 1.1 < unsigned(best_ind.points))
        return;

    else if (P[idx].position == "por" && N0 == 1)
    {
        if (por - i < N0)
            return;
        if (sol.price + P[idx].price <= T)
        {
            add(sol.points, sol.price, idx, sol.por, N0, sol.ppor, sol.prpor);
            find_rec(sol, ++idx, ++i, j, k, l);
            del(sol.points, sol.price, idx, sol.por, N0, sol.ppor, sol.prpor);
        }
    }
    else if (P[idx].position == "def" && N1 > 0)
    {
        if (def - j < N1)
            return;
        if (sol.price + P[idx].price <= T)
        {
            add(sol.points, sol.price, idx, sol.def, N1, sol.pdef, sol.prdef);
            find_rec(sol, ++idx, i, ++j, k, l);
            del(sol.points, sol.price, idx, sol.def, N1, sol.pdef, sol.prdef);
        }
    }
    else if (P[idx].position == "mig" && N2 > 0)
    {
        if (mig - k < N2)
            return;
        if (sol.price + P[idx].price <= T)
        {
            add(sol.points, sol.price, idx, sol.mig, N2, sol.pmig, sol.prmig);
            find_rec(sol, ++idx, i, j, ++k, l);
            del(sol.points, sol.price, idx, sol.mig, N2, sol.pmig, sol.prmig);
        }
    }
    else if (P[idx].position == "dav" && N3 > 0)
    {
        if (dav - l < N3)
            return;
        if (sol.price + P[idx].price <= T)
        {
            add(sol.points, sol.price, idx, sol.dav, N3, sol.pdav, sol.prdav);
            find_rec(sol, ++idx, i, j, k, ++l);
            del(sol.points, sol.price, idx, sol.dav, N3, sol.pdav, sol.prdav);
        }
    }
    if (unsigned(idx) < P.size())
        find_rec(sol, ++idx, i, j, k, l);
}




void find()
{

    Individual solucio = Individual(vector<Player>(), vector<Player>(), vector<Player>(), vector<Player>(), 0, 0);
    find_rec(solucio, 0, 0, 0, 0, 0);
}

int main(int argc, char **argv)
{
    // read data
    read(argc, argv);
    find();
}