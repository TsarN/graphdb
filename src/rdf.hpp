#pragma once

#include "graph.hpp"
#include <serd/serd.h>
#include <string>

enum class RdfFormat {
    Turtle,
    NTriples
};

class RdfReader {
public:
    RdfReader(RdfFormat fmt, Graph &graph);
    RdfReader(RdfReader &&that);
    RdfReader &operator=(RdfReader that);
    ~RdfReader();

    void swap(RdfReader &that);

    void readUri(const std::string &uri);

private:
    static SerdStatus statementSink(
            void *handle,
            SerdStatementFlags flags,
            const SerdNode *graph,
            const SerdNode *subject,
            const SerdNode *predicate,
            const SerdNode *object,
            const SerdNode *object_datatype,
            const SerdNode *object_lang);

    SerdReader *reader;
    Graph &graph;
};
