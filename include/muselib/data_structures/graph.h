#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <deque>


// Data structure to store a graph edge
struct Edge {
    int src, dest;
};

// A class to represent a graph object
class Graph
{
public:
    // a vector of vectors to represent an adjacency list
    std::vector<std::vector<int>> adjList;

    // Graph Constructor
    Graph(std::vector<Edge> const &edges, int n)
    {
        // resize the vector to hold `n` elements of type `vector<int>`
        adjList.resize(n);

        // add edges to the directed graph
        for (auto &edge: edges)
        {
            // insert at the end
            adjList[edge.src].push_back(edge.dest);

            // uncomment the following code for undirected graph
            // adjList[edge.dest].push_back(edge.src);
        }
    }

    void printGraph(Graph const &graph, int n, std::deque<std::string> deps);
    void printGraph2(Graph const &graph, int n, std::deque<std::string> deps, std::deque<int> level);


    void printGraph2(Graph const &graph, int n, std::deque<std::string> &deps);
    void printGraph2(Graph const &graph, int n, std::deque<std::string> &deps, std::deque<std::string> &com);

    void printGraph2_old(Graph const &graph, int n, std::deque<std::string> &deps, std::deque<std::string> &com);

    void printGraph2_old(Graph const &graph, int n, std::deque<std::string> &deps);
};

#ifndef STATIC_MUSELIB
#include "graph.cpp"
#endif

#endif // GRAPH_H
