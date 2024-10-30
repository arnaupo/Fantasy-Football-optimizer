#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <random>

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
    bool used;

    Player(int ident, const string &n, const string &pos, int pr, const string &c, int p, bool u = false) : id(ident), name(n), position(pos), price(pr), club(c), points(p), used(u) {}
};

// The solutions of the problem are individuals
class Individual
{
public:
    vector<Player> por;
    vector<Player> def;
    vector<Player> mig;
    vector<Player> dav;
    int price;
    int points;
    Individual(vector<Player> a, vector<Player> b, vector<Player> c, vector<Player> d, int e, int f) : por(a), def(b), mig(c), dav(d), price(e), points(f) {}
};

using population = vector<Individual>;
// parameters
uint N1;
uint N2;
uint N3;
int T;
int J;
vector<Player> por;
vector<Player> def;
vector<Player> mig;
vector<Player> dav;
double t0, t1;

Individual best_ind = Individual(vector<Player>(), vector<Player>(), vector<Player>(), vector<Player>(), 0, 0);

vector<Player> P;
int type;
int pop_size;
random_device rd;
mt19937 gen(rd());
int N;

/*
Compares two players according to a desired type
*/
bool sorting(const Player &p1, const Player &p2)
{
    if (p1.points != 0 && p2.points == 0)
        return true;
    if (p1.points == 0 && p2.points != 0)
        return false;
    if (type == 1)
        return p1.points * p1.points * double(p1.points) / double(p1.price) > p2.points * p2.points * (double(p2.points) / double(p2.price));

    else if (type == 2)
        return p1.points * double(p1.points) / double(p1.price) > p2.points * double(p2.points) / double(p2.price);
    else
        return double(p1.points) / double(p1.price) > double(p2.points) / double(p2.price);
}

/*
Compares to players by points
*/
bool ComparePoints(const Player &p1, const Player &p2)
{
    if (p1.points != 0 && p2.points == 0)
        return true;
    if (p1.points == 0 && p2.points != 0)
        return false;
    if (p1.used && not p2.used)
        return false;
    if (p2.used && not p1.used)
        return true;
    return p1.points > p2.points;
}

/*
Reads players data
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

        if (preu <= J and !(preu > 0 and punts == 0))
        {
            ++nextId;
            P.push_back(Player(nextId, nom, posicio, preu, club, punts));
        }
    }
    in.close();
}

/*
Writes the solution at the output file
*/
void write_sol(Individual &ind)
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
        file << ind.por[i].name << endl;
    }
    bool first = true;
    file << "DEF: ";
    for (uint j = 0; j < N1; ++j)
    {
        if (first)
        {
            file << ind.def[j].name;
            first = false;
        }
        else
            file << ';' << ind.def[j].name;
    }
    file << endl;
    first = true;
    file << "MIG: ";
    for (uint k = 0; k < N2; ++k)
    {
        if (first)
        {
            file << ind.mig[k].name;
            first = false;
        }
        else
            file << ';' << ind.mig[k].name;
    }
    file << endl;
    first = true;
    file << "DAV: ";
    for (uint l = 0; l < N3; ++l)
    {
        if (first)
        {
            file << ind.dav[l].name;
            first = false;
        }
        else
            file << ';' << ind.dav[l].name;
    }
    file << endl;
    file << "Punts: " << ind.points << endl;
    file << "Preu: " << ind.price << endl;
}

/*
Adds a Player to a list of players and updates de solution/individual
*/
void add(vector<Player> &f, const Player &p, int &points, int &price)
{
    f.push_back(p);
    price += p.price;
    points += p.points;
}

/*
Returns a parent with a greedy algorithm
*/
Individual find_parent()
{
    string pos;
    sort(P.begin(), P.end(), sorting);
    Individual ind = Individual(vector<Player>(), vector<Player>(), vector<Player>(), vector<Player>(), 0, 0);
    // The first 10 players of the individual are chosen following the order of the sorted list
    for (int i = 0; i < N; ++i)
    {
        if (ind.price + P[i].price <= T)
        {
            if (P[i].used)
                P[i].used = false;
            pos = P[i].position;
            if (pos == "por" && ind.por.size() < 1)
            {
                add(ind.por,P[i],ind.points,ind.price);
                P[i].used = true;
            }
            else if (pos == "def" && ind.def.size() < N1)
            {
                add(ind.def,P[i],ind.points,ind.price);
                P[i].used = true;
            }
            else if (pos == "mig" && ind.mig.size() < N2)
            {
                add(ind.mig,P[i],ind.points,ind.price);
                P[i].used = true;
            }
            else if (pos == "dav" && ind.dav.size() < N3)
            {
                add(ind.dav,P[i],ind.points,ind.price);
                P[i].used = true;
            }
        }
        if (ind.por.size() + ind.def.size() + ind.mig.size() + ind.dav.size() == 10)
            break;
    }
    /*The eleventh player to choose is the one that contributes the most points without exceeding the price - greedy -*/
    sort(P.begin(), P.end(), ComparePoints);
    for (int i = 0; i < N; ++i)
    {
        if (not P[i].used && ind.price + P[i].price <= T)
        {
            pos = P[i].position;
            if (pos == "por" && ind.por.size() == 0)
            {
                add(ind.por,P[i],ind.points,ind.price);
                break;
            }
            if (pos == "def" && ind.def.size() == N1 - 1)
            {
                add(ind.def,P[i],ind.points,ind.price);
                break;
            }
            if (pos == "mig" && ind.mig.size() == N2 - 1)
            {
                add(ind.mig,P[i],ind.points,ind.price);
                break;
            }
            if (pos == "dav" && ind.dav.size() == N3 - 1)
            {
                add(ind.dav,P[i],ind.points,ind.price);
                break;
            }
        }
    }
    ++type;
    return ind;
}

/*
Given a list of players of a specific position and a two partial solutions of two sons, chooses if the player of a parent goes to the list of
players of the correspoding son with probability 0.5 .If not, the son receives the player from the other parent.
*/
void crossover_position(vector<Player> &s1, vector<Player> &s2, const vector<Player> &p1, const vector<Player> &p2, int &points1, int &price1, int &points2, int &price2)
{
    uniform_int_distribution<int> uniform_distribution(1, 2);
    int random_number;
    for (uint i = 0; i < p1.size(); ++i)
    {
        random_number = uniform_distribution(gen);

        if (random_number == 1)
        {
            add(s1, p1[i], points1, price1);
            add(s2, p2[i], points2, price2);
        }
        else
        {
            add(s1, p2[i], points1, price1);
            add(s2, p1[i], points2, price2);
        }
    }
}

/*
Returns if a son (individual) does not violate the restrictions
*/
bool check(const Individual &s)
{
    if (s.price > T)
        return false;
    vector<int> ids = vector<int>();
    for (const Player &p : s.def)
        ids.push_back(p.id);

    for (const Player &p : s.mig)
        ids.push_back(p.id);
    for (const Player &p : s.dav)
        ids.push_back(p.id);

    sort(ids.begin(), ids.end());
    // It is validated if the same player is not used twice
    for (int i = 0; i < 9; ++i)
        if (ids[i] == ids[i + 1])
            return false;

    return true;
}

/*
Auxiliar function to mutate
*/
void mutate_position(vector<Player> &p, const Player &j, int &points, int &price)
{
    uniform_int_distribution<int> uniform_distribution(0, p.size() - 1);
    int random_number = uniform_distribution(gen);
    points -= p[random_number].points;
    price -= p[random_number].price;
    p[random_number] = j;
    points += j.points;
    price += j.price;
}

/*
mutates a individual
*/
void mutate(Individual &s)
{
    uniform_int_distribution<int> uniform_distribution(0, N - 1);
    int random_number = uniform_distribution(gen);
    string pos = P[random_number].position;
    if (pos == "por")
        mutate_position(s.por, P[random_number],s.points,s.price);
    else if (pos == "def")
        mutate_position(s.def, P[random_number],s.points,s.price);
    else if (pos == "mig")
        mutate_position(s.mig, P[random_number],s.points,s.price);
    else if (pos == "dav")
        mutate_position(s.dav, P[random_number],s.points,s.price);
}

/*
Crossovers the players between two parents to obtain two sons
*/
void crossover(const Individual &p1, const Individual &p2, population &fills)
{
    uniform_int_distribution<int> uniform_distribution(1, 100);
    int random_number;
    Individual s1 = Individual(vector<Player>(), vector<Player>(), vector<Player>(), vector<Player>(), 0, 0);
    Individual s2 = Individual(vector<Player>(), vector<Player>(), vector<Player>(), vector<Player>(), 0, 0);

    crossover_position(s1.por, s2.por, p1.por, p2.por, s1.points, s1.price, s2.points, s2.price);
    crossover_position(s1.def, s2.def, p1.def, p2.def, s1.points, s1.price, s2.points, s2.price);
    crossover_position(s1.mig, s2.mig, p1.mig, p2.mig, s1.points, s1.price, s2.points, s2.price);
    crossover_position(s1.dav, s2.dav, p1.dav, p2.dav, s1.points, s1.price, s2.points, s2.price);
    // It is validated whether the obtained sons violate the restrictions
    random_number = uniform_distribution(gen);
    if (random_number > 90)
        mutate(s1);
    if (random_number > 90)
        mutate(s2);

    if (check(s1))
    {
        fills.push_back(s1);
    }
    if (check(s2))
    {
        fills.push_back(s2);
        ++pop_size;
    }
}

/*
Increases the number of individuals of the population
*/
void expand(population &Parents)
{
    population sons = population();
    // each parent is crossovered with the others
    // from n parents 2*(n over 2) sons are obtained at maximum

    for (uint i = 0; i < Parents.size(); ++i)
        for (uint j = i + 1; j < Parents.size(); ++j)
        {
            if (pop_size > 524288)
                return; // If the grows too much the executing time increases exponentially
            crossover(Parents[i], Parents[j], sons);
        }
    for (const Individual &s : sons)
    {
        Parents.push_back(s);
        if (s.points > best_ind.points)
        {
            best_ind = s;
            write_sol(best_ind);
        };
    }
}

/*
Heuristic function based on genetic algorithms to find a suitable solution to the problem
*/
void gen_heuristic()
{
    N = P.size();
    population Population = population();
    // Three parents are found with a greedy algorithm so the best individual will be the same or better than the greedy solution.
    Individual P1 = find_parent();
    best_ind = P1;
    Individual P2 = find_parent();
    if (P2.points > best_ind.points)
        best_ind = P2;
    Individual P3 = find_parent();
    if (P3.points > best_ind.points)
        best_ind = P3;
    Population.push_back(P1);
    Population.push_back(P2);
    Population.push_back(P3);
    pop_size = 3;
    for (Individual& parent: Population){
        if (parent.points > best_ind.points) best_ind = parent;
    }
    write_sol(best_ind);
    /*
    The population is expanded as much as wanted
    */
    while (pop_size < 524288)
    {
        expand(Population);
    }
    
}
int main(int argc, char **argv)
{
    // read data
    read(argc, argv);
    gen_heuristic();
}