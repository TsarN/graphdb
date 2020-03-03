#include "automaton.hpp"

#include <iostream>

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cerr << "Usage: " << argv[0] << " <regex> [nfa]\n";
        return 1;
    }

    auto nfa = NFA::fromRegex(argv[1]);
    if (argc > 2) {
        std::cout << nfa;
    } else {
        auto dfa = nfa.determinize();
        std::cout << dfa;
    }
    return 0;
}
