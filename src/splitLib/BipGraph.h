#ifndef MYPOLYGON_BIPGRAPH_H
#define MYPOLYGON_BIPGRAPH_H


#include<bits/stdc++.h>

using namespace std;

#define NIL 0
#define INF INT_MAX

// A class to represent Bipartite graph for Hopcroft
// Karp implementation
class BipGraph
{
    // m and n are number of vertices on left
    // and right sides of Bipartite Graph
    int m, n;

    // adj[u] stores adjacents of left side
    // vertex 'u'. The value of u ranges from 1 to m.
    // 0 is used for dummy vertex
    list<int> *adj;

    // These are basically pointers to arrays needed
    // for hopcroftKarp()
    int *pairU, *pairV, *dist;

    int matched = false;

    list<int> *adj_v;
    bool *visited_u, *visited_v;

public:
    vector<int> U_ind, V_ind;

    BipGraph(int m, int n); // Constructor
    void addEdge(int u, int v); // To add edge

    // Returns true if there is an augmenting path
    bool bfs();

    // Adds augmenting path if there is one beginning
    // with u
    bool dfs(int u);

    // Returns size of maximum matcing
    int hopcroftKarp();

    void maximum_independent_set();

    void myDFS(int, bool, int);
};
#endif //MYPOLYGON_BIPGRAPH_H
