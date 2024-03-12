#include <iostream>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <ctime> 

using namespace std;
using namespace chrono;

const int NPREF = 2;
const int NHASH = 5;
const int MAXGEN = 10000;
const int MULTIPLIER = 31;

class Suffix;
class State;

class Suffix {
public:
    string word;
    Suffix *next;
};

class State {
public:
    string pref[NPREF];
    Suffix *suf;
    State *next;
};

State *statetab[NHASH];

unsigned int myhash(string s[NPREF]) {
    unsigned int h = 0;
    for (int i = 0; i < NPREF; i++)
        for (char c : s[i])
            h = MULTIPLIER * h + c;
    return h % NHASH;
}

State *lookup(string prefix[NPREF], int create) {
    int i;
    State *sp;
    int h = myhash(prefix);
    for (sp = statetab[h]; sp != NULL; sp = sp->next) {
        for (i = 0; i < NPREF; i++)
            if (prefix[i] != sp->pref[i])
                break;
        if (i == NPREF)
            return sp;
    }
    if (create) {
        sp = new State;
        if (sp == NULL) {
            cerr << "Memory allocation error." << endl;
            exit(1);
        }
        for (i = 0; i < NPREF; i++)
            sp->pref[i] = prefix[i];
        sp->suf = NULL;
        sp->next = statetab[h];
        statetab[h] = sp;
    }
    return sp;
}

void addsuffix(State *sp, string suffix) {
    Suffix *suf = new Suffix;
    if (suf == NULL) {
        cerr << "Memory allocation error." << endl;
        exit(1);
    }
    suf->word = suffix;
    suf->next = sp->suf;
    sp->suf = suf;
}

void add(string prefix[NPREF], string suffix) {
    State *sp = lookup(prefix, 1);
    addsuffix(sp, suffix);
    for (int i = 0; i < NPREF - 1; i++) {
        prefix[i] = prefix[i + 1];
    }
    prefix[NPREF - 1] = suffix;
}

void build(string prefix[NPREF], istream& in) {
    string buf;
    while (in >> buf) {
        cout << buf << " ";
        add(prefix, buf);
    }
    cout << endl;
}

void generate(int nwords) {
    string NONWORD = "\n";
    State *sp;
    Suffix *suf;
    string prefix[NPREF], w;
    int i, nmatch;
    for (i = 0; i < NPREF; i++)
        prefix[i] = NONWORD;
    for (i = 0; i < nwords; i++) {
        sp = lookup(prefix, 0);
        nmatch = 0;
        for (suf = sp->suf; suf != NULL; suf = suf->next)
            if (rand() % ++nmatch == 0)
                w = suf->word;
        if (w == NONWORD)
            break;
        cout << w << endl;
        for (int j = 0; j < NPREF - 1; j++) {
            prefix[j] = prefix[j + 1];
        }
        prefix[NPREF - 1] = w;
    }
}

int main() {
    auto start_time = high_resolution_clock::now();

    int i, nwords = MAXGEN;
    string prefix[NPREF];
    string NONWORD = "\n";
    for (i = 0; i < NPREF; i++)
        prefix[i] = NONWORD;
        
    FILE *file = fopen("brooks.txt", "r");
    if (file) {
        build(prefix, cin);
        fclose(file);
    }
    add(prefix, NONWORD);
    generate(nwords);

    auto end_time = high_resolution_clock::now();
    auto elapsed_time = duration_cast<microseconds>(end_time - start_time).count();
    cout << "Program execution time: " << elapsed_time << " microseconds" << endl;
    
    return 0;
}