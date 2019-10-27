#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <utility>

std::unordered_map<uint32_t, std::pair<std::unordered_map<std::string, uint64_t>, uint64_t> > string_map;

std::unordered_map<uint32_t, std::unordered_map<uint64_t, std::unordered_set<uint64_t> > > posets;

std::unordered_map<uint32_t, std::unordered_map<uint64_t, std::unordered_set<uint64_t> > > transposed;

uint32_t next_poset_id;

void print_string_map() {
  std::cout << "STRING MAP\n";
  for(auto it = string_map.begin(); it != string_map.end(); it++) {
    std::cout << (*it).first << " " << (*it).second.second << ":\n";
    for(auto it2 = (*it).second.first.begin(); it2 != (*it).second.first.end(); it2++) {
      std::cout << (*it2).first << " " << (*it2).second << "\n";
    }
  }
}

void print_posets() {
  std::cout << "POSETS\n";
  for(auto it = posets.begin(); it != posets.end(); it++) {
    std::cout << (*it).first << ":\n";
    for(auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++) {
      std::cout << (*it2).first << " " << (*it2).second.size() << "\n";
    }
  }
}

void print_transposed() {
  std::cout << "TRANSPOSED\n";
  for(auto it = transposed.begin(); it != transposed.end(); it++) {
    std::cout << (*it).first << ":\n";
    for(auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++) {
      std::cout << (*it2).first << " " << (*it2).second.size() << "\n";
    }
  }
}

bool poset_exists(uint32_t id) {
  return posets.count(id) > 0;
}

uint32_t poset_new() {
  posets[next_poset_id].clear();
  transposed[next_poset_id].clear();
  string_map[next_poset_id].first.clear();
  string_map[next_poset_id].second = 0;
  return next_poset_id++;
}

void poset_delete(uint32_t id) {
  posets.erase(id);
  transposed.erase(id);
  string_map.erase(id);
  return;
}

size_t poset_size(uint32_t id) {
  if(string_map.count(id)) {
    return string_map[id].first.size();
  }
  return 0;
}

bool poset_insert(uint32_t id, const char *value) {
  if(!poset_exists(id)) {
    return false;
  }
  if(string_map[id].first.count(value)) {
    return false;
  }
  uint32_t string_id = string_map[id].second++;
  string_map[id].first[value] = string_id;
  posets[id][string_id].clear();
  transposed[id][string_id].clear();
  return true;
}

bool poset_test(uint32_t id, const char *value1, const char *value2) {
  if(!poset_exists(id)) {
    return false;
  }
  if(string_map[id].first.count(value1) == 0
    || string_map[id].first.count(value2) == 0) {
    return false;
  }
  uint64_t v1 = string_map[id].first[value1];
  uint64_t v2 = string_map[id].first[value2];
  return posets[id][v1].find(v2) != posets[id][v1].end();
}

void transitive_closure(uint32_t id, uint64_t v1, uint64_t v2) {
  for(auto it = posets[id][v2].begin(); it != posets[id][v2].end(); it++) {
    if(posets[id][v1].find((*it)) == posets[id][v1].end()) {
      posets[id][v1].insert((*it));
      transposed[id][(*it)].insert(v1);
    }
  }
}

bool poset_add(uint32_t id, char const *value1, char const *value2) {
  if(!poset_exists(id)) {
    return false;
  }
  if(string_map[id].first.count(value1) == 0
    || string_map[id].first.count(value2) == 0) {
    return false;
  }
  if(poset_test(id, value1, value2) || poset_test(id, value2, value1)) {
    return false;
  }
  uint64_t v1 = string_map[id].first[value1];
  uint64_t v2 = string_map[id].first[value2];
  posets[id][v1].insert(v2);
  transposed[id][v2].insert(v1);
  transitive_closure(id, v1, v2);
  for(auto it = transposed[id][v1].begin(); it != transposed[id][v1].end(); it++) {
    transitive_closure(id, (*it), v1);
  }
  return true;
}

bool longer_path(uint32_t id, uint64_t v1, uint64_t v2) {
  for(auto it = posets[id][v1].begin(); it != posets[id][v1].end(); it++) {
    for(auto it2 = posets[id][(*it)].begin(); it2 != posets[id][(*it)].end(); it2++) {
      if((*it2) == v2) return true;
    }
  }
  return false;
}

bool poset_del(uint32_t id, char const *value1, char const *value2) {
  if(!poset_exists(id)) {
    return false;
  }
  if(string_map[id].first.count(value1) == 0
    || string_map[id].first.count(value2) == 0) {
    return false;
  }
  if(!poset_test(id, value1, value2)) {
    return false;
  }
  uint64_t v1 = string_map[id].first[value1];
  uint64_t v2 = string_map[id].first[value2];
  if(longer_path(id, v1, v2)) {
    return false;
  }
  posets[id][v1].erase(v2);
  transposed[id][v2].erase(v1);
  return true;
}

bool poset_remove(uint32_t id, const char *value) {
  if(!poset_exists(id)) {
    return false;
  }
  if(string_map[id].first.count(value) == 0) {
    return false;
  }
  uint64_t v = string_map[id].first[value];
  for(auto it = posets[id][v].begin(); it != posets[id][v].end(); it++) {
    transposed[id][(*it)].erase(v);
  }
  posets[id].erase(v);
  for(auto it = transposed[id][v].begin(); it != transposed[id][v].end(); it++) {
    posets[id][(*it)].erase(v);
  }
  transposed[id].erase(v);
  string_map[id].first.erase(value);
  return true;
}

void poset_clear(uint32_t id) {
  if(poset_exists(id)) {
    posets.erase(id);
    transposed.erase(id);
    string_map.erase(id);
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
