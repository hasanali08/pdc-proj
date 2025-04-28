#include <iostream>
#include <vector>
#include <fstream>
#include <string>

class Graph {
private:
    std::vector<std::vector<int>> adjacencyList;
    int numVertices;

public:
    Graph(int vertices) : numVertices(vertices) {
        adjacencyList.resize(vertices);
    }

    void addEdge(int from, int to) {
        adjacencyList[from].push_back(to);
        adjacencyList[to].push_back(from);
    }

    void printGraph() {
        for (int i = 0; i < numVertices; i++) {
            std::cout << "Vertex " << i << " -> ";
            for (int neighbor : adjacencyList[i]) {
                std::cout << neighbor << " ";
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    // Create a simple graph
    Graph g(5);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 4);
    
    std::cout << "Basic Graph Implementation:" << std::endl;
    g.printGraph();
    
    return 0;
} 