// File: tree_builder.cpp
#include "tree_builder.hpp"
#include "permutation_utils.hpp"
#include <numeric>
#include <fstream>
#include <iostream>
#include <filesystem>
//tree
TreeBuilder::TreeBuilder(int n)
  : n_(n)
  , T_(n - 1)
  , identity_(n)
{
    std::cout << "TreeBuilder constructor: n=" << n << std::endl;
    std::iota(identity_.begin(), identity_.end(), 1);
    std::cout << "Identity permutation: ";
    for (int x : identity_) std::cout << x << " ";
    std::cout << std::endl;

    // Generate permutations
    perms_ = PermutationUtils::allPerms(n);
    total_ = perms_.size();
    std::cout << "Total permutations: " << total_ << std::endl;

    // Initialize tables
    posIndex_.resize(total_, std::vector<int>(n+1));
    firstMismatch_.resize(total_);
    initTables();
}

void TreeBuilder::initTables() {
    for (size_t i = 0; i < total_; ++i) {
        auto &p = perms_[i];
        indexOf_[PermutationUtils::toKey(p)] = i;
        for (int j = 0; j < n_; ++j)
            posIndex_[i][p[j]] = j;
        int r = n_-1;
        while (r >= 0 && p[r] == r+1) --r;
        firstMismatch_[i] = (r < 0 ? 1 : r);
    }
}

std::vector<int> TreeBuilder::swapAdjacent(size_t idx, int sym) const {
    auto v = perms_[idx];
    int pos = posIndex_[idx][sym];
    if (pos < 0 || pos+1 >= (int)v.size()) return v;
    std::swap(v[pos], v[pos+1]);
    return v;
}

std::vector<int> TreeBuilder::fallback(size_t idx, int t) const {
    auto v = swapAdjacent(idx, t);
    if (t == 2 && v == identity_) return swapAdjacent(idx, 1);
    int pen = perms_[idx][n_-2];
    if (pen == t || pen == n_-1) return swapAdjacent(idx, firstMismatch_[idx] + 1);
    return v;
}

size_t TreeBuilder::findParent(size_t idx, int t) const {
    const auto &p = perms_[idx];
    int last = p[n_-1], prev = p[n_-2];
    if (last == n_) {
        if (t != n_-1)
            return indexOf_.at(PermutationUtils::toKey(fallback(idx,t)));
        return indexOf_.at(PermutationUtils::toKey(swapAdjacent(idx, prev)));
    }
    if (last == n_-1 && prev == n_ && swapAdjacent(idx, n_) != identity_) {
        auto alt = (t == 1 ? swapAdjacent(idx, n_) : swapAdjacent(idx, t-1));
        return indexOf_.at(PermutationUtils::toKey(alt));
    }
    auto swp = (last == t ? swapAdjacent(idx, n_) : swapAdjacent(idx, t));
    return indexOf_.at(PermutationUtils::toKey(swp));
}

void TreeBuilder::writeGraph(int treeId, const std::vector<std::vector<uint32_t>>& children) const {
    // Create dot directory and subdirectory for this n
    std::string dotDir = "dot/" + std::to_string(n_);
    std::filesystem::create_directories(dotDir);
    
    std::string fn = dotDir + "/Tree_" + std::to_string(n_) + "_" + std::to_string(treeId) + ".dot";
    std::cout << "Writing graph to " << fn << "...\n";
    
    // Try to open file in binary mode to ensure proper writing
    std::ofstream out(fn, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Failed to open file: " << fn << std::endl;
        return;
    }
    
    // Print first few children for debugging
    std::cout << "First few children in tree " << treeId << ":\n";
    for (size_t p = 0; p < std::min(size_t(3), total_); ++p) {
        std::cout << "Node " << PermutationUtils::toKey(perms_[p]) << " has " 
                 << children[p].size() << " children\n";
    }
    
    // Write header
    out << "digraph Tree" << n_ << "_" << treeId << " {\n  rankdir=TB;\n";
    size_t edgeCount = 0;
    
    // Write edges
    for (size_t p = 0; p < total_; ++p) {
        for (auto c : children[p]) {
            std::string edge = "  \"" + PermutationUtils::toKey(perms_[p]) + 
                             "\" -> \"" + PermutationUtils::toKey(perms_[c]) + "\";\n";
            out << edge;
            if (!out.good()) {
                std::cerr << "Error writing edge to file!" << std::endl;
                return;
            }
            edgeCount++;
        }
    }
    
    // Write footer
    out << "}\n";
    out.close();
    
    std::cout << "Wrote " << edgeCount << " edges to " << fn << std::endl;
    
    // Verify file was written
    std::ifstream check(fn);
    if (check.is_open()) {
        std::string line;
        std::getline(check, line);
        std::cout << "First line of file: " << line << std::endl;
        check.close();
    } else {
        std::cerr << "Could not verify file contents!" << std::endl;
    }
}

std::vector<std::vector<std::vector<uint32_t>>> TreeBuilder::buildTrees() {
    std::cout << "Building trees for n=" << n_ << " with " << total_ << " permutations\n";
    std::cout << "First few permutations: ";
    for (size_t i = 0; i < std::min(size_t(3), total_); ++i) {
        std::cout << PermutationUtils::toKey(perms_[i]) << " ";
    }
    std::cout << "\n";
    
    allChildren_.resize(T_, std::vector<std::vector<uint32_t>>(total_));
    for (int t = 1; t <= T_; ++t) {
        std::cout << "Building tree " << t << "...\n";
        size_t edgesInTree = 0;
        for (size_t v = 0; v < total_; ++v) {
            if (perms_[v] == identity_) {
                std::cout << "Skipping identity permutation\n";
                continue;
            }
            size_t p = findParent(v, t);
            allChildren_[t-1][p].push_back(uint32_t(v));
            edgesInTree++;
            if (edgesInTree <= 3) {
                std::cout << "Added edge: " << PermutationUtils::toKey(perms_[p]) 
                         << " -> " << PermutationUtils::toKey(perms_[v]) << "\n";
            }
        }
        std::cout << "Tree " << t << " has " << edgesInTree << " edges\n";
    }
    return allChildren_;
}
