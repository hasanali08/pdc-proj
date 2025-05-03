#ifndef PERMUTATION_UTILS_HPP
#define PERMUTATION_UTILS_HPP

#include <vector>
#include <string>
#include <cstdint>

// Utility for generating and keying permutations
class PermutationUtils {
public:
    // Generate all permutations of {1..n}
    static std::vector<std::vector<int>> allPerms(int n);

    // Convert a permutation vector to a string key
    static std::string toKey(const std::vector<int>& perm);
};

#endif // PERMUTATION_UTILS_HPP
