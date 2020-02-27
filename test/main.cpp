#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "graph.hpp"
#include "rdf.hpp"

const std::vector<Triple> artists_triples = {
    { "ex:Picasso", "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", "ex:Artist" },
    { "ex:Picasso", "foaf:firstName", "Pablo" },
    { "ex:Picasso", "foaf:surname", "Picasso" },
    { "ex:Picasso", "ex:creatorOf", "ex:guernica" },
    { "ex:Picasso", "ex:homeAddress", "node1" },
    { "node1", "ex:street", "31 Art Gallery" },
    { "node1", "ex:city", "Madrid" },
    { "node1", "ex:country", "Spain" },
    { "ex:guernica", "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", "ex:Painting" },
    { "ex:guernica", "rdfs:label", "Guernica" },
    { "ex:guernica", "ex:technique", "oil on canvas" },
    { "ex:VanGogh", "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", "ex:Artist" },
    { "ex:VanGogh", "foaf:firstName", "Vincent" },
    { "ex:VanGogh", "foaf:surname", "van Gogh" },
    { "ex:VanGogh", "ex:creatorOf", "ex:starryNight" },
    { "ex:VanGogh", "ex:creatorOf", "ex:sunflowers" },
    { "ex:VanGogh", "ex:creatorOf", "ex:potatoEaters" },
    { "ex:starryNight", "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", "ex:Painting" },
    { "ex:starryNight", "ex:technique", "oil on canvas" },
    { "ex:starryNight", "rdfs:label", "Starry Night" },
    { "ex:sunflowers", "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", "ex:Painting" },
    { "ex:sunflowers", "ex:technique", "oil on canvas" },
    { "ex:sunflowers", "rdfs:label", "Sunflowers" },
    { "ex:potatoEaters", "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", "ex:Painting" },
    { "ex:potatoEaters", "ex:technique", "oil on canvas" },
    { "ex:potatoEaters", "rdfs:label", "The Potato Eaters" },
};

TEST_CASE( "Painters paint paintings", "[rdf]" ) {
    TripleListGraph graph;
    RdfReader reader(RdfFormat::Turtle, graph);
    reader.readUri("test/sample.ttl");

    REQUIRE(graph.triples == artists_triples);
}
