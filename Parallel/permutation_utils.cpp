#include "permutation_utils.hpp"
#include <algorithm>
#include <numeric>
#include <iostream>

std::vector<std::vector<uint8_t>> PermutationUtils::allPerms(int n) {
    std::cout << "Generating permutations for n=" << n << std::endl;
    
    // Pre-calculate factorial size to avoid resizing
    int factorial = 1;
    for(int i = 2; i <= n; i++) {
        factorial *= i;
    }
    
    std::vector<uint8_t> base(n);
    std::iota(base.begin(), base.end(), 1);
    
    // Pre-allocate the exact space needed
    std::vector<std::vector<uint8_t>> result;
    result.reserve(factorial);
    
    std::cout << "Initial base: ";
    for (uint8_t x : base) std::cout << (int)x << " ";
    std::cout << std::endl;
    
    // More efficient insertion
    result.push_back(base);  // Add the first permutation
    
    std::cout << "Starting permutation generation...\n";
    while (std::next_permutation(base.begin(), base.end())) {
        result.push_back(base);
        if (result.size() <= 3) {
            std::cout << "Added permutation: ";
            for (uint8_t x : base) std::cout << (int)x << " ";
            std::cout << std::endl;
        }
    }
    
    std::cout << "Generated " << result.size() << " permutations\n";
    return result;
}

std::string PermutationUtils::toKey(const std::vector<uint8_t>& perm) {
    // Faster string construction with single allocation
    std::string s;
    s.reserve(perm.size());
    for (uint8_t v : perm)
        s.push_back(char('0' + v));
    return s;
}
