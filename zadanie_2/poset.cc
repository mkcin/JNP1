//#include <iostream>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <utility>
#include "poset.h"

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

    //Mapa, która przechowuje grafy reprezentujące posety
    std::unordered_map <uint32_t, poset_t> &get_posets() {
        static std::unordered_map <uint32_t, poset_t> posets;
        return posets;
    }

    //Mapa, która przechowuje transponowane graft reprezentujące posety
    std::unordered_map <uint32_t, poset_t> &get_transposed() {
        static std::unordered_map <uint32_t, poset_t> transposed;
        return transposed;
    }

    //Mapa, która dla każdego posetu przechowuje stringi z nazwami i nadaje im identyfikator
    std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &get_string_map() {
        static std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> string_maps;
        return string_maps;
    }

    //Zmienna służąca nadaniu unikatowych id dla nowych posetów
    uint32_t &get_next_poset_id() {
        static uint32_t next_poset_id;
        return next_poset_id;
    }

    bool same_value(const char *v1, const char *v2) {
        return strcmp(v1, v2) == 0;
    }

    //Sprawdza istnienie posetu o danym id
    bool poset_exists(uint32_t id) {
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        return posets.count(id) > 0;
    }

    //Jeżeli istnieje ścieżka w fanym posecie z v1 do v2 długości 2 zwraca true, w przeciwnym
    //przypadku zwraca false.
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

    //Domyka relację przechodnio. Dodaje relacje z v1 do wszystkich elementów
    //z którymi v2 jest w relacji.
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

    //Pomocnicza funkcja sprawdzająca czy value1 i value2 sa w relacji o indeksie id
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
        if (debug) {
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
        if (debug) {
            std::cerr << "poset_new: poset " << next_poset_id << " created\n";
        }
        return next_poset_id++;
    }

    void poset_delete(uint32_t id) {
        cerr_init();
        if (debug) {
            std::cerr << "poset_delete(" << id << ")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (poset_exists(id)) {
            posets.erase(id);
            transposed.erase(id);
            string_maps.erase(id);
            if (debug) {
                std::cerr << "poset_delete: poset " << id << " deleted\n";
            }
            return;
        }
        if (debug) {
            std::cerr << "poset_delete: poset " << id << " does  not exist\n";
        }
        return;
    }

    size_t poset_size(uint32_t id) {
        cerr_init();
        if (debug) {
            std::cerr << "poset_size(" << id << ")\n";
        }
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (string_maps.count(id)) {
            size_t s = string_maps[id].first.size();
            if (debug) {
                std::cerr << "poset_size: poset " << id << " contains " << s << " elements\n";
            }
            return s;
        }
        if (debug) {
            std::cerr << "poset_size: poset " << id << " does not exist\n";
        }
        return 0;
    }

    bool poset_insert(uint32_t id, const char *value) {
        cerr_init();
        if (debug) {
            std::cerr << "poset_insert(" << id << ", \"";
            if (!value) std::cerr << "NULL" << ")\n";
            else std::cerr << value << "\")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (!value) {
            if (debug) {
                std::cerr << "poset_insert: invalid value (NULL)\n";
            }
            return false;
        }
        if (!poset_exists(id)) {
            if (debug) {
                std::cerr << "poset_insert: poset " << id << " does not exist\n";
            }
            return false;
        }
        std::pair <string_id_map_t, uint64_t> &string_map_id = string_maps[id];
        if (string_map_id.first.count(value)) {
            if (debug) {
                std::cerr << "poset_insert: poset " << id << ", element \"";
                std::cerr << value << "\" already exists\n";
            }
            return false;
        }
        uint32_t string_id = string_map_id.second++;
        string_map_id.first[value] = string_id;
        posets[id][string_id].clear();
        transposed[id][string_id].clear();
        if (debug) {
            std::cerr << "poset_insert: poset " << id << ", element \"";
            std::cerr << value << "\" inserted\n";
        }
        return true;
    }

    bool poset_test(uint32_t id, const char *value1, const char *value2) {
        cerr_init();
        if (debug) {
            std::cerr << "poset_test(" << id << ", ";
            if (!value1) std::cerr << "NULL, ";
            else std::cerr << "\"" << value1 << "\", ";
            if (!value2) std::cerr << "NULL)\n";
            else std::cerr << "\"" << value2 << "\")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (!value1 || !value2) {
            if (debug && !value1) {
                std::cerr << "poset_test: invalid value1 (NULL)\n";
            }
            if (debug && !value2) {
                std::cerr << "poset_test: invalid value2 (NULL)\n";
            }
            return false;
        }
        if (!poset_exists(id)) {
            if (debug) {
                std::cerr << "poset_test: poset " << id << " does not exist\n";
            }
            return false;
        }
        string_id_map_t &m = string_maps[id].first;
        if (m.count(value1) == 0
            || m.count(value2) == 0) {
            if (debug) {
                std::cerr << "poset_test: poset " << id << ", element \"";
                std::cerr << value1 << "\" or \"" << value2 << "\" does not exist\n";
            }
            return false;
        }
        bool relation_exitst = false;
        if (same_value(value1, value2)) {
            relation_exitst = true;
        } else {
            uint64_t v1 = m[value1];
            uint64_t v2 = m[value2];
            neighbours_set_t &outgoing = posets[id][v1];
            if (outgoing.find(v2) != outgoing.end()) {
                relation_exitst = true;
            }
        }
        if (debug) {
            std::cerr << "poset_test: poset " << id << ", relation (\"";
            std::cerr << value1 << "\", \"" << value2 << "\") ";
            if (relation_exitst) {
                std::cerr << "exists\n";
            } else {
                std::cerr << "does not exist\n";
            }
        }
        return relation_exitst;
    }

    bool poset_add(uint32_t id, char const *value1, char const *value2) {
        cerr_init();
        if (debug) {
            std::cerr << "poset_add(" << id << ", ";
            if (!value1) std::cerr << "NULL, ";
            else std::cerr << "\"" << value1 << "\", ";
            if (!value2) std::cerr << "NULL)\n";
            else std::cerr << "\"" << value2 << "\")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (!value1 || !value2) {
            if (debug && !value1) {
                std::cerr << "poset_add: invalid value1 (NULL)\n";
            }
            if (debug && !value2) {
                std::cerr << "poset_add: invalid value2 (NULL)\n";
            }
            return false;
        }
        if (!poset_exists(id)) {
            if (debug) {
                std::cerr << "poset_add: poset " << id << " does not exist\n";
            }
            return false;
        }
        string_id_map_t &m = string_maps[id].first;
        if (m.count(value1) == 0
            || m.count(value2) == 0) {
            if (debug) {
                std::cerr << "poset_add: poset " << id << ", element \"";
                std::cerr << value1 << "\" or \"" << value2 << "\" does not exist\n";
            }
            return false;
        }
        if (poset_test_internal(id, value1, value2) || poset_test_internal(id, value2, value1)) {
            if (debug) {
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
        if (debug) {
            std::cerr << "poset_add: poset " << id << ", relation (\"";
            std::cerr << value1 << "\", \"" << value2 << "\") added\n";
        }
        return true;
    }

    bool poset_del(uint32_t id, char const *value1, char const *value2) {
        cerr_init();
        if (debug) {
            std::cerr << "poset_del(" << id << ", ";
            if (!value1) std::cerr << "NULL, ";
            else std::cerr << "\"" << value1 << "\", ";
            if (!value2) std::cerr << "NULL)\n";
            else std::cerr << "\"" << value2 << "\")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (!value1 || !value2) {
            if (debug && !value1) {
                std::cerr << "poset_del: invalid value1 (NULL)\n";
            }
            if (debug && !value2) {
                std::cerr << "poset_del: invalid value2 (NULL)\n";
            }
            return false;
        }
        if (!poset_exists(id)) {
            if (debug) {
                std::cerr << "poset_del: poset " << id << " does not exist\n";
            }
            return false;
        }
        string_id_map_t &m = string_maps[id].first;
        if (m.count(value1) == 0
            || m.count(value2) == 0) {
            if (debug) {
                std::cerr << "poset_del: poset " << id << ", element \"";
                std::cerr << value1 << "\" or \"" << value2 << "\" does not exist\n";
            }
            return false;
        }
        bool can_be_deleted = true;
        if (!poset_test_internal(id, value1, value2) || same_value(value1, value2)) {
            can_be_deleted = false;
        } else {
            uint64_t v1 = m[value1];
            uint64_t v2 = m[value2];
            if (longer_path(id, v1, v2)) {
                can_be_deleted = false;
            } else {
                posets[id][v1].erase(v2);
                transposed[id][v2].erase(v1);
            }
        }
        if (debug) {
            std::cerr << "poset_del: poset " << id << ", relation (\"";
            std::cerr << value1 << "\", \"" << value2 << "\") ";
            if (can_be_deleted) {
                std::cerr << "deleted\n";
            } else {
                std::cerr << "cannot be deleted\n";
            }
        }
        return can_be_deleted;
    }

    bool poset_remove(uint32_t id, const char *value) {
        cerr_init();
        if (debug) {
            std::cerr << "poset_remove(" << id << ", ";
            if (!value) std::cerr << "NULL" << ")\n";
            else std::cerr << "\"" << value << "\")\n";
        }
        std::unordered_map <uint32_t, poset_t> &posets = get_posets();
        std::unordered_map <uint32_t, poset_t> &transposed = get_transposed();
        std::unordered_map <uint32_t, std::pair<string_id_map_t, uint64_t>> &string_maps = get_string_map();

        if (!value) {
            if (debug) {
                std::cerr << "poset_remove: invalid value (NULL)\n";
            }
            return false;
        }
        if (!poset_exists(id)) {
            if (debug) {
                std::cerr << "poset_remove: poset " << id << " does not exist\n";
            }
            return false;
        }
        string_id_map_t &m = string_maps[id].first;
        if (m.count(value) == 0) {
            if (debug) {
                std::cerr << "poset_remove: poset " << id << ", element \"";
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
        if (debug) {
            std::cerr << "poset_remove: poset " << id << ", element \"";
            std::cerr << value << "\" removed\n";
        }
        return true;
    }

    void poset_clear(uint32_t id) {
        cerr_init();
        if (debug) {
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
            if (debug) {
                std::cerr << "poset_clear: poset " << id << " cleared\n";
            }
        } else if (debug) {
            std::cerr << "poset_clear: poset " << id << " does not exist\n";
        }
        return;
    }
}
