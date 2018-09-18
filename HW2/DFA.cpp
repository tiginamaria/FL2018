#include <bits/stdc++.h>

using namespace std;
const int N = 1e3;
const int M = 1e4;

struct DFA {
    int Qsize, Ssize, Fsize;
    vector<string> Q; map<string, int> codeQ;  //states
    vector<string> S; map<string, int> codeS;  //symbols
    vector<string> F; set<int> codeF;          //accept states
    string q0;                              //start state
    int T[N][M];                     //transition function T[q1][s] = q2
    vector<int> G[N][M];                    //transition function G[q2][s] = {q1, q3}
    bool R[N];                              //reachable states
};

DFA a;                                      //deterministic finite automaton
DFA ma;                                     //minimalized deterministic finite automaton

void readDFA(string file) {
    std::ifstream in;
    in.open(file);

    in >> a.Qsize >> a.Ssize >> a.q0 >> a.Fsize;

    string state;
    for (int i = 0; i < a.Qsize; ++i) {
        in >> state;
        a.Q.push_back(state);
        a.codeQ[state] = i;
    }

    string symbol;
    for (int i = 0; i < a.Ssize; ++i) {
        in >> symbol;
        a.S.push_back(symbol);
        a.codeS[symbol] = i;
    }

    for (int i = 0; i < a.Fsize; ++i) {
        in >> state;
        a.F.push_back(state);
        a.codeF.insert(a.codeQ[state]);
    }

    string q;
    for(int i = 0; i < a.Qsize; ++i)
        for(int k = 0; k < a.Ssize; ++k) {
            in >> q; int j = a.codeQ[q];
            a.T[i][k] = j;
            a.G[j][k].push_back(i);
        }
    in.close();
}

void writeDFA(string file, DFA &ma) {
    ofstream out;
    out.open(file);

    out << ma.Qsize << ' ' << ma.Ssize << ' ' << ma.q0 << ' ' << ma.Fsize;
    out << '\n';

    for (auto q : ma.Q)
        out << q << ' ';
    out << '\n';

    for (auto s : ma.S)
        out << s << ' ';
    out << '\n';

    for (auto f : ma.F)
        out << f << ' ';
    out << '\n';

    for(int i = 0; i < ma.Qsize; ++i) {
        for(int k = 0; k < ma.Ssize; ++k)
            out << ma.T[i][k] << ' ';
        out << '\n';
    }
    out.close();
}

void markReachable(int i) {
    a.R[i] = 1;
    for(int k = 0; k < a.Ssize; ++k)
        if(!a.R[a.T[i][k]])
            markReachable(a.T[i][k]);
}

string str(int x) {
    string s = "";
    if(!x)
        return "0";
    while (x) {
        s += char(x % 10 + '0');
        x /= 10;
    }
    return s;
}

void minimizationDFA() {

    markReachable(a.codeQ[a.q0]);
    queue<pair<int, int>> q;
    bool used[N][N];

    for(int i = 0; i < a.Qsize; ++i)
        for(int j = 0; j < a.Qsize; ++j)
            used[i][j] = 0;

    for(auto f : a.F)
        for(int j= 0; j < a.Qsize; ++j) {
            int i = a.codeQ[f];
            if (a.codeF.find(j) == a.codeF.end()) {
                q.push({i, j});
                used[i][j] = used[j][i] = 1;
            }
        }

    while(!q.empty()) {
        pair<int, int> p = q.front();
        q.pop();
        for(int k = 0; k < a.Ssize; ++k)
            for(int i : a.G[p.first][k])
                for(int j : a.G[p.second][k])
                    if(!used[i][j]) {
                        used[i][j] = used[j][i] = 1;
                        q.push({i, j});
                    }
    }


    int c = 0;
    vector<int> num(N, -1);
    vector<int> comp[N];
    for(int i = 0; i < a.Qsize; ++i)
        if(a.R[i] && num[i] == -1) {
            for(int j = 0; j < a.Qsize; ++j)
                if (a.R[j] && !used[i][j])
                    num[j] = c, comp[c].push_back(j);
            ++c;
        }

    ma.Qsize = c;
    for(int i = 0; i < ma.Qsize; ++i) {
        ma.Q.push_back(str(i));
        ma.codeQ[str(i)] = i;
        for (auto j : comp[i]) {
            if (a.codeF.find(j) != a.codeF.end())
                ma.codeF.insert(i);
            if (j == a.codeQ[a.q0])
                ma.q0 = str(i);
            for (int k = 0; k < a.Ssize; ++k)
                ma.T[i][k] = num[a.T[j][k]];
        }
    }
    ma.Fsize = ma.codeF.size();
    for(auto f : ma.codeF)
        ma.F.push_back(str(f));

    ma.Ssize = a.Ssize;
    for(auto s : a.S)
        ma.S.push_back(s);
}

void writeDot(string file, DFA &ma) {
    ofstream out(file);
    string double_circle = "node [shape = doublecircle]";
    string circle = "node [shape = circle];";
    string l = "[label =";

    out << "digraph {\n";
    out << "node [shape = none]; \"\";";
    for (auto q : ma.Q) {
        if (ma.codeF.find(ma.codeQ[q]) == ma.codeF.end())
            out << circle << q << ";\n";
        else
            out << double_circle << q << ";\n";
    }
    out << "\"\" -> " << ma.q0 << ";";
    for (int i = 0; i < ma.Qsize; ++i) {
        for (int k = 0; k < ma.Ssize; ++k) {
            out << "  " << ma.Q[i] << " -> " << ma.Q[ma.T[i][k]] << l << ma.S[k] << "] ;\n";
        }
    }
    out << "}\n";
    out.close();
}

int main(int, char* argv[]) {
    string fin = argv[1];
    readDFA(fin);
    writeDot("DFA.dot", a);
    system("dot DFA.dot -Tpng -oDFA.png");
    minimizationDFA();
    string fout = "DFAinfo";

    if(a.Qsize == ma.Qsize)
        writeDFA(fout, a), writeDot("minDFA.dot", a);
    else
        writeDFA(fout, ma), writeDot("minDFA.dot", ma);

    system("dot minDFA.dot -Tpng -ominDFA.png");
    return 0;
}

