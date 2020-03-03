#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <exception>
#include <iostream>

class ParseException : public std::exception {
    const char* what() const noexcept {
        return "Parse error";
    }
};

class DFA {
    friend class NFA;

public:
    struct Node {
        std::map<int, Node*> trans;
        bool term = false;
    };

    friend std::ostream &operator<<(std::ostream &os, const DFA &dfa);

private:
    std::vector<std::unique_ptr<Node>> nodes;
};


class NFA {
    friend class DFA;

public:
    struct Node {
        std::multimap<int, Node*> trans;
        bool term = false;
    };

    static const int Epsilon;

    NFA();

    static NFA fromRegex(const std::string &str);

    void addCharacter(int c);
    void concat(NFA &&that);
    void alternative(NFA &&that);
    void kleene();
    DFA determinize() const;

    friend std::ostream &operator<<(std::ostream &os, const NFA &nfa);

private:
    /* fromRegex helper functions for LL(1) parser
     *   expr  ::= EPS | <seq> | <seq> '|' <expr>
     *   seq   ::= <star> | <star> <seq>
     *   star  ::= <unit> | <unit> '*'
     *   unit  ::= <char> | '(' <expr> ')'
     *   char  ::= anything but EOF, '|', '*', '(', ')'
     */
    static NFA fromExpr(std::istream &s);
    static NFA fromSeq(std::istream &s);
    static NFA fromStar(std::istream &s);
    static NFA fromUnit(std::istream &s);

    std::vector<std::unique_ptr<Node>> nodes;
};
