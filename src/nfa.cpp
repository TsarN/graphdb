#include "automaton.hpp"
#include <sstream>
#include <algorithm>
#include <set>
#include <functional>

NFA::NFA() {
    auto node = std::make_unique<NFA::Node>();
    node->term = true;
    nodes.emplace_back(std::move(node));
}

NFA::NFA(const NFA& that) {
    std::map<Node*, int> indexes;
    for (auto &node : that.nodes) {
        indexes[node.get()] = indexes.size();
        auto t = std::make_unique<Node>();
        t->term = node->term;
        nodes.emplace_back(std::move(t));
    }

    for (auto &node : that.nodes) {
        auto &t = nodes[indexes[node.get()]];
        for (auto &[ch, to] : node->trans) {
            t->trans.insert({ ch, nodes[indexes[to]].get() });
        }
    }
}

NFA &NFA::operator=(NFA that) {
    swap(that);
    return *this;
}

void NFA::swap(NFA &that) {
    nodes.swap(that.nodes);
}

NFA NFA::fromRegex(const std::string &str) {
    std::istringstream s{str};
    auto nfa = fromExpr(s);
    if (s.peek() != -1) {
        throw ParseException{};
    }
    return nfa;
}

NFA NFA::fromExpr(std::istream &s) {
    auto ch = s.peek();
    if (ch == -1 || ch == '|' || ch == '*' || ch == ')') {
        auto nfa = NFA{};
        return nfa;
    }
    auto nfa = fromSeq(s);
    if (s.peek() == '|') {
        s.get();
        nfa.alternative(fromExpr(s));
    }
    return nfa;
}

NFA NFA::fromSeq(std::istream &s) {
    auto nfa = fromStar(s);

    // to determine whether there are more elements in the sequence,
    // next character must not be EOF, '|', '*', ')'
    auto next = s.peek();
    if (next == -1 || next == '|' || next == '*' || next == ')') {
        return nfa;
    }

    nfa.concat(fromSeq(s));
    return nfa;
}

NFA NFA::fromStar(std::istream &s) {
    auto nfa = fromUnit(s);
    if (s.peek() == '*') {
        s.get();
        nfa.kleene();
    }
    return nfa;
}

NFA NFA::fromUnit(std::istream &s) {
    auto ch = s.peek();

    if (ch == '|' || ch == ')' || ch == '*' ||  ch == -1) {
        throw ParseException{};
    }

    if (ch == '(') {
        s.get();
        auto nfa = fromExpr(s);
        if (s.get() != ')') throw ParseException{};
        return nfa;
    }

    // normal character
    s.get();
    auto nfa = NFA{};
    nfa.addCharacter(ch);
    return nfa;
}

void NFA::addCharacter(int c) {
    auto term = std::make_unique<NFA::Node>();
    term->term = true;

    for (auto &node : nodes) {
        if (!node->term) continue;
        node->term = false;
        node->trans.insert({ c, term.get() });
    }

    nodes.emplace_back(std::move(term));
}

void NFA::concat(NFA that) {
    for (auto &node : nodes) {
        if (!node->term) continue;
        node->term = false;
        node->trans.insert({ Epsilon, that.nodes[0].get() });
    }

    for (auto &node : that.nodes) {
        nodes.emplace_back(std::move(node));
    }

    that.nodes.clear();
}

void NFA::alternative(NFA that) {
    nodes[0]->trans.insert({ Epsilon, that.nodes[0].get() });

    for (auto &node : that.nodes) {
        nodes.emplace_back(std::move(node));
    }

    that.nodes.clear();
}

void NFA::kleene() {
    for (auto &node : nodes) {
        if (!node->term) continue;
        node->trans.insert({ Epsilon, nodes[0].get() });
    }
    nodes[0]->term = true;
}

void NFA::intersect(const DFA& that) {
    // precondition: NFA doesn't contain epsilon-transitions
    std::map<Node*, int> thisIdx;
    std::map<DFA::Node*, int> thatIdx;
    auto thisNodes = std::move(nodes);
    nodes.clear();

    for (auto &node : thisNodes) {
        thisIdx[node.get()] = thisIdx.size();
    }

    for (auto &node : that.nodes) {
        thatIdx[node.get()] = thatIdx.size();
    }

    for (int i = 0; i < thisNodes.size(); ++i) {
        for (int j = 0; j < that.nodes.size(); ++j) {
            auto node = std::make_unique<Node>();
            node->term = thisNodes[i]->term && that.nodes[j]->term;
            nodes.emplace_back(std::move(node));
        }
    }

    for (int i = 0; i < thisNodes.size(); ++i) {
        auto &ti = thisNodes[i]->trans;
        for (int j = 0; j < that.nodes.size(); ++j) {
            int idx = i * that.nodes.size() + j;
            auto &tj = that.nodes[j]->trans;

            for (auto &[ch, to] : ti) {
                if (tj.find(ch) == tj.end()) continue;
                int u = thisIdx[to];
                int v = thatIdx[tj[ch]];
                int tidx = u * that.nodes.size() + v;
                nodes[idx]->trans.emplace(ch, nodes[tidx].get());
            }
        }
    }
}

DFA NFA::determinize() const {
    std::map<NFA::Node*, int> indexes;
    for (auto &node : nodes) {
        indexes[node.get()] = indexes.size();
    }

    // First of all, construct epsilon equivalent states
    // To do this, find strongly connected components

    // I use char since std::vector<bool> is too weird, man
    std::vector<char> used(nodes.size());

    // topsort order
    std::vector<int> order;
    order.reserve(nodes.size());

    // reverse epsilon transitions
    std::vector<std::vector<int>> rg(nodes.size());

    // equivalence classes
    std::vector<int> equiv(nodes.size());
    std::vector<char> termClass;

    std::function<void(int)> dfs1 = [&](int v) {
        used[v] = 1;
        for (auto [ch, to] : nodes[v]->trans) {
            if (ch != Epsilon) {
                continue;
            }

            int u = indexes[to];
            rg[u].push_back(v);
            if (used[u]) continue;
            dfs1(u);
        }
        order.push_back(v);
    };

    for (int i = 0; i < nodes.size(); ++i) {
        if (!used[i]) {
            dfs1(i);
        }
    }
    used.assign(nodes.size(), 0);

    std::function<void(int, int)> dfs2 = [&](int v, int c) {
        used[v] = 1;
        equiv[v] = c;

        if (nodes[v]->term) termClass[c] = 1;

        for (int u : rg[v]) {
            if (used[u]) continue;
            dfs2(u, c);
        }
    };

    for (auto it = order.rbegin(); it != order.rend(); ++it) {
        if (!used[*it]) {
            termClass.push_back(0);
            dfs2(*it, termClass.size() - 1);
        }
    }

    // Now we have assigned equivalence classes,
    // let's build transitions map in epsilon-compressed automata
    std::vector<std::map<int, std::set<int>>> tr(termClass.size());

    for (auto &node : nodes) {
        int v = equiv[indexes[node.get()]];
        for (auto [ch, to] : node->trans) {
            int u = equiv[indexes[to]];
            tr[v][ch].insert(u);
        }
    }

    // Build epsilon reachability table
    std::vector<std::set<int>> eps(termClass.size());

    for (int v = eps.size() - 1; v >= 0; --v) {
        eps[v].insert(v);
        for (int u : tr[v][Epsilon]) {
            termClass[v] |= termClass[u];
            for (int i : eps[u]) {
                eps[v].insert(i);
            }
        }
    }

    for (int v = eps.size() - 1; v >= 0; --v) {
        for (auto [ch, to] : tr[v]) {
            std::set<int> res;
            for (int u : to) {
                for (int w : eps[u]) {
                    res.insert(w);
                }
            }
            tr[v][ch] = std::move(res);
        }
    }

    // Okay, now to map subsets of equiv classes to ints
    std::map<std::set<int>, int> indexesFinal;

    indexesFinal[eps[equiv[0]]] = 0; // start node
    std::vector<std::set<int>> indexesFinalV{eps[equiv[0]]};

    auto dfa = DFA{};
    dfa.nodes.resize(indexesFinal.size());
    for (auto &[s, i] : indexesFinal) {
        auto node = std::make_unique<DFA::Node>();
        for (int j : s) {
            if (termClass[j]) {
                node->term = true;
                break;
            }
        }
        dfa.nodes[i] = std::move(node);
    }

    for (int i = 0; i < indexesFinalV.size(); ++i) {
        auto &s = indexesFinalV[i];
        std::map<int, std::set<int>> u;
        auto from = dfa.nodes[i].get();

        for (int j : s) {
            for (auto &[ch, to] : tr[j]) {
                if (ch == Epsilon) continue;
                for (int k : to) {
                    u[ch].insert(k);
                }
            }
        }

        for (auto &[ch, to] : u) {
            if (to.empty()) continue;
            if (indexesFinal.find(to) == indexesFinal.end()) {
                indexesFinal[to] = indexesFinal.size();
                indexesFinalV.push_back(to);
                auto node = std::make_unique<DFA::Node>();

                for (int j : to) {
                    if (termClass[j]) {
                        node->term = true;
                        break;
                    }
                }

                dfa.nodes.emplace_back(std::move(node));
            }

            auto node = dfa.nodes[indexesFinal[to]].get();
            from->trans[ch] = node;
        }
    }

    dfa.minimize();

    // whew

    return dfa;
}

std::ostream &operator<<(std::ostream &os, const NFA &nfa) {
    // print NFA in DOT graph format

    os << "digraph NFA {" << std::endl;

    std::map<NFA::Node*, int> indexes;
    bool f = false;
    for (auto &node : nfa.nodes) {
        indexes[node.get()] = indexes.size();
        if (node->term) {
            f = true;
        }
    }
    if (f) {
        os << "node [shape = doublecircle];";
        for (auto &node : nfa.nodes) {
            if (node->term) {
                os << " " << indexes[node.get()];
            }
        }
        os << ";" << std::endl;
    }

    os << "node [shape = circle];" << std::endl;

    for (auto &node : nfa.nodes) {
        int fromIdx = indexes[node.get()];
        for (auto [ch, to] : node->trans) {
            int toIdx = indexes[to];
            os << fromIdx << " -> " << toIdx;
            std::string label{(char)ch};
            if (ch == Epsilon) {
                label = "eps";
            }
            os << " [label = \"" << label << "\" ];" << std::endl;
        }
    }

    os << "}" << std::endl;
    return os;
}

