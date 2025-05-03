#ifndef TREE_BUILDER_HPP
#define TREE_BUILDER_HPP

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

// Serial construction of n−1 spanning trees on Bₙ
class TreeBuilder {
public:
    explicit TreeBuilder(int n);
    // Builds all trees and returns children adjacency lists
    std::vector<std::vector<std::vector<uint32_t>>> buildTrees();
    const std::vector<std::vector<int>>& getPerms() const { return perms_; }

    const std::vector<std::tuple<int, uint32_t, uint32_t>>& getEdges() const {
        return localEdges_;
    }
    const std::vector<std::vector<uint32_t>>& getChildren(int t) const {
        return allChildren_[t - 1];
    }
    std::vector<std::vector<uint32_t>>& getMutableChildren(int t) {
        return allChildren_[t - 1];
    }
    void writeGraph(int treeId, const std::vector<std::vector<uint32_t>>& children) const;

private:
    int n_;                                  // permutation length
    size_t total_;                           // n! permutations
    int T_;                                  // number of trees (n−1)
    std::vector<std::vector<int>> perms_;
    std::vector<std::vector<int>> posIndex_;
    std::vector<int> firstMismatch_;
    std::vector<int> identity_;
    std::unordered_map<std::string, size_t> indexOf_;
    std::vector<std::tuple<int, uint32_t, uint32_t>> localEdges_;
    std::vector<std::vector<std::vector<uint32_t>>> allChildren_;

    void initTables();
    std::vector<int> swapAdjacent(size_t, int) const;
    std::vector<int> fallback(size_t, int) const;
    size_t findParent(size_t, int) const;
};

#endif // TREE_BUILDER_HPP
