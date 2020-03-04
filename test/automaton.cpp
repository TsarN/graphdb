#include "automaton.hpp"
#include <catch.hpp>
#include <algorithm>

TEST_CASE( "Automaton from regex", "[regex]" ) {
    SECTION( "Regex: 0|1*" ) {
        auto nfa = NFA::fromRegex("0|1*");
        auto dfa = nfa.determinize();

        SECTION( "correctness" ) {
            CHECK( dfa.accepts("") );
            CHECK( dfa.accepts("0") );
            CHECK( dfa.accepts("1") );
            CHECK( dfa.accepts("11") );
            CHECK( dfa.accepts("111") );
            CHECK( dfa.accepts("1111") );

            CHECK( !dfa.accepts("00") );
            CHECK( !dfa.accepts("10") );
            CHECK( !dfa.accepts("01") );
            CHECK( !dfa.accepts("0111") );
            CHECK( !dfa.accepts("010") );
            CHECK( !dfa.accepts("101") );
            CHECK( !dfa.accepts("2") );
            CHECK( !dfa.accepts("11112") );
            CHECK( !dfa.accepts("02") );
        }

        SECTION( "minimality" ) {
            REQUIRE(dfa.size() == 3);
        }
    }

    SECTION( "Regex: (0|(1(01*0)*1))*" ) {
        // Regex checks if a binary number is divisible by 3
        auto nfa = NFA::fromRegex("(0|(1(01*0)*1))*");
        auto dfa = nfa.determinize();

        SECTION( "correctness preset input" ) {
            CHECK( dfa.accepts("") );
            CHECK( dfa.accepts("0") );
            CHECK( dfa.accepts("11") );
            CHECK( !dfa.accepts("1") );
            CHECK( !dfa.accepts("10") );
        }


        SECTION( "correctness random input" ) {
            int n = GENERATE(take(100, random(4, 10000)));
            std::string bin;
            for (int i = n; i > 0; i /= 2) {
                bin.push_back('0' + (i & 1));
            }
            std::reverse(bin.begin(), bin.end());

            if (n % 3 == 0) {
                CHECK( dfa.accepts(bin) );
            } else {
                CHECK( !dfa.accepts(bin) );
            }
        }

        SECTION( "minimality" ) {
            REQUIRE(dfa.size() == 3);
        }
    }

    SECTION( "Regex: .*" ) {
        auto nfa = NFA::fromRegex(".*");
        auto dfa = nfa.determinize();

        SECTION( "correctness" ) {
            CHECK( dfa.accepts("") );
            CHECK( dfa.accepts("a") );
            CHECK( dfa.accepts("hello") );
            CHECK( dfa.accepts("anything goes") );
        }

        SECTION( "minimality" ) {
            REQUIRE(dfa.size() == 1);
        }
    }

    SECTION( "Empty regex" ) {
        auto nfa = NFA::fromRegex("");
        auto dfa = nfa.determinize();

        SECTION( "correctness" ) {
            CHECK( dfa.accepts("") );
            CHECK( !dfa.accepts("a") );
            CHECK( !dfa.accepts("hello") );
            CHECK( !dfa.accepts("nothing goes") );
        }

        SECTION( "minimality" ) {
            REQUIRE(dfa.size() == 1);
        }
    }

    SECTION( "Regex: ab*(c|)" ) {
        auto nfa = NFA::fromRegex("ab*(c|)");
        auto dfa = nfa.determinize();

        SECTION( "correctness" ) {
            CHECK( dfa.accepts("a") );
            CHECK( dfa.accepts("ac") );
            CHECK( dfa.accepts("ab") );
            CHECK( dfa.accepts("abc") );
            CHECK( dfa.accepts("abbbb") );
            CHECK( dfa.accepts("abbbbbc") );

            CHECK( !dfa.accepts("") );
            CHECK( !dfa.accepts("ba") );
            CHECK( !dfa.accepts("abcc") );
            CHECK( !dfa.accepts("abcb") );
            CHECK( !dfa.accepts("aabbbb") );
            CHECK( !dfa.accepts("abbbbbcc") );
        }

        SECTION( "minimality" ) {
            REQUIRE(dfa.size() == 3);
        }
    }

    SECTION( "Invalid regexes" ) {
        CHECK_THROWS_AS(NFA::fromRegex("("), ParseException);
        CHECK_THROWS_AS(NFA::fromRegex(")"), ParseException);
        CHECK_THROWS_AS(NFA::fromRegex("*b"), ParseException);
        CHECK_THROWS_AS(NFA::fromRegex("|"), ParseException);
    }
}
