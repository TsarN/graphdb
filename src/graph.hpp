#pragma once

#include <string>
#include <vector>

struct Triple {
    using Locator = std::string;

    Locator subject, predicate, object;
};

class Graph {
public:
    virtual void addTriple(const Triple &triple) = 0;
};

class TripleListGraph : public Graph {
public:
    void addTriple(const Triple &triple) override;

    std::vector<Triple> triples;
};
