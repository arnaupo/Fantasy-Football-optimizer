#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <ctime>
#include <cmath>
using namespace std;

string outputFile;

class Player {
public:
    int    id;
    string name;
    string position;
    int    price;
    string club;
    int points;
    bool used;

    Player(int ident, const string& n, const string& pos, double pr, const string& c, double p,bool u=false):
        id(ident), name(n), position(pos), price(pr), club(c), points(p),used(u){}
};

class Individual {
public:
    vector<Player> por;
    vector<Player> def;
    vector<Player> mig;
    vector<Player> dav;
    int price;
    int points;
    Individual(vector<Player> a, vector<Player> b, vector<Player> c, vector<Player> d, int e, int f):
        por(a), def(b), mig(c), dav(d), price(e),points(f){}
};

using Population = vector<Individual>;

//parameters
uint N1; 
uint N2;
uint N3;
int T; //max total price
int J; //max individual price
vector<Player> P;
double t0,t1;


bool sorting1(const Player &p1, const Player &p2) {
    if (p1.points != 0 && p2.points == 0) return true;
    if (p1.points == 0 && p2.points != 0) return false;
    //return (p1.points)*log10(p1.points)*(log10(p2.price)-log2(p2.points)) > p2.points*log10(p2.points)*(log10(p1.price)-log2(p1.points));
    //return p1.points/exp(-double(p1.points)/double(p1.price)) > p2.points/exp(-double(p2.points)/double(p2.price));
    //return (p1.points*p1.points)*sqrt(double(p1.points)/double(p1.price)) > (p2.points*p2.points)*sqrt(double(p2.points)/double(p2.price));
    return p1.points*p1.points*double(p1.points)/double(p1.price) > p2.points*p2.points*(double(p2.points)/double(p2.price));

}

bool sorting2(const Player &p1, const Player &p2){
    if (p1.points != 0 && p2.points == 0) return true;
    if (p1.points == 0 && p2.points != 0) return false;
    if (p1.used && not p2.used) return false;
    if(p2.used && not p1.used) return true;
    return p1.points > p2.points;
}


void read(int argc, char** argv){
    /*
    Reads all data from player database and stores it.
    */

    outputFile = argv[3];
    //read query
    ifstream in2(argv[2]);
    while (not in2.eof()) {
        in2 >> N1 >> N2 >> N3 >> T >> J;
    }
    in2.close();
    
    ifstream in(argv[1]);
    int nextId = 0;
    while (not in.eof()) {
        string nom, posicio, club;
        double punts, preu;
        getline(in,nom,';');    if (nom == "") break;
        getline(in,posicio,';');
        in >> preu;
        char aux; in >> aux;
        getline(in,club,';');
        in >> punts;
        string aux2;
        getline(in,aux2);
        
        if (preu <= J and !(preu > 0 and punts == 0)){
            P.push_back(Player(nextId++, nom, posicio, preu, club, punts));
    }
    
    }
    in.close();
    sort(P.begin(),P.end(),sorting1);
}



void write_sol(Individual& sol) {
    /*
    Writes the solution to the output file.
    */
    t1 = clock();
    double ex_t = (t1 - t0)/CLOCKS_PER_SEC;
    ofstream file;
    file.open(outputFile.c_str(), fstream::out);
    file.setf(ios::fixed);
    file.setf(ios::showpoint);
    file.precision(1);
    file << ex_t << endl;
    file << "POR: ";
    for (int i = 0; i < 1; ++i) {
        file << sol.por[i].name << endl;
    }
    bool first = true;
    file << "DEF: ";
    for (uint j = 0; j < N1; ++j) {
        if (first){
        file << sol.def[j].name; 
        first = false;
        }
        else file << ';' << sol.def[j].name;
    }
    file << endl;
    first = true;
    file << "MIG: ";
    for (uint k = 0; k < N2; ++k) {
        if (first){
        file << sol.mig[k].name; 
        first = false;
        }
        else file << ';' << sol.mig[k].name;
    }
    file << endl;
    first = true;
    file << "DAV: ";
    for (uint l = 0; l < N3; ++l) {
        if (first){
        file << sol.dav[l].name; 
        first = false;
        }
        else file << ';' << sol.dav[l].name;
    }
    file << endl;
    file << "Punts: " << sol.points << endl;
    file << "Preu: " << sol.price << endl;

}


void greedy(){

    Individual sol = Individual(vector<Player>(), vector<Player>(), vector<Player>(), vector<Player>(),0,0);
    for (uint i = 0; i < P.size(); ++i){
        if (sol.price + P[i].price <= T){
            if (P[i].position == "por" && sol.por.size() < 1){
                sol.por.push_back(P[i]);
                sol.points += P[i].points;
                sol.price += P[i].price;
                P[i].used = true;
            }
            else if (P[i].position == "def" && sol.def.size() < N1){
                sol.def.push_back(P[i]);
                sol.points += P[i].points;
                sol.price += P[i].price;
                P[i].used = true;
            }
            else if (P[i].position == "mig" && sol.mig.size() < N2){
                sol.mig.push_back(P[i]);
                sol.points += P[i].points;
                sol.price += P[i].price;
                P[i].used = true;
            }
            else if (P[i].position == "dav" && sol.dav.size() < N3){
                sol.dav.push_back(P[i]);
                sol.points += P[i].points;
                sol.price += P[i].price;
                P[i].used = true;
            }
        }
        if (sol.por.size() + sol.def.size() + sol.mig.size() + sol.dav.size() == 10) break;  
    }
    sort(P.begin(),P.end(),sorting2);
    for (uint i = 0; i < P.size();++i){
        if (not P[i].used && sol.price + P[i].price <= T){
            if (P[i].position == "por" && sol.por.size() == 0){
                sol.por.push_back(P[i]);
                sol.points+=P[i].points;
                sol.price+=P[i].price;
                break;
            }
            if (P[i].position == "def" && sol.def.size() == N1-1){
                sol.def.push_back(P[i]);
                sol.points+=P[i].points;
                sol.price+=P[i].price;
                break;
            }
            if (P[i].position == "mig" && sol.mig.size() == N2-1){
                sol.mig.push_back(P[i]);
                sol.points+=P[i].points;
                sol.price+=P[i].price;
                break;
            }
            if (P[i].position == "dav" && sol.dav.size() == N3-1){
                sol.dav.push_back(P[i]);
                sol.points+=P[i].points;
                sol.price+=P[i].price;
                break;
            }
        }
    }
    write_sol(sol);
    return;
}

int main(int argc, char** argv) {
    t0 = clock();
    read(argc, argv);
    greedy();

    
}