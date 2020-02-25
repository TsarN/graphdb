#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "graph.hpp"
#include "rdf.hpp"

const char artists_ttl[] =
    "@prefix ex: <http://example.org/> .\n"
    "@prefix foaf: <http://xmlns.com/foaf/0.1/> .\n"
    "ex:Picasso a ex:Artist ;\n"
    "           foaf:firstName \"Pablo\" ;\n"
    "           foaf:surname \"Picasso\";\n"
    "           ex:creatorOf ex:guernica ;\n"
    "           ex:homeAddress _:node1 .\n"
    "\n"
    "_:node1  ex:street \"31 Art Gallery\" ;\n"
    "         ex:city \"Madrid\" ;\n"
    "         ex:country \"Spain\" .\n"
    "\n"
    "ex:guernica a ex:Painting ;\n"
    "            rdfs:label \"Guernica\";\n"
    "            ex:technique \"oil on canvas\".\n"
    "\n"
    "ex:VanGogh a ex:Artist ;\n"
    "           foaf:firstName \"Vincent\" ;\n"
    "           foaf:surname \"van Gogh\";\n"
    "           ex:creatorOf ex:starryNight, ex:sunflowers, ex:potatoEaters .\n"
    "\n"
    "ex:starryNight a ex:Painting ;\n"
    "               ex:technique \"oil on canvas\";\n"
    "               rdfs:label \"Starry Night\" .\n"
    "\n"
    "ex:sunflowers a ex:Painting ;\n"
    "              ex:technique \"oil on canvas\";\n"
    "              rdfs:label \"Sunflowers\" .\n"
    "\n"
    "ex:potatoEaters a ex:Painting ;\n"
    "                ex:technique \"oil on canvas\";\n"
    "                rdfs:label \"The Potato Eaters\" .\n";

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
    reader.readString(artists_ttl);

    REQUIRE(graph.triples == artists_triples);
}
