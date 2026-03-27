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

// Basic sector range element
struct sect_rng {
    u64 sect;               // First sector of a range
    u64 size;               // Number of sectors
    struct sect_rng* next;  // Next element in the list
};

// List of sector ranges
struct sect_rng_list {
    struct sect_rng* head;  // Oldest element in the list
    struct sect_rng* tail;  // Newest element in the list
    unsigned int count;     // List number of elements
    unsigned int max_count; // Maximal number of elements (it can increase)
    unsigned int hits;      // Incoming bio range hit counter (might be zeroed)
    unsigned int msec;      // Variable used for time period measurement
    unsigned int rec_added; // Recently added entries
    unsigned int clone_mem_max; // Maximum memory need by clone
    struct rcu_head rcu;    // Needed by RCU manager
};

void srng_init(struct sect_rng_list* srlist);
void srng_add(struct sect_rng_list* srlist, struct sect_rng* srng);
void srng_del(struct sect_rng_list* srlist);
int srng_check(struct sect_rng_list* srlist, struct sect_rng* srng);
unsigned int srng_count(struct sect_rng_list* srlist);
void srng_free_all(struct sect_rng_list* srlist);
void srng_inc_max_count(struct sect_rng_list* srlist);

#endif /* SSET_LIST_H_ */
