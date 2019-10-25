#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <bits/stdc++.h>

std::unordered_map<uint32_t, std::pair<std::unordered_map<std::string, uint64_t>, uint64_t> > string_map;

std::unordered_map<uint32_t, std::unordered_map<uint64_t, std::unordered_set<uint64_t> > > posets;

uint32_t next_poset_id;

bool poset_exists(uint32_t id) {
  return posets.count(id) > 0;
}

uint32_t poset_new() {
  posets[next_poset_id].clear();
  string_map[next_poset_id].first.clear();
  string_map[next_poset_id].second = 0;
  return next_poset_id++;
}

void poset_delete(uint32_t id) {
  posets.erase(id);
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
  std::string copy = value;
  if(!poset_exists(id)) {
    return false;
  }
  if(string_map[id].first.count(copy)) {
    return false;
  }
  string_map[id].first[copy] = string_map[id].second++;
  posets[id][string_map[id].first[copy]].clear();
  return true;
}

int main() {
  poset_new();
  std::cout << poset_size(0) << "\n";
  const char *a = "sadf";
  poset_insert(0, a);
  std::cout << poset_size(0) << "\n";
  const char *b = "lalala";
  poset_insert(0, a);
  std::cout << poset_size(0) << "\n";
  const char *c = "pokf";
  poset_insert(0, a);
}
