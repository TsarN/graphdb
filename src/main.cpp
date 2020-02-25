#include "graph.hpp"
#include "rdf.hpp"

#include <iostream>

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cerr << "Usage: " << argv[0] << " <turtle file>\n";
        return 1;
    }

    TripleListGraph graph;
    RdfReader reader(RdfFormat::Turtle, graph);
    reader.readUri(argv[1]);

    for (const auto &triple : graph.triples) {
        std::cout << "{ \"" << triple.subject << "\", \""
                  << triple.predicate << "\", \""
                  << triple.object << "\" }, " << std::endl;
    }
    return 0;
}
