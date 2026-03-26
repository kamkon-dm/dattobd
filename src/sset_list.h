// SPDX-License-Identifier: GPL-2.0-only

/*
 * Copyright (C) 2022 Datto Inc.
 */

#ifndef SSET_LIST_H_
#define SSET_LIST_H_

#include <linux/types.h>
#include "logging.h"

struct sector_set {
        struct sector_set *next;
        sector_t sect;
        unsigned int len;
};

struct sset_list {
        struct sector_set *head;
        struct sector_set *tail;
};

void sset_list_init(struct sset_list *sl);

int sset_list_empty(const struct sset_list *sl);

void sset_list_add(struct sset_list *sl, struct sector_set *sset);

struct sector_set *sset_list_pop(struct sset_list *sl);

/* Static array of sector ranges management
*   void srng_init(struct srng_array *srng_a) - init array with zeros
*   int srng_add(struct srng_array *srng_a, struct srng_range *srng) - add new range element to the array
*   int srng_check(struct srng_array *srng_a, struct srng_range *srng) - check if given range is in the array
*/

// The higher value the more time is needed for sectors ranges checking and more memory is consumed
#define SRNG_MAX_COUNT (200)

struct srng_range {
    u64 sect;       // first sector of a range
    u64 size;       // in sectors
};

struct srng_array {
    unsigned int curr_idx;
    unsigned int cnt;
    struct srng_range rng[SRNG_MAX_COUNT];
    struct rcu_head rcu;
};

void srng_init(struct srng_array* srng_a);

int srng_add(struct srng_array* srng_a, struct srng_range* srng);

int srng_check(struct srng_array* srng_a, struct srng_range* srng);

#endif /* SSET_LIST_H_ */
