//mpic++ -O3 -std=c++17 main.cpp tree_builder.cpp permutation_utils.cpp -o parallel_tree_builder
// mpiexec -n 4 ./parallel_tree_builder 10
#include "tree_builder.hpp"
#include <mpi.h>
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    MPI_Init(&argc,&argv);
    int rank,size; MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if (argc!=2) {
        if (rank==0) std::cerr<<"Usage: "<<argv[0]<<" <n>\n";
        MPI_Finalize(); return 1;
    }
    int n=std::stoi(argv[1]); if (n<2||n>10) {
        if (rank==0) std::cerr<<"n must be 2..10\n";
        MPI_Finalize(); return 1;
    }

    double start_time = MPI_Wtime();
    
    ParallelTreeBuilder builder(n);
    double init_time = MPI_Wtime();
    
    int T=n-1, per=T/size, rem=T%size;
    int lo=(rank<rem? rank*(per+1)+1 : rem*(per+1)+(rank-rem)*per+1);
    int hi=(rank<rem? lo+per : lo+per-1);
    std::vector<int> setTrees;
    for (int t=lo; t<=hi; ++t) setTrees.push_back(t);

    double tree_dist_time = MPI_Wtime();
    
    builder.generateEdges(setTrees);
    double edge_gen_time = MPI_Wtime();
    
    builder.assembleAndWrite(rank,size);
    double write_time = MPI_Wtime();
    
    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    // Print timing information from rank 0
    if (rank == 0) {
        std::cout << "\nTiming Information:\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Initialization time: " << (init_time - start_time) << " seconds\n";
        std::cout << "Tree distribution time: " << (tree_dist_time - init_time) << " seconds\n";
        std::cout << "Edge generation time: " << (edge_gen_time - tree_dist_time) << " seconds\n";
        std::cout << "Assembly and write time: " << (write_time - edge_gen_time) << " seconds\n";
        std::cout << "Total execution time: " << (end_time - start_time) << " seconds\n";
    }

    MPI_Finalize(); return 0;
}
