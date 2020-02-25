#include "graph.hpp"
#include <algorithm>

void TripleListGraph::addTriple(const Triple &triple) {
    triples.push_back(triple);
}

bool TripleListGraph::hasTriple(const Triple &triple) const {
    return std::find(triples.cbegin(), triples.cend(), triple) != triples.cend();
}
