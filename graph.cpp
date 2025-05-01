#include "graph.h"
#include <iostream>
#include <fstream>

Graph::Graph(int vertices) : numVertices(vertices) {
    adjacencyList.resize(vertices);
}

void Graph::addEdge(int from, int to) {
    adjacencyList[from].push_back(to);
    adjacencyList[to].push_back(from);
}

void Graph::printGraph() const {
    for (int i = 0; i < numVertices; i++) {
        std::cout << "Vertex " << i << " -> ";
        for (int neighbor : adjacencyList[i]) {
            std::cout << neighbor << " ";
        }
        std::cout << std::endl;
    }
}

void Graph::saveToFile(const std::string& filename) const {
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

void Graph::loadFromFile(const std::string& filename) {
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

std::vector<int> Graph::bfs(int startVertex) const {
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

std::vector<int> Graph::dfs(int startVertex) const {
    std::vector<int> visited(numVertices, -1);
    std::stack<int> s;
    int currentTime = 0;
    
    s.push(startVertex);
    visited[startVertex] = currentTime++;
    
    while (!s.empty()) {
        int current = s.top();
        bool hasUnvisitedNeighbor = false;
        
        for (int neighbor : adjacencyList[current]) {
            if (visited[neighbor] == -1) {
                s.push(neighbor);
                visited[neighbor] = currentTime++;
                hasUnvisitedNeighbor = true;
                break;
            }
        }
        
        if (!hasUnvisitedNeighbor) {
            s.pop();
        }
    }
    
    return visited;
}

bool Graph::isConnected() const {
    if (numVertices == 0) return true;
    
    std::vector<int> distances = bfs(0);
    for (int dist : distances) {
        if (dist == -1) return false;
    }
    return true;
} 