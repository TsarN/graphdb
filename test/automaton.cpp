#include "automaton.hpp"
#include <catch.hpp>
#include <algorithm>

TEST_CASE( "Automaton from regex", "[regex]" ) {
    SECTION( "Regex: 0|1*" ) {
        auto dfa = DFA::fromRegex("0|1*");

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
        auto dfa = DFA::fromRegex("(0|(1(01*0)*1))*");

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

    SECTION( "Empty regex" ) {
        auto dfa = DFA::fromRegex("");

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
        auto dfa = DFA::fromRegex("ab*(c|)");

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

TEST_CASE("DFA intersection", "[dfa_intersection]") {
    SECTION( "Empty intersection" ) {
        auto dfa = DFA::fromRegex("a");
        dfa.intersect(DFA::fromRegex("b"));

        SECTION( "correctness" ) {
            CHECK( !dfa.accepts("") );
            CHECK( !dfa.accepts("a") );
            CHECK( !dfa.accepts("b") );
            CHECK( !dfa.accepts("hello") );
            CHECK( !dfa.accepts("nothing goes") );
        }

        SECTION( "size" ) {
            REQUIRE(dfa.size() == 1);
        }
    }

    SECTION( "Divisible by 6" ) {
        auto dfa = DFA::fromRegex("(0|(1(01*0)*1))*");
        dfa.intersect(DFA::fromRegex("(0|1)*0"));

        SECTION( "correctness preset input" ) {
            CHECK( dfa.accepts("0") );
            CHECK( dfa.accepts("110") );
            CHECK( dfa.accepts("1100") );
            CHECK( dfa.accepts("10010") );
            CHECK( !dfa.accepts("") );
            CHECK( !dfa.accepts("11") );
            CHECK( !dfa.accepts("1") );
            CHECK( !dfa.accepts("10") );
        }


        SECTION( "correctness random input" ) {
            int n = GENERATE(take(100, random(1, 10000)));
            std::string bin;
            for (int i = n; i > 0; i /= 2) {
                bin.push_back('0' + (i & 1));
            }
            std::reverse(bin.begin(), bin.end());

            if (n % 6 == 0) {
                CHECK( dfa.accepts(bin) );
            } else {
                CHECK( !dfa.accepts(bin) );
            }
        }

        SECTION( "minimality" ) {
            REQUIRE(dfa.size() == 4);
        }
    }
}
