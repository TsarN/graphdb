#include "automaton.hpp"
#include <sstream>
#include <algorithm>
#include <set>
#include <functional>

DFA::DFA(const DFA& that) {
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
            t->trans[ch] = nodes[indexes[to]].get();
        }
    }
}

DFA &DFA::operator=(DFA that) {
    swap(that);
    return *this;
}

void DFA::swap(DFA &that) {
    nodes.swap(that.nodes);
}

DFA DFA::fromRegex(const std::string &str) {
    return NFA::fromRegex(str).determinize();
}

void DFA::intersect(DFA that) {
    std::map<Node*, int> thisIdx, thatIdx;
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
                nodes[idx]->trans[ch] = nodes[tidx].get();
            }
        }
    }

    minimize();
}

void DFA::stripUnreachable() {
    std::set<Node*> used;
    std::function<void(Node*)> dfs = [&](Node *v) {
        used.insert(v);
        for (auto &[ch, to] : v->trans) {
            if (used.find(to) == used.end()) {
                dfs(to);
            }
        }
    };

    dfs(nodes[0].get());
    for (int i = 0; i < nodes.size();) {
        if (used.find(nodes[i].get()) == used.end()) {
            std::swap(nodes[i], nodes.back());
            nodes.pop_back();
        } else {
            ++i;
        }
    }
}

void DFA::minimize() {
    // https://en.wikipedia.org/wiki/DFA_minimization#Hopcroft's_algorithm

    stripUnreachable();

    std::map<Node*, int> indexes;
    std::vector<std::set<int>> p(2);
    for (auto &node : nodes) {
        int idx = indexes[node.get()] = indexes.size();
        if (node->term) {
            p[0].insert(idx);
        } else {
            p[1].insert(idx);
        }
    }

    auto w = p;

    while (!w.empty()) {
        auto a = w.back();
        w.pop_back();
        if (a.empty()) continue;

        std::map<int, std::set<int>> xs;

        for (auto &node : nodes) {
            for (auto &[ch, to] : node->trans) {
                auto idx = indexes[to];
                if (a.find(idx) != a.end()) {
                    xs[ch].insert(indexes[node.get()]);
                }
            }
        }

        for (auto &[ch, x] : xs) {
            for (int j = 0; j < p.size();) {
                auto &y = p[j];
                std::set<int> s1, s2;

                for (int i : y) {
                    if (x.find(i) != x.end()) {
                        s1.insert(i);
                    } else {
                        s2.insert(i);
                    }
                }

                if (s1.empty() || s2.empty()) {
                    ++j;
                    continue;
                }

                p.push_back(s1);
                p.push_back(s2);

                auto wit = std::find(w.begin(), w.end(), y);

                if (wit != w.end()) {
                    w.erase(wit);
                    w.push_back(s1);
                    w.push_back(s2);
                } else {
                    if (s1.size() <= s2.size()) {
                        w.push_back(s1);
                    } else {
                        w.push_back(s2);
                    }
                }

                p.erase(p.begin() + j);
            }
        }
    }

    std::vector<int> equiv(nodes.size());
    for (auto &s : p) {
        if (s.empty()) continue;
        for (int i : s) {
            equiv[i] = *s.begin();
        }
    }

    for (auto &node : nodes) {
        for (auto &[ch, to] : node->trans) {
            to = nodes[equiv[indexes[to]]].get();
        }
    }

    std::vector<std::unique_ptr<Node>> newNodes;

    for (int i = 0; i < nodes.size(); ++i) {
        if (equiv[i] != i) continue;
        newNodes.emplace_back(std::move(nodes[i]));
    }

    nodes = std::move(newNodes);
}

bool DFA::accepts(const std::string &s) const {
    auto state = nodes[0].get();

    for (char c : s) {
        if (state->trans.find(c) != state->trans.end()) {
            state = state->trans[c];
        } else {
            return false;
        }
    }

    return state->term;
}

int DFA::size() const {
    return nodes.size();
}

std::ostream &operator<<(std::ostream &os, const DFA &dfa) {
    // print DFA in DOT graph format

    os << "digraph DFA {" << std::endl;

    std::map<DFA::Node*, int> indexes;
    bool f = false;
    for (auto &node : dfa.nodes) {
        indexes[node.get()] = indexes.size();
        if (node->term) {
            f = true;
        }
    }
    if (f) {
        os << "node [shape = doublecircle];";
        for (auto &node : dfa.nodes) {
            if (node->term) {
                os << " " << indexes[node.get()];
            }
        }
        os << ";" << std::endl;
    }
    os << "node [shape = circle];" << std::endl;

    for (auto &node : dfa.nodes) {
        int fromIdx = indexes[node.get()];
        for (auto [ch, to] : node->trans) {
            int toIdx = indexes[to];
            os << fromIdx << " -> " << toIdx;
            std::string label{(char)ch};
            os << " [label = \"" << label << "\" ];" << std::endl;
        }
    }

    os << "}" << std::endl;
    return os;
}
