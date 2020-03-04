#include "automaton.hpp"

#include <iostream>

int main(int argc, char **argv) {
    if (argc <= 2) {
        std::cerr << "Usage: " << argv[0] << " <regex1> <regex2>\n";
        return 1;
    }

    auto dfa = NFA::fromRegex(argv[1]).determinize();
    auto dfa2 = NFA::fromRegex(argv[2]).determinize();
    dfa.intersect(dfa2);
    std::cout << dfa;
    return 0;
}
