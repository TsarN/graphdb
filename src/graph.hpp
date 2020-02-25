#pragma once

#include <string>
#include <vector>

struct Triple {
    using Locator = std::string;

    Locator subject, predicate, object;

    bool operator==(const Triple &that) const {
        return (subject == that.subject) &&
               (predicate == that.predicate) &&
               (object == that.object);
    }
};

class Graph {
public:
    virtual void addTriple(const Triple &triple) = 0;
    virtual bool hasTriple(const Triple &triple) const = 0;
};

class TripleListGraph : public Graph {
public:
    void addTriple(const Triple &triple) override;
    bool hasTriple(const Triple &triple) const override;

    std::vector<Triple> triples;
};
