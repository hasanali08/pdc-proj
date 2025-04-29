#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <queue>
#include <unordered_set>

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

    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << numVertices << std::endl;
            for (int i = 0; i < numVertices; i++) {
                for (int neighbor : adjacencyList[i]) {
                    if (i < neighbor) {  // Avoid duplicate edges
                        file << i << " " << neighbor << std::endl;
                    }
                }
            }
            file.close();
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (file.is_open()) {
            file >> numVertices;
            adjacencyList.clear();
            adjacencyList.resize(numVertices);
            
            int from, to;
            while (file >> from >> to) {
                addEdge(from, to);
            }
            file.close();
        }
    }

    std::vector<int> bfs(int startVertex) {
        std::vector<int> distances(numVertices, -1);
        std::queue<int> q;
        std::unordered_set<int> visited;
        
        distances[startVertex] = 0;
        q.push(startVertex);
        visited.insert(startVertex);
        
        while (!q.empty()) {
            int current = q.front();
            q.pop();
            
            for (int neighbor : adjacencyList[current]) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    distances[neighbor] = distances[current] + 1;
                    q.push(neighbor);
                }
            }
        }
        
        return distances;
    }
};

int main() {
    // Create a graph
    Graph g(6);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 4);
    g.addEdge(3, 5);
    
    std::cout << "Enhanced Graph Implementation:" << std::endl;
    g.printGraph();
    
    // Save graph to file
    g.saveToFile("graph.txt");
    
    // Load graph from file
    Graph g2(0);
    g2.loadFromFile("graph.txt");
    
    std::cout << "\nLoaded Graph:" << std::endl;
    g2.printGraph();
    
    // Perform BFS
    std::vector<int> distances = g2.bfs(0);
    std::cout << "\nBFS Distances from vertex 0:" << std::endl;
    for (int i = 0; i < distances.size(); i++) {
        std::cout << "Distance to " << i << ": " << distances[i] << std::endl;
    }
    
    return 0;
} 