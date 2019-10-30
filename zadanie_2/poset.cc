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

namespace {

    #ifndef NDEBUG
        const bool debug = false;
    #else
        const bool debug = true;
    #endif

    void cerr_init() {
        static std::ios_base::Init init;
    }

    using neighbours_set_t = std::unordered_set<uint64_t>;
    using poset_t = std::unordered_map<uint64_t, neighbours_set_t>;
    using string_id_map_t = std::unordered_map<std::string, uint64_t>;

    std::unordered_map <uint32_t, poset_t> &get_posets() {
        static std::unordered_map <uint32_t, poset_t> posets;
        return posets;
    }

    std::unordered_map <uint32_t, poset_t> &get_transposed() {
        static std::unordered_map <uint32_t, poset_t> transposed;
        return transposed;
    }

    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &get_string_map() {
        static std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> string_maps;
        return string_maps;
    }

    uint32_t &get_next_poset_id() {
        static uint32_t next_poset_id;
        return next_poset_id;
    }

    bool same_value(const char *v1, const char *v2) {
        return strcmp(v1, v2) == 0;
    }

    bool poset_exists(uint32_t id) {
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        return posets.count(id) > 0;
    }

    bool longer_path(uint32_t id, uint64_t v1, uint64_t v2) {
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();

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

    void transitive_closure(uint32_t id, uint64_t v1, uint64_t v2) {
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();

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

    bool poset_test_internal(uint32_t id, const char *value1, const char *value2) {
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();
        string_id_map_t &m = string_maps[id].first;

        if (same_value(value1, value2)) {
            return true;
        }
        uint64_t v1 = m[value1];
        uint64_t v2 = m[value2];
        neighbours_set_t &outgoing = posets[id][v1];
        return outgoing.find(v2) != outgoing.end();
    }
}

namespace jnp1 {
    uint32_t poset_new() {
        cerr_init();
        if(debug) {
            std::cerr << "poset_new()\n";
        }
        uint32_t &next_poset_id = get_next_poset_id();
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        posets[next_poset_id].clear();
        transposed[next_poset_id].clear();
        std::pair <string_id_map_t, uint64_t> &string_map_id = string_maps[next_poset_id];
        string_map_id.first.clear();
        string_map_id.second = 0;
        if(debug) {
            std::cerr << "poset_new: poset " << next_poset_id << " created\n";
        }
        return next_poset_id++;
    }

    void poset_delete(uint32_t id) {
        cerr_init();
        if(debug) {
            std::cerr << "poset_delete(" << id << ")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if(poset_exists(id)) {
            posets.erase(id);
            transposed.erase(id);
            string_maps.erase(id);
            if(debug) {
                std::cerr << "poset_delete: poset " << id << " deleted\n";
            }
            return;
        }
        if(debug) {
            std::cerr << "poset_delete: poset " << id << " does  not exist\n";
        }
        return;
    }

    size_t poset_size(uint32_t id) {
        cerr_init();
        if(debug) {
            std::cerr << "poset_size(" << id << ")\n";
        }
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (string_maps.count(id)) {
            size_t s = string_maps[id].first.size();
            if(debug) {
                std::cerr << "poset_size: poset " << id << " contains " << s << " elements\n";
            }
            return s;
        }
        if(debug) {
            std::cerr << "poset_size: poset " << id << " does not exist\n";
        }
        return 0;
    }

    bool poset_insert(uint32_t id, const char *value) {
        cerr_init();
        if(debug) {
            std::cerr << "poset_insert(" << id << ", \"";
            if(!value) std::cerr << "NULL" << ")\n";
            else std::cerr << value << "\")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (!value) {
            if(debug) {
                std::cerr << "poset_insert: invalid value (NULL)\n";
            }
            return false;
        }
        if (!poset_exists(id)) {
            if(debug) {
                std::cerr << "poset_insert: poset " << id << " does not exist\n";
            }
            return false;
        }
        std::pair <string_id_map_t, uint64_t> &string_map_id = string_maps[id];
        if (string_map_id.first.count(value)) {
            if(debug) {
                std::cerr << "poset_insert: poset " << id << ", element \"";
                std::cerr << value << "\" already exists\n";
            }
            return false;
        }
        uint32_t string_id = string_map_id.second++;
        string_map_id.first[value] = string_id;
        posets[id][string_id].clear();
        transposed[id][string_id].clear();
        if(debug) {
            std::cerr << "poset_insert: poset " << id << ", element \"";
            std::cerr << value << "\" inserted\n";
        }
        return true;
    }

    bool poset_test(uint32_t id, const char *value1, const char *value2) {
        cerr_init();
        if(debug) {
            std::cerr << "poset_test(" << id << ", ";
            if(!value1) std::cerr << "NULL, ";
            else std::cerr << "\"" << value1 << "\", ";
            if(!value2) std::cerr << "NULL)\n";
            else std::cerr << "\"" << value2 << "\")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (!value1 || !value2) {
            if(debug && !value1) {
                std::cerr << "poset_test: invalid value1 (NULL)\n";
            }
            if(debug && !value2) {
                std::cerr << "poset_test: invalid value2 (NULL)\n";
            }
            return false;
        }
        if (!poset_exists(id)) {
            if(debug) {
                std::cerr << "poset_test: poset " << id << " does not exist\n";
            }
            return false;
        }
        string_id_map_t &m = string_maps[id].first;
        if (m.count(value1) == 0
            || m.count(value2) == 0) {
            if(debug) {
                std::cerr << "poset_test: poset " << id << ", element \"";
                std::cerr << value1 << "\" or \"" << value2 << "\" does not exist\n";
            }
            return false;
        }
        bool relation_exitst = false;
        if (same_value(value1, value2)) {
            relation_exitst = true;
        }
        else {
            uint64_t v1 = m[value1];
            uint64_t v2 = m[value2];
            neighbours_set_t &outgoing = posets[id][v1];
            if(outgoing.find(v2) != outgoing.end()) {
                relation_exitst = true;
            }
        }
        if(debug) {
            std::cerr << "poset_test: poset " << id << ", relation (\"";
            std::cerr << value1 << "\", \"" << value2 << "\") ";
            if(relation_exitst) {
                std::cerr << "exists\n";
            }
            else {
                std::cerr << "does not exist\n";
            }
        }
        return relation_exitst;
    }

    bool poset_add(uint32_t id, char const *value1, char const *value2) {
        cerr_init();
        if(debug) {
            std::cerr << "poset_add(" << id << ", ";
            if(!value1) std::cerr << "NULL, ";
            else std::cerr << "\"" << value1 << "\", ";
            if(!value2) std::cerr << "NULL)\n";
            else std::cerr << "\"" << value2 << "\")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (!value1 || !value2) {
            if(debug && !value1) {
                std::cerr << "poset_add: invalid value1 (NULL)\n";
            }
            if(debug && !value2) {
                std::cerr << "poset_add: invalid value2 (NULL)\n";
            }
            return false;
        }
        if (!poset_exists(id)) {
            if(debug) {
                std::cerr << "poset_add: poset " << id << " does not exist\n";
            }
            return false;
        }
        string_id_map_t &m = string_maps[id].first;
        if (m.count(value1) == 0
            || m.count(value2) == 0) {
            if(debug) {
                std::cerr << "poset_add: poset " << id << ", element \"";
                std::cerr << value1 << "\" or \"" << value2 << "\" does not exist\n";
            }
            return false;
        }
        if (poset_test_internal(id, value1, value2) || poset_test_internal(id, value2, value1)) {
            if(debug) {
                std::cerr << "poset_add: poset " << id << ", relation (\"";
                std::cerr << value1 << "\", \"" << value2 << "\") cannot be added3\n";
            }
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
        if(debug) {
            std::cerr << "poset_add: poset " << id << ", relation (\"";
            std::cerr << value1 << "\", \"" << value2 << "\") added\n";
        }
        return true;
    }

    bool poset_del(uint32_t id, char const *value1, char const *value2) {
        cerr_init();
        if(debug) {
            std::cerr << "poset_del(" << id << ", ";
            if(!value1) std::cerr << "NULL, ";
            else std::cerr << "\"" << value1 << "\", ";
            if(!value2) std::cerr << "NULL)\n";
            else std::cerr << "\"" << value2 << "\")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (!value1 || !value2) {
            if(debug && !value1) {
                std::cerr << "poset_del: invalid value1 (NULL)\n";
            }
            if(debug && !value2) {
                std::cerr << "poset_del: invalid value2 (NULL)\n";
            }
            return false;
        }
        if (!poset_exists(id)) {
            if(debug) {
                std::cerr << "poset_del: poset " << id << " does not exist\n";
            }
            return false;
        }
        string_id_map_t &m = string_maps[id].first;
        if (m.count(value1) == 0
            || m.count(value2) == 0) {
            if(debug) {
                std::cerr << "poset_del: poset " << id << ", element \"";
                std::cerr << value1 << "\" or \"" << value2 << "\" does not exist\n";
            }
            return false;
        }
        bool can_be_deleted = true;
        if (!poset_test_internal(id, value1, value2) || same_value(value1, value2)) {
            can_be_deleted = false;
        }
        else {
            uint64_t v1 = m[value1];
            uint64_t v2 = m[value2];
            if (longer_path(id, v1, v2)) {
                can_be_deleted = false;
            }
            else {
                posets[id][v1].erase(v2);
                transposed[id][v2].erase(v1);
            }
        }
        if(debug) {
            std::cerr << "poset_del: poset " << id << ", relation (\"";
            std::cerr << value1 << "\", \"" << value2 << "\") ";
            if(can_be_deleted) {
                std::cerr << "deleted\n";
            }
            else {
                std::cerr << "cannot be deleted\n";
            }
        }
        return can_be_deleted;
    }

    bool poset_remove(uint32_t id, const char *value) {
        cerr_init();
        if(debug) {
            std::cerr << "poset_remove(" << id << ", ";
            if(!value) std::cerr << "NULL" << ")\n";
            else std::cerr << "\"" << value << "\")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (!value) {
            if(debug) {
                std::cerr << "poset_remove: invalid value (NULL)\n";
            }
            return false;
        }
        if (!poset_exists(id)) {
            if(debug) {
                std::cerr << "poset_remove: poset " << id << " does not exist\n";
            }
            return false;
        }
        string_id_map_t &m = string_maps[id].first;
        if (m.count(value) == 0) {
            if(debug) {
                std::cerr << "poset_remove: poset "<< id << ", element \"";
                std::cerr << value << "\" does not exist\n";
            }
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
        if(debug) {
            std::cerr << "poset_remove: poset "<< id << ", element \"";
            std::cerr << value << "\" removed\n";
        }
        return true;
    }

    void poset_clear(uint32_t id) {
        cerr_init();
        if(debug) {
            std::cerr << "poset_clear(" << id << ")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (poset_exists(id)) {
            posets[id].clear();
            transposed[id].clear();
            std::pair <string_id_map_t, uint64_t> &string_map_id = string_maps[id];
            string_map_id.first.clear();
            string_map_id.second = 0;
            if(debug) {
                std::cerr << "poset_clear: poset " << id << " cleared\n";
            }
        }
        else if (debug) {
            std::cerr << "poset_clear: poset "<< id << " does not exist\n";
        }
        return;
    }
}

// int main() {
   // jnp1::poset_new();
   // std::cout << poset_size(0) << "\n";
   // const char *a = "a";
   // poset_insert(0, a);
   // std::cout << poset_size(0) << "\n";
   // const char *b = "b";
   // poset_insert(0, b);
   // std::cout << poset_size(0) << "\n";
   // const char *c = "c";
   // poset_insert(0, c);
   // std::cout << poset_size(0) << "\n";
   // const char *d = "d";
   // poset_insert(0, d);
   // std::cout << poset_size(0) << "\n";
   // const char *e = "e";
   // poset_insert(0, e);
   // std::cout << poset_size(0) << "\n";
   // const char *f = "f";
   // poset_insert(0, f);
   //
   // std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a b c d e f
   // poset_add(0, a, b);
   // std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a<b c d e f
   // poset_add(0, b, c);
   // std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a<b<c d e f
   // poset_add(0, d, e);
   // std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a<b<c d<e f
   // poset_add(0, e, f);
   // std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a<b<c d<e<f
   // poset_add(0, c, d);
   // std::cout << "czy a < f: " << poset_test(0, a, f) << "\n"; // a<b<c<d<e<f
   //
   // print_string_map();
   // print_posets();
   // print_transposed();
   //
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
   //
   // std::cout << "usuwam a < f: " << poset_del(0, a, f) << "\n";
   // std::cout << "usuwam a < b: " << poset_del(0, a, b) << "\n";
   // std::cout << "usuwam a < c: " << poset_del(0, a, c) << "\n";
   // std::cout << "usuwam a < d: " << poset_del(0, a, d) << "\n";
   // std::cout << "usuwam a < e: " << poset_del(0, a, e) << "\n";
   // std::cout << "usuwam a < f: " << poset_del(0, a, f) << "\n";

   // unsigned long p1;
   //
   // p1 = jnp1::poset_new();
   //
   // assert(jnp1::poset_size(p1) == 0);
   // assert(jnp1::poset_size(p1 + 1) == 0);
   // assert(!jnp1::poset_insert(p1, NULL));
   // assert(jnp1::poset_insert(p1, "A"));
   // assert(jnp1::poset_test(p1, "A", "A"));
   // assert(!jnp1::poset_insert(p1, "A"));
   // assert(!jnp1::poset_insert(p1 + 1, "B"));
   // assert(jnp1::poset_size(p1) == 1);
   // assert(!jnp1::poset_remove(p1 + 1, "A"));
   // assert(jnp1::poset_remove(p1, "A"));
   // assert(!jnp1::poset_remove(p1, "A"));
   // assert(jnp1::poset_insert(p1, "B"));
   // assert(jnp1::poset_insert(p1, "C"));
   // assert(jnp1::poset_add(p1, "B", "C"));
   // assert(!jnp1::poset_remove(p1, "A"));
   // assert(!jnp1::poset_add(p1, NULL, "X"));
   // assert(!jnp1::poset_del(p1, NULL, "X"));
   // assert(!jnp1::poset_test(p1, NULL, "X"));
   // assert(!jnp1::poset_add(p1, "X", NULL));
   // assert(!jnp1::poset_del(p1, "X", NULL));
   // assert(!jnp1::poset_test(p1, "X", NULL));
   // assert(!jnp1::poset_add(p1, NULL, NULL));
   // assert(!jnp1::poset_del(p1, NULL, NULL));
   // assert(!jnp1::poset_test(p1, NULL, NULL));
   // assert(!jnp1::poset_add(p1, "C", "D"));
   // assert(!jnp1::poset_add(p1, "D", "C"));
   // assert(!jnp1::poset_del(p1, "C", "D"));
   // assert(!jnp1::poset_del(p1, "D", "C"));
   // assert(!jnp1::poset_test(p1, "C", "D"));
   // assert(!jnp1::poset_test(p1, "D", "C"));
   // assert(!jnp1::poset_add(p1 + 1, "C", "D"));
   // assert(!jnp1::poset_del(p1 + 1, "C", "D"));
   // assert(!jnp1::poset_test(p1 + 1, "C", "D"));
   // jnp1::poset_clear(p1);
   // jnp1::poset_clear(p1 + 1);
   // assert(jnp1::poset_insert(p1, "E"));
   // assert(jnp1::poset_insert(p1, "F"));
   // assert(jnp1::poset_insert(p1, "G"));
   // assert(jnp1::poset_add(p1, "E", "F"));
   // assert(!jnp1::poset_add(p1, "E", "F"));
   // assert(!jnp1::poset_add(p1, "F", "E"));
   // assert(jnp1::poset_test(p1, "E", "F"));
   // assert(!jnp1::poset_test(p1, "F", "E"));
   // assert(jnp1::poset_add(p1, "F", "G"));
   // assert(jnp1::poset_test(p1, "E", "G"));
   // assert(!jnp1::poset_del(p1, "E", "G"));
   // assert(jnp1::poset_del(p1, "E", "F"));
   // assert(!jnp1::poset_del(p1, "E", "F"));
   // assert(!jnp1::poset_del(p1, "G", "F"));
   // assert(!jnp1::poset_del(p1, "G", "G"));
   // assert(jnp1::poset_size(p1) == 3);
   // jnp1::poset_delete(p1);
   // jnp1::poset_delete(p1);
   // jnp1::poset_delete(p1 + 1);

// }
