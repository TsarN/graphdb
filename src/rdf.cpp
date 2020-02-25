#include "rdf.hpp"
#include <algorithm>

RdfReader::RdfReader(RdfFormat fmt, Graph &graph) : graph(graph) {
    SerdSyntax syntax;
    switch (fmt) {
        case RdfFormat::Turtle:
            syntax = SERD_TURTLE;
            break;

        case RdfFormat::NTriples:
            syntax = SERD_NTRIPLES;
            break;
    }

    reader = serd_reader_new(
        syntax, static_cast<void*>(this), nullptr,
        nullptr, nullptr, RdfReader::statementSink, nullptr
    );
}

RdfReader::RdfReader(RdfReader &&that) :
    reader(that.reader),
    graph(that.graph) {
    that.reader = nullptr;
}

RdfReader &RdfReader::operator=(RdfReader that) {
    this->swap(that);
    return *this;
}

RdfReader::~RdfReader() {
    if (reader) {
        serd_reader_free(reader);
    }
}

void RdfReader::swap(RdfReader &that) {
    std::swap(reader, that.reader);
}

void RdfReader::readUri(const std::string &uri) {
    serd_reader_read_file(reader, (uint8_t*)uri.c_str());
}

void RdfReader::readString(const std::string &data) {
    serd_reader_read_string(reader, (uint8_t*)data.c_str());
}

SerdStatus RdfReader::statementSink(
        void *handle,
        SerdStatementFlags flags,
        const SerdNode *graph,
        const SerdNode *subject,
        const SerdNode *predicate,
        const SerdNode *object,
        const SerdNode *object_datatype,
        const SerdNode *object_lang) {
    (void)flags;
    (void)graph;
    (void)object_datatype;
    (void)object_lang;

    auto self = reinterpret_cast<RdfReader*>(handle);
    self->graph.addTriple( Triple {
            (char*)subject->buf,
            (char*)predicate->buf,
            (char*)object->buf
    });

    return SERD_SUCCESS;
}
