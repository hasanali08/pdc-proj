#include "graph.h"
#include <iostream>

void printDistances(const std::vector<int>& distances, const std::string& algorithm) {
    std::cout << "\n" << algorithm << " Results:" << std::endl;
    for (int i = 0; i < distances.size(); i++) {
        if (distances[i] != -1) {
            std::cout << "Vertex " << i << ": " << distances[i] << std::endl;
        } else {
            std::cout << "Vertex " << i << ": Not reachable" << std::endl;
        }
    }
}

int main() {
    // Create a more complex graph
    Graph g(8);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 4);
    g.addEdge(3, 5);
    g.addEdge(4, 6);
    g.addEdge(5, 7);
    g.addEdge(6, 7);
    
    std::cout << "Graph Structure:" << std::endl;
    g.printGraph();
    
    // Save and load the graph
    g.saveToFile("complex_graph.txt");
    Graph g2(0);
    g2.loadFromFile("complex_graph.txt");
    
    std::cout << "\nLoaded Graph:" << std::endl;
    g2.printGraph();
    
    // Test connectivity
    std::cout << "\nGraph is " << (g2.isConnected() ? "connected" : "not connected") << std::endl;
    
    // Perform BFS
    std::vector<int> bfsDistances = g2.bfs(0);
    printDistances(bfsDistances, "BFS");
    
    // Perform DFS
    std::vector<int> dfsDistances = g2.dfs(0);
    printDistances(dfsDistances, "DFS");
    
    return 0;
} 