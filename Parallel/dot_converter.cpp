#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

namespace fs = std::filesystem;

struct Edge {
    std::string from;
    std::string to;
};

void formatAndConvertDot(const std::string& dotFile) {
    // Extract the tree number from the filename
    std::string filename = fs::path(dotFile).filename().string();
    std::string treeNum = filename.substr(5, filename.length() - 9);
    
    // Read the DOT file
    std::ifstream inFile(dotFile);
    if (!inFile.is_open()) {
        std::cerr << "Error opening file: " << dotFile << std::endl;
        return;
    }

    // Parse the graph
    std::map<std::string, std::set<std::string>> edges;
    std::string line;
    bool inGraph = false;
    
    while (std::getline(inFile, line)) {
        if (line.find("digraph") != std::string::npos) {
            inGraph = true;
            continue;
        }
        
        if (inGraph && line.find("->") != std::string::npos) {
            // Parse edge
            size_t arrowPos = line.find("->");
            std::string from = line.substr(0, arrowPos);
            std::string to = line.substr(arrowPos + 2);
            
            // Clean up strings
            from.erase(std::remove_if(from.begin(), from.end(), isspace), from.end());
            to.erase(std::remove_if(to.begin(), to.end(), isspace), to.end());
            
            // Remove quotes and braces
            from.erase(std::remove(from.begin(), from.end(), '"'), from.end());
            to.erase(std::remove(to.begin(), to.end(), '"'), to.end());
            to.erase(std::remove(to.begin(), to.end(), '{'), to.end());
            to.erase(std::remove(to.begin(), to.end(), '}'), to.end());
            
            // Split multiple destinations
            std::stringstream ss(to);
            std::string dest;
            while (std::getline(ss, dest, ' ')) {
                if (!dest.empty()) {
                    edges[from].insert(dest);
                }
            }
        }
    }
    inFile.close();

    // Create formatted output
    std::string tempFile = "temp_" + filename;
    std::ofstream outFile(tempFile);
    
    // Write header
    outFile << "digraph Tree" << treeNum << " {\n";
    outFile << "    rankdir = TB;\n";
    outFile << "    graph [splines=true, nodesep=0.5, ranksep=0.8];\n";
    outFile << "    node [shape=rectangle, style=filled, fillcolor=lightgray, fontname=\"Helvetica\"];\n";
    outFile << "    edge [arrowhead=vee];\n\n";

    // Group edges by source node
    std::map<std::string, std::vector<std::string>> groupedEdges;
    for (const auto& [from, toSet] : edges) {
        std::stringstream ss;
        ss << "\"" << from << "\" -> {";
        bool first = true;
        for (const auto& to : toSet) {
            if (!first) ss << " ";
            ss << "\"" << to << "\"";
            first = false;
        }
        ss << "};";
        groupedEdges[from].push_back(ss.str());
    }

    // Write edges in groups
    for (const auto& [from, edgeList] : groupedEdges) {
        for (const auto& edge : edgeList) {
            outFile << "    " << edge << "\n";
        }
        outFile << "\n";
    }

    outFile << "}\n";
    outFile.close();

    // Convert to PNG using dot
    std::string pngFile = dotFile.substr(0, dotFile.length() - 4) + ".png";
    std::string command = "dot -Tpng " + tempFile + " -o " + pngFile;
    
    if (system(command.c_str()) != 0) {
        std::cerr << "Error converting " << dotFile << " to PNG" << std::endl;
    }

    // Clean up temporary file
    fs::remove(tempFile);
}

int main() {
    // Get current directory
    std::string currentDir = fs::current_path().string();
    
    // Find all .dot files in the current directory
    for (const auto& entry : fs::directory_iterator(currentDir)) {
        if (entry.path().extension() == ".dot") {
            std::cout << "Converting " << entry.path().filename() << " to PNG..." << std::endl;
            formatAndConvertDot(entry.path().string());
        }
    }
    
    std::cout << "Conversion complete!" << std::endl;
    return 0;
} 