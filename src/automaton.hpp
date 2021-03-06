#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <exception>
#include <iostream>

// special transitions
enum {
    Epsilon = -1
};

class ParseException : public std::exception {
    const char* what() const noexcept {
        return "Parse error";
    }
};

class DFA;
class NFA;

class DFA {
    friend class NFA;

public:
    struct Node {
        std::map<int, Node*> trans;
        bool term = false;
    };

    DFA() = default;
    DFA(const DFA& that);
    DFA(DFA &&that) = default;
    DFA &operator=(DFA that);
    void swap(DFA &that);

    static DFA fromRegex(const std::string &str);

    void intersect(DFA that);
    void stripUnreachable();
    void minimize();
    bool accepts(const std::string &s) const;
    int size() const;

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

    NFA();
    NFA(const NFA& that);
    NFA(NFA &&that) = default;
    NFA &operator=(NFA that);
    void swap(NFA &that);

    static NFA fromRegex(const std::string &str);

    void intersect(const DFA& that);
    void addCharacter(int c);
    void concat(NFA that);
    void alternative(NFA that);
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
