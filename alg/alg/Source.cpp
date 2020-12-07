#include <stdio.h>
#include <algorithm>
#include <vector>
#include <queue>
#include <random>
#include <ctime>
#include <iostream>
#include <chrono>
#include <locale>

using namespace std;




class comma : public numpunct<char> {
public:
    comma() : numpunct<char>() {}
protected:
    char do_decimal_point() const {
        return ',';
    }
};

class Timer
{
public:
    Timer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const {
        return std::chrono::duration_cast<second_>
            (clock_::now() - beg_).count();
    }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> beg_;
};


struct Edge {
    int a, b, cap, flow;
    Edge(int a, int b, int cap) : a(a), b(b), cap(cap), flow(0) {}
    int other(int v) {
        return v == a ? b : a;
    }
    int capTo(int v) {
        return v == b ? cap - flow : flow;
    }
    void addFlowTo(int v, int f) {
        flow += (v == b ? f : -f);
    }
};
vector<Edge> edges;


int** gen_graf(int n) {

    int** matrix = new int* [n];
    for (int i = 0; i < n; ++i) {
        matrix[i] = new int[n];
        for (int j = 0; j < n; ++j) {
            if (i < j)
                matrix[i][j] = rand() % 100 + 1;
            else
                matrix[i][j] = 0;
        }
    }
    return matrix;
}

bool bfs(int v, int vTarget, int * u, int * p, vector<int>* g) {
    queue<int> q;
    u[v] = 1;
    q.push(v);
    while (!q.empty()) {
        v = q.front();
        q.pop();
        if (v == vTarget)
            return 1;
        for (int i = 0; i < g[v].size(); i++) {
            int e = g[v][i], to = edges[e].other(v);
            if (!u[to] && edges[e].capTo(to)) {
                p[to] = e;
                u[to] = u[v] + 1;
                q.push(to);
            }
        }
    }
    return 0;
}

bool dfs(int v, int vTarget, int * u, int * p, int* pos, vector<int>* g) {
    if (v == vTarget)
        return 1;
    for (; pos[v] < g[v].size(); pos[v]++) {
        int e = g[v][pos[v]], to = edges[e].other(v);
        if (u[v] + 1 == u[to] && edges[e].capTo(to)) {
            p[to] = e;
            if (dfs(to, vTarget, u, p, pos, g))
                return 1;
        }
    }
    return 0;
}

int main() {
    locale loccomma(cout.getloc(), new comma);
    cout.imbue(loccomma);
    srand(time(0));
    int n;
    printf("Ennter the number of vertices in the graph: ");
    scanf_s("%d", &n);

        int* u = new int[n * n];
        int* p = new int[n * n];
        int* pos = new int[n * n];


        vector<int>* g = new vector<int >[n * n];
        int** matrix = gen_graf(n);


        for (int i = 0; i < n; ++i)
            for (int j = n; j > i; j--) {
                edges.push_back(Edge(i, j, matrix[i][j]));
                g[i].push_back(edges.size() - 1);
                g[j].push_back(edges.size() - 1);
            }


        Timer tmr;
        double t = tmr.elapsed();

        int flow = 0;
        while (1) {
            fill(u, u + n, 0);
            if (!bfs(0, n - 1, u, p, g))
                break;
            fill(pos, pos + n, 0);
            while (dfs(0, n - 1, u, p, pos, g)) {
                int deltaFlow = 1 << 30;
                for (int v = n - 1; v != 0; v = edges[p[v]].other(v))
                    deltaFlow = min(deltaFlow, edges[p[v]].capTo(v));
                flow += deltaFlow;
                for (int v = n - 1; v != 0; v = edges[p[v]].other(v))
                    edges[p[v]].addFlowTo(v, deltaFlow);
            }
        }

        double t2 = tmr.elapsed();

        
        printf("Adjacency matrix: \n");
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j)
                printf("%d ", matrix[i][j]);
                cout << endl;
            }
        

        printf("Max flow: %d\nTime: %fs\n", flow, t2 - t);
    system("pause");
    return 0;
}