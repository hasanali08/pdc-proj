#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <queue>
#include <unordered_set>
#include <stack>

class Graph {
private:
    std::vector<std::vector<int>> adjacencyList;
    int numVertices;

public:
    Graph(int vertices = 0);
    void addEdge(int from, int to);
    void printGraph() const;
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
    std::vector<int> bfs(int startVertex) const;
    std::vector<int> dfs(int startVertex) const;
    bool isConnected() const;
    int getNumVertices() const { return numVertices; }
    const std::vector<int>& getNeighbors(int vertex) const { return adjacencyList[vertex]; }
};

#endif // GRAPH_H 