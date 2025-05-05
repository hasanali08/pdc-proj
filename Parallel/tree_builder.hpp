#ifndef TREE_BUILDER_HPP
#define TREE_BUILDER_HPP

#include <vector>
#include <tuple>
#include <cstdint>
#include <string>
#include <unordered_map>

// Constructs n-1 spanning trees on B_n using MPI + OpenMP
class ParallelTreeBuilder {
public:
    explicit ParallelTreeBuilder(int dimension);

    // Parallel: produce parent-child tuples for assigned tree indices
    void generateEdges(const std::vector<int>& trees);
    // Collate edges from all ranks and emit GraphViz DOT
    void assembleAndWrite(int rank, int worldSize);

private:
    int dim_;                            // permutation length n
    size_t count_;                       // n! vertices
    int treeCount_;                      // n-1 trees
    std::vector<std::vector<uint8_t>> elements_;  // all perms
    std::vector<std::vector<uint8_t>> locator_;   // position lookup
    std::vector<uint8_t> mismatchPos_;            // first mismatch
    std::vector<uint8_t> identity_;               // [1..n]
    std::unordered_map<std::string,size_t> indexOf_; // perm -> idx
    std::vector<int> assignedTrees_;          // tree indices assigned to this rank

    // temporary storage of (treeIdxLocal, parentIdx, childIdx)
    std::vector<std::tuple<int,uint32_t,uint32_t>> localList_;
    // global children: trees->[parent]->vector<child>
    std::vector<std::vector<std::vector<uint32_t>>> globalKids_;

    // Setup structures
    void initData();
    // Compute parent of node for tree t
    uint32_t findParent(size_t node, int t) const;
    std::vector<uint8_t> slide(size_t idx, int sym) const;
    std::vector<uint8_t> fallbackParent(size_t idx, int t) const;
    // write one DOT file
    void writeDot(int tree, const std::vector<std::vector<uint32_t>>& kids) const;
};

#endif // TREE_BUILDER_HPP