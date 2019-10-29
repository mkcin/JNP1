//
// Created by tomasz on 2019. 10. 28..
//

#include "poset.h"

#include <iostream>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <utility>

using neighbours_set_t = std::unordered_set<uint64_t>;
using poset_t = std::unordered_map <uint64_t, neighbours_set_t>;
using string_id_map_t = std::unordered_map <std::string, uint64_t>;

std::unordered_map <uint32_t, poset_t > &get_posets() {
    static std::unordered_map <uint32_t, poset_t > posets;
    return posets;
}

std::unordered_map <uint32_t, poset_t > &get_transposed() {
    static std::unordered_map <uint32_t, poset_t > transposed;
    return transposed;
}

std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > &get_string_map() {
    static std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > string_maps;
    return string_maps;
}

uint32_t next_poset_id;

uint32_t &get_next_poset_id() {
    static uint32_t next_poset_id;
    return next_poset_id;
}

void print_string_map() {
    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > &string_maps = get_string_map();
    std::cout << "STRING MAP\n";
    for (auto it = string_maps.begin(); it != string_maps.end(); ++it) {
        std::cout << it->first << " " << it->second.second << ":\n";
        for (auto it2 = it->second.first.begin(); it2 != it->second.first.end(); ++it2) {
            std::cout << it2->first << " " << it2->second << "\n";
        }
    }
}

void print_posets() {
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();
    std::cout << "POSETS\n";
    for (auto it = posets.begin(); it != posets.end(); ++it) {
        std::cout << it->first << ":\n";
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
            std::cout << it2->first << " " << it2->second.size() << "\n";
        }
    }
}

void print_transposed() {
    std::unordered_map <uint32_t, poset_t > &transposed = get_transposed();
    std::cout << "TRANSPOSED\n";
    for (auto it = transposed.begin(); it != transposed.end(); ++it) {
        std::cout << it->first << ":\n";
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
            std::cout << it2->first << " " << it2->second.size() << "\n";
        }
    }
}

bool same_value(const char *v1, const char *v2) {
    return strcmp(v1, v2) == 0;
}

bool poset_exists(uint32_t id) {
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();
    return posets.count(id) > 0;
}

uint32_t poset_new() {
    uint32_t &next_poset_id = get_next_poset_id();
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();
    std::unordered_map <uint32_t, poset_t > &transposed = get_transposed();
    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > &string_maps = get_string_map();

    posets[next_poset_id].clear();
    transposed[next_poset_id].clear();
    std::pair<string_id_map_t, uint64_t> &string_map_id = string_maps[next_poset_id];
    string_map_id.first.clear();
    string_map_id.second = 0;
    return next_poset_id++;
}

void poset_delete(uint32_t id) {
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();
    std::unordered_map <uint32_t, poset_t > &transposed = get_transposed();
    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > &string_maps = get_string_map();

    posets.erase(id);
    transposed.erase(id);
    string_maps.erase(id);
    return;
}

size_t poset_size(uint32_t id) {
    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > &string_maps = get_string_map();

    if (string_maps.count(id)) {
        return string_maps[id].first.size();
    }
    return 0;
}

bool poset_insert(uint32_t id, const char *value) {
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();
    std::unordered_map <uint32_t, poset_t > &transposed = get_transposed();
    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > &string_maps = get_string_map();

    if(!value) {
        return false;
    }
    if (!poset_exists(id)) {
        return false;
    }
    std::pair<string_id_map_t, uint64_t> &string_map_id = string_maps[id];
    if (string_map_id.first.count(value)) {
        return false;
    }
    uint32_t string_id = string_map_id.second++;
    string_map_id.first[value] = string_id;
    posets[id][string_id].clear();
    transposed[id][string_id].clear();
    return true;
}

bool poset_test(uint32_t id, const char *value1, const char *value2) {
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();
    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > &string_maps = get_string_map();

    if(!value1 || !value2) {
        return false;
    }
    if (!poset_exists(id)) {
        return false;
    }
    string_id_map_t &m = string_maps[id].first;
    if (m.count(value1) == 0
        || m.count(value2) == 0) {
        return false;
    }
    if(same_value(value1, value2)) {
        return true;
    }
    uint64_t v1 = m[value1];
    uint64_t v2 = m[value2];
    neighbours_set_t &outgoing = posets[id][v1];
    return outgoing.find(v2) != outgoing.end();
}

void transitive_closure(uint32_t id, uint64_t v1, uint64_t v2) {
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();

    poset_t &posets_id = posets[id];
    neighbours_set_t &outgoing1 = posets_id[v1];
    neighbours_set_t &outgoing2 = posets_id[v2];

    for (auto it = outgoing2.begin(); it != outgoing2.end(); ++it) {
        if (outgoing1.find(*it) == outgoing1.end()) {
            outgoing1.insert(*it);
            posets_id[*it].insert(v1);
        }
    }
}

bool poset_add(uint32_t id, char const *value1, char const *value2) {
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();
    std::unordered_map <uint32_t, poset_t > &transposed = get_transposed();
    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > &string_maps = get_string_map();

    if(!value1 || !value2) {
        return false;
    }
    if (!poset_exists(id)) {
        return false;
    }
    string_id_map_t &m = string_maps[id].first;
    if (m.count(value1) == 0
        || m.count(value2) == 0) {
        return false;
    }
    if (poset_test(id, value1, value2) || poset_test(id, value2, value1)) {
        return false;
    }
    poset_t &transposed_id = transposed[id];
    uint64_t v1 = m[value1];
    uint64_t v2 = m[value2];
    posets[id][v1].insert(v2);
    transposed_id[v2].insert(v1);
    transitive_closure(id, v1, v2);
    neighbours_set_t &incoming = transposed_id[v1];
    for (auto it = incoming.begin(); it != incoming.end(); ++it) {
        transitive_closure(id, *it, v1);
    }
    return true;
}

bool longer_path(uint32_t id, uint64_t v1, uint64_t v2) {
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();

    poset_t &posets_id = posets[id];
    neighbours_set_t &outgoing = posets_id[v1];

    for (auto it = outgoing.begin(); it != outgoing.end(); ++it) {
        for (auto it2 = posets_id[*it].begin(); it2 != posets_id[*it].end(); ++it2) {
            if (*it2 == v2) {
              return true;
            }
        }
    }
    return false;
}

bool poset_del(uint32_t id, char const *value1, char const *value2) {
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();
    std::unordered_map <uint32_t, poset_t > &transposed = get_transposed();
    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > &string_maps = get_string_map();

    if(!value1 || !value2) {
        return false;
    }
    if (!poset_exists(id)) {
        return false;
    }
    string_id_map_t &m = string_maps[id].first;
    if (m.count(value1) == 0
        || m.count(value2) == 0) {
        return false;
    }
    if (!poset_test(id, value1, value2)) {
        return false;
    }
    uint64_t v1 = m[value1];
    uint64_t v2 = m[value2];
    if (longer_path(id, v1, v2)) {
        return false;
    }
    posets[id][v1].erase(v2);
    transposed[id][v2].erase(v1);
    return true;
}

bool poset_remove(uint32_t id, const char *value) {
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();
    std::unordered_map <uint32_t, poset_t > &transposed = get_transposed();
    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > &string_maps = get_string_map();

    if(!value) {
        return false;
    }
    if (!poset_exists(id)) {
        return false;
    }
    string_id_map_t &m = string_maps[id].first;
    if (m.count(value) == 0) {
        return false;
    }

    uint64_t v = m[value];
    poset_t &posets_id = posets[id];
    neighbours_set_t &outgoing = posets_id[v];
    poset_t &transposed_id = transposed[id];
    neighbours_set_t &incoming = transposed_id[v];

    for (auto it = outgoing.begin(); it != outgoing.end(); ++it) {
        transposed_id[*it].erase(v);
    }
    posets[id].erase(v);
    for (auto it = incoming.begin(); it != incoming.end(); ++it) {
        posets_id[*it].erase(v);
    }
    transposed_id.erase(v);
    m.erase(value);
    return true;
}

void poset_clear(uint32_t id) {
    std::unordered_map <uint32_t, poset_t > &posets = get_posets();
    std::unordered_map <uint32_t, poset_t > &transposed = get_transposed();
    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t> > &string_maps = get_string_map();

    if (poset_exists(id)) {
        posets[id].clear();
        transposed[id].clear();
        std::pair<string_id_map_t, uint64_t> &string_map_id = string_maps[id];
        string_map_id.first.clear();
        string_map_id.second;
    }
    return;
}

int main() {
    poset_new();
    std::cout << poset_size(0) << "\n";
    const char *a = "a";
    poset_insert(0, a);
    std::cout << poset_size(0) << "\n";
    const char *b = "b";
    poset_insert(0, b);
    std::cout << poset_size(0) << "\n";
    const char *c = "c";
    poset_insert(0, c);
    std::cout << poset_size(0) << "\n";
    const char *d = "d";
    poset_insert(0, d);
    std::cout << poset_size(0) << "\n";
    const char *e = "e";
    poset_insert(0, e);
    std::cout << poset_size(0) << "\n";
    const char *f = "f";
    poset_insert(0, f);

    std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a b c d e f
    poset_add(0, a, b);
    std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a<b c d e f
    poset_add(0, b, c);
    std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a<b<c d e f
    poset_add(0, d, e);
    std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a<b<c d<e f
    poset_add(0, e, f);
    std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a<b<c d<e<f
    poset_add(0, c, d);
    std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a<b<c<d<e<f

    print_string_map();
    print_posets();
    print_transposed();

    // std::cout << "usuwam a: " << poset_remove(0, a) << "\n";
    // print_string_map();
    // print_posets();
    // print_transposed();
    // std::cout << "usuwam b: " << poset_remove(0, b) << "\n";
    // print_string_map();
    // print_posets();
    // print_transposed();
    // std::cout << "usuwam c: " << poset_remove(0, c) << "\n";
    // print_string_map();
    // print_posets();
    // print_transposed();
    // std::cout << "usuwam d: " << poset_remove(0, d) << "\n";
    // print_string_map();
    // print_posets();
    // print_transposed();
    // std::cout << "usuwam e: " << poset_remove(0, e) << "\n";
    // print_string_map();
    // print_posets();
    // print_transposed();
    // std::cout << "usuwam f: " << poset_remove(0, f) << "\n";
    // print_string_map();
    // print_posets();
    // print_transposed();

    std::cout << "usuwam a < f: " << poset_del(0, a, f) << "\n";
    std::cout << "usuwam a < b: " << poset_del(0, a, b) << "\n";
    std::cout << "usuwam a < c: " << poset_del(0, a, c) << "\n";
    std::cout << "usuwam a < d: " << poset_del(0, a, d) << "\n";
    std::cout << "usuwam a < e: " << poset_del(0, a, e) << "\n";
    std::cout << "usuwam a < f: " << poset_del(0, a, f) << "\n";

}
