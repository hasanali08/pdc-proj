# Parallelizing Independent Spanning Trees Using Bubble Networks

This project implements both parallel and serial versions of a independent spanning trees building algorithm using Bubble Networks utilizing MPI (Message Passing Interface) for parallel processing. The implementation focuses on generating and analyzing tree structures with efficient distribution of work across multiple processes.

## Project Structure

```
pdc-proj/
├── Parallel/           # Parallel implementation using MPI
│   ├── dot/           # Generated DOT files for visualization
│   ├── tree_builder.hpp
│   ├── tree_builder.cpp
│   ├── permutation_utils.hpp
│   ├── permutation_utils.cpp
│   ├── main.cpp
│   └── dot_converter.cpp
├── Serial/            # Serial implementation
│   ├── dot/           # Generated DOT files for visualization
│   ├── tree_builder.hpp
│   ├── tree_builder.cpp
│   ├── permutation_utils.hpp
│   ├── permutation_utils.cpp
│   ├── main.cpp
│   └── dot_converter.cpp
└── README.md
```

## Features

- Parallel implementation using MPI for distributed processing
- Serial implementation for comparison and testing
- Tree generation and analysis
- DOT file generation for tree visualization
- Performance timing and analysis
- Support for trees of size 2 to 10

## Requirements

- C++17 compatible compiler
- MPI implementation (for parallel version)
- Graphviz (for visualizing DOT files)

## Building the Project

### Parallel Version

```bash
cd Parallel
mpic++ -O3 -std=c++17 main.cpp tree_builder.cpp permutation_utils.cpp -o parallel_tree_builder
```

### Serial Version

```bash
cd Serial
g++ -O3 -std=c++17 main.cpp tree_builder.cpp permutation_utils.cpp -o serial_tree_builder
```

## Usage

### Parallel Version

```bash
mpiexec -n <number_of_processes> ./parallel_tree_builder <n>
```

Where:
- `<number_of_processes>` is the number of MPI processes to use
- `<n>` is the size of the tree (2-10)

Example:
```bash
mpiexec -n 4 ./parallel_tree_builder 10
```

### Serial Version

```bash
./serial_tree_builder <n>
```

Where:
- `<n>` is the size of the tree (2-10)

Example:
```bash
./serial_tree_builder 10
```

## Performance Analysis

The parallel implementation includes timing information for:
- Initialization time
- Tree distribution time
- Edge generation time
- Assembly and write time
- Total execution time

## Output

Both implementations generate DOT files in their respective `dot/` directories, which can be visualized using Graphviz tools.

## Notes

- The input size `n` must be between 2 and 10
- The parallel implementation distributes work across available processes
- Generated DOT files can be converted to various image formats using Graphviz