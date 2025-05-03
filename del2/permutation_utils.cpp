#include "permutation_utils.hpp"
#include <algorithm>
#include <numeric>
#include <iostream>

std::vector<std::vector<int>> PermutationUtils::allPerms(int n) {
    std::cout << "Generating permutations for n=" << n << std::endl;
    std::vector<int> base(n);
    std::iota(base.begin(), base.end(), 1);
    std::cout << "Initial base: ";
    for (int x : base) std::cout << x << " ";
    std::cout << std::endl;
    
    std::vector<std::vector<int>> result;
    result.push_back(base);  // Add the first permutation
    
    std::cout << "Starting permutation generation...\n";
    while (std::next_permutation(base.begin(), base.end())) {
        result.push_back(base);
        if (result.size() <= 3) {
            std::cout << "Added permutation: ";
            for (int x : base) std::cout << x << " ";
            std::cout << std::endl;
        }
    }
    
    std::cout << "Generated " << result.size() << " permutations\n";
    return result;
}

std::string PermutationUtils::toKey(const std::vector<int>& perm) {
    std::string s;
    s.reserve(perm.size());
    for (int v : perm)
        s.push_back(char('0' + v));
    return s;
}
