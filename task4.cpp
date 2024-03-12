#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <ctime> 
#include <list>  

using namespace std;
using namespace chrono;


typedef vector<string> Prefix;
map<Prefix, list<string>> statetab;


enum {
    NPREF = 2, 
    NHASH = 5, 
    MAXGEN = 10000 
};

const char NONWORD[] = "\n"; 

void add(Prefix& prefix, const string& s) {
    if (prefix.size() == NPREF) {
        statetab[prefix].push_back(s);
        prefix.erase(prefix.begin());
    }
    prefix.push_back(s);
}

void build(Prefix& prefix, istream& in) {
    string buf;
    while (in >> buf) {
        add(prefix, buf);
    }
    add(prefix, NONWORD); 
}

void generate(int nwords) {
    Prefix prefix;
    int i;
    for (i = 0; i < NPREF; i++) { 
        add(prefix, NONWORD);
    }
    for (i = 0; i < nwords; i++) {
        auto it = statetab.find(prefix);
        if (it == statetab.end()) {
            break; 
        }
        list<string>& suf = it->second;
        if (suf.empty()) {
            break; 
        }
        auto r = rand() % suf.size();
        auto suf_it = suf.begin();
        advance(suf_it, r); 
        const string& w = *suf_it;
        if (w == NONWORD) {
            break; 
        }
        cout << w << "\n";
        prefix.erase(prefix.begin()); 
        prefix.push_back(w);
    }
}

int main() {
    auto start_time = high_resolution_clock::now();

    int nwords = MAXGEN;
    Prefix prefix;
    for (int i = 0; i < NPREF; i++) {
        add(prefix, NONWORD);
    }
    build(prefix, cin);
    generate(nwords);

    auto end_time = high_resolution_clock::now();
    auto elapsed_time = duration_cast<microseconds>(end_time - start_time).count();
    cout << "Program execution time: " << elapsed_time << " microseconds" << endl;

    return 0;
}