#include "tree_builder.hpp"
#include "permutation_utils.hpp"
#include <mpi.h>
#include <omp.h>
#include <numeric>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <filesystem>

ParallelTreeBuilder::ParallelTreeBuilder(int dimension)
    : dim_(dimension)
    , treeCount_(dimension - 1)
    , identity_(dimension)
{
    double start_time = MPI_Wtime();
    
    std::cout << "ParallelTreeBuilder constructor: n=" << dimension << std::endl;
    std::iota(identity_.begin(), identity_.end(), 1);
    std::cout << "Identity permutation: ";
    for (uint8_t x : identity_) std::cout << (int)x << " ";
    std::cout << std::endl;

    // Generate permutations
    double perm_start = MPI_Wtime();
    elements_ = PermutationUtils::allPerms(dimension);
    count_ = elements_.size();
    std::cout << "Total permutations: " << count_ << std::endl;
    double perm_end = MPI_Wtime();

    // Initialize tables
    double init_start = MPI_Wtime();
    locator_.resize(count_, std::vector<uint8_t>(dimension+1));
    mismatchPos_.resize(count_);
    globalKids_.resize(treeCount_, std::vector<std::vector<uint32_t>>(count_));
    initData();
    double init_end = MPI_Wtime();

    double end_time = MPI_Wtime();
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        std::cout << "\nConstructor Timing:\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Permutation generation: " << (perm_end - perm_start) << " seconds\n";
        std::cout << "Data structure initialization: " << (init_end - init_start) << " seconds\n";
        std::cout << "Total constructor time: " << (end_time - start_time) << " seconds\n";
    }
}

void ParallelTreeBuilder::initData() {
    // build index map
    for (size_t i = 0; i < count_; ++i)
        indexOf_[PermutationUtils::toKey(elements_[i])] = i;
    
    // Use OpenMP for parallel initialization
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < count_; ++i) {
        const auto &perm = elements_[i];
        // Optimized inner loop - unrolled for small n
        if (dim_ <= 8) {
            for (int j = 0; j < dim_; ++j) {
                locator_[i][perm[j]] = (uint8_t)j;
            }
        } else {
            // Vectorizable loop for larger n
            for (int j = 0; j < dim_; ++j) {
                locator_[i][perm[j]] = (uint8_t)j;
            }
        }
        
        // More efficient first wrong computation
        int k = dim_-1; 
        while (k >= 0 && perm[k] == k+1) --k;
        mismatchPos_[i] = (k < 0 ? 1 : (uint8_t)k);
    }
}

std::vector<uint8_t> ParallelTreeBuilder::slide(size_t idx, int sym) const {
    const auto& curr = elements_[idx];
    int pos = locator_[idx][sym];
    if (pos < 0 || pos+1 >= dim_) return curr;
    
    // Create result with reserved capacity
    std::vector<uint8_t> result(curr);
    std::swap(result[pos], result[pos+1]);
    return result;
}

std::vector<uint8_t> ParallelTreeBuilder::fallbackParent(size_t idx, int t) const {
    auto cp = slide(idx, t);
    if (t == 2 && cp == identity_) return slide(idx, t-1);
    uint8_t pen = elements_[idx][dim_-2];
    if (pen == t || pen == dim_-1) return slide(idx, mismatchPos_[idx]+1);
    return cp;
}

uint32_t ParallelTreeBuilder::findParent(size_t node, int t) const {
    const auto &perm = elements_[node];
    uint8_t last = perm[dim_-1], prev = perm[dim_-2];
    
    if (last == dim_) {
        if (t != dim_-1) return indexOf_.at(PermutationUtils::toKey(fallbackParent(node,t)));
        return indexOf_.at(PermutationUtils::toKey(slide(node, prev)));
    }
    
    if (last == dim_-1 && prev == dim_ && slide(node, dim_) != identity_) {
        auto alt = (t == 1 ? slide(node, dim_) : slide(node, t-1));
        return indexOf_.at(PermutationUtils::toKey(alt));
    }
    
    auto swp = (last == t ? slide(node, dim_) : slide(node, t));
    return indexOf_.at(PermutationUtils::toKey(swp));
}

void ParallelTreeBuilder::generateEdges(const std::vector<int>& trees) {
    double start_time = MPI_Wtime();
    
    assignedTrees_ = trees;  // Store the assigned trees
    size_t total = trees.size() * count_;
    
    double parallel_start = MPI_Wtime();
    
    // Pre-allocate localList_ with estimated size
    localList_.reserve(total);
    
    #pragma omp parallel
    {
        // Thread-local buffer for collecting edges
        const int MAX_EDGES_PER_THREAD = 100000;
        std::vector<std::tuple<int,uint32_t,uint32_t>> thread_edges;
        thread_edges.reserve(MAX_EDGES_PER_THREAD);
        
        #pragma omp for schedule(dynamic, 1024)
        for (size_t x = 0; x < total; ++x) {
            size_t li = x / count_; 
            size_t v = x % count_;
            if (elements_[v] == identity_) continue;
            
            uint32_t p = findParent(v, trees[li]);
            
            // Store edge in thread-local buffer
            thread_edges.emplace_back((int)li, p, (uint32_t)v);
            
            // If buffer is full, flush to global list
            if (thread_edges.size() >= MAX_EDGES_PER_THREAD) {
                #pragma omp critical
                {
                    localList_.insert(localList_.end(), thread_edges.begin(), thread_edges.end());
                }
                thread_edges.clear();
            }
        }
        
        // Flush remaining edges
        if (!thread_edges.empty()) {
            #pragma omp critical
            {
                localList_.insert(localList_.end(), thread_edges.begin(), thread_edges.end());
            }
        }
    }
    
    double parallel_end = MPI_Wtime();
    
    double end_time = MPI_Wtime();
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        std::cout << "\nEdge Generation Timing:\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Parallel computation time: " << (parallel_end - parallel_start) << " seconds\n";
        std::cout << "Total edge generation time: " << (end_time - start_time) << " seconds\n";
    }
}

void ParallelTreeBuilder::assembleAndWrite(int rank, int worldSize) {
    double start_time = MPI_Wtime();
    
    int T = treeCount_;
    if (rank == 0) {
        // place local edges
        double local_start = MPI_Wtime();
        for (auto &e: localList_) {
            int li; uint32_t p,c; std::tie(li,p,c)=e;
            globalKids_[assignedTrees_[li]-1][p].push_back(c);
        }
        double local_end = MPI_Wtime();
        
        // gather from others
        double gather_start = MPI_Wtime();
        for (int src=1; src<worldSize; ++src) {
            // First receive the number of trees this process has
            int numTrees;
            MPI_Recv(&numTrees, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Receive the tree indices this process is handling
            std::vector<int> treeIndices(numTrees);
            MPI_Recv(treeIndices.data(), numTrees, MPI_INT, src, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // For each tree this process handles
            for (int i = 0; i < numTrees; ++i) {
                int treeIdx = treeIndices[i];
                int countMsg;
                MPI_Recv(&countMsg, 1, MPI_INT, src, treeIdx, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                
                if (countMsg > 0) {
                    std::vector<uint32_t> buf(2*countMsg);
                    MPI_Recv(buf.data(), 2*countMsg, MPI_UINT32_T, src, treeIdx+T, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    
                    for (int j=0; j<countMsg; ++j) {
                        globalKids_[treeIdx-1][buf[2*j]].push_back(buf[2*j+1]);
                    }
                }
            }
        }
        double gather_end = MPI_Wtime();
        
        // write DOTs
        double write_start = MPI_Wtime();
        for (int t=1; t<=T; ++t)
            writeDot(t, globalKids_[t-1]);
        double write_end = MPI_Wtime();
        
        double end_time = MPI_Wtime();
        
        std::cout << "\nAssembly and Write Timing (Rank 0):\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Local edge placement: " << (local_end - local_start) << " seconds\n";
        std::cout << "Gathering from other ranks: " << (gather_end - gather_start) << " seconds\n";
        std::cout << "Writing DOT files: " << (write_end - write_start) << " seconds\n";
        std::cout << "Total assembly and write time: " << (end_time - start_time) << " seconds\n";
    } else {
        double send_start = MPI_Wtime();
        
        // Send number of trees this process handles
        int numTrees = assignedTrees_.size();
        MPI_Send(&numTrees, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        
        // Send the tree indices this process handles
        MPI_Send(assignedTrees_.data(), numTrees, MPI_INT, 0, 1, MPI_COMM_WORLD);
        
        // Group edges by tree
        std::vector<std::vector<std::pair<uint32_t, uint32_t>>> treeEdges(numTrees);
        for (auto &e: localList_) {
            int li; uint32_t p,c; std::tie(li,p,c)=e;
            treeEdges[li].emplace_back(p, c);
        }
        
        // Send edges for each tree
        for (int i = 0; i < numTrees; ++i) {
            int treeIdx = assignedTrees_[i];
            int cnt = treeEdges[i].size();
            MPI_Send(&cnt, 1, MPI_INT, 0, treeIdx, MPI_COMM_WORLD);
            
            if (cnt > 0) {
                std::vector<uint32_t> buf(2*cnt);
                for (int j = 0; j < cnt; ++j) {
                    buf[2*j] = treeEdges[i][j].first;
                    buf[2*j+1] = treeEdges[i][j].second;
                }
                MPI_Send(buf.data(), 2*cnt, MPI_UINT32_T, 0, treeIdx+T, MPI_COMM_WORLD);
            }
        }
        double send_end = MPI_Wtime();
        
        std::cout << "\nAssembly Timing (Rank " << rank << "):\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Sending data to rank 0: " << (send_end - send_start) << " seconds\n";
    }
}

void ParallelTreeBuilder::writeDot(int tree, const std::vector<std::vector<uint32_t>>& kids) const {
    // Create dot directory and subdirectory for this n
    std::string dotDir = "dot/" + std::to_string(dim_);
    std::filesystem::create_directories(dotDir);
    
    std::string filename = dotDir + "/Tree_" + std::to_string(dim_) + "_" + std::to_string(tree) + ".dot";
    std::ofstream os(filename);
    os << "digraph Tree" << dim_ << "_" << tree << " {\n";
    os << "    rankdir = LR;\n";
    
    // Pre-allocate string buffer for better performance
    std::string edge_str;
    edge_str.reserve(100);  // Typical edge string size
    
    for (uint32_t p=0; p<count_; ++p) {
        if (kids[p].empty()) continue;
        for (auto c: kids[p]) {
            edge_str.clear();
            edge_str = "    \"";
            edge_str += PermutationUtils::toKey(elements_[p]);
            edge_str += "\" -> \"";
            edge_str += PermutationUtils::toKey(elements_[c]);
            edge_str += "\";\n";
            os << edge_str;
        }
    }
    os << "}\n";
}
