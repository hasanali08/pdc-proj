//g++ -O3 -std=c++17 main.cpp tree_builder.cpp permutation_utils.cpp -o serial_tree_builder
//./serial_tree_builder.exe 3  

#include "tree_builder.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <n>\n";
        return 1;
    }

    int n = std::stoi(argv[1]);
    if (n < 2 || n > 10) {
        std::cerr << "n must be 2..10\n";
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    
    TreeBuilder builder(n);
    auto init_time = std::chrono::high_resolution_clock::now();

    // Build all trees
    auto trees = builder.buildTrees();
    auto tree_build_time = std::chrono::high_resolution_clock::now();

    // Export each tree
    for (int t = 1; t < n; ++t) {
        builder.writeGraph(t, builder.getChildren(t));
    }
    auto write_time = std::chrono::high_resolution_clock::now();

    auto end_time = std::chrono::high_resolution_clock::now();

    // Print timing information
    std::cout << "\nTiming Information:\n";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Initialization time: " 
              << std::chrono::duration<double>(init_time - start_time).count() 
              << " seconds\n";
    std::cout << "Tree building time: " 
              << std::chrono::duration<double>(tree_build_time - init_time).count() 
              << " seconds\n";
    std::cout << "Writing time: " 
              << std::chrono::duration<double>(write_time - tree_build_time).count() 
              << " seconds\n";
    std::cout << "Total execution time: " 
              << std::chrono::duration<double>(end_time - start_time).count() 
              << " seconds\n";

    return 0;
}
