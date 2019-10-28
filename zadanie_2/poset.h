//
// Created by tomasz on 2019. 10. 28..
//

#ifndef POSET_POSET_H
#define POSET_POSET_H

#ifndef __cplusplus
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#endif

#ifdef __cplusplus
#include <iostream>
//    namespace jnp1;
extern "C"{
#endif

uint32_t poset_new();

void poset_delete(uint32_t id);

size_t poset_size(uint32_t id);

bool poset_insert(uint32_t id, const char *value);

bool poset_test(uint32_t id, const char *value1, const char *value2);

bool poset_add(uint32_t id, char const *value1, char const *value2);

bool poset_del(uint32_t id, char const *value1, char const *value2);

bool poset_remove(uint32_t id, const char *value);

void poset_clear(uint32_t id);

#ifdef __cplusplus
}
//}
#endif

#endif //POSET_POSET_H
