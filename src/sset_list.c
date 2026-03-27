// SPDX-License-Identifier: GPL-2.0-only

/*
 * Copyright (C) 2022 Datto Inc.
 */

#include "sset_list.h"

/**
 * sset_list_init() - Initializes the @sl structure.
 *
 * @sl: The &struct sset_list object pointer.
 */
inline void sset_list_init(struct sset_list *sl)
{
        sl->head = sl->tail = NULL;
}

/**
 * sset_list_empty() - Checks to see if the the supplied &struct sset_list
 * is empty.
 *
 * @sl: The &struct sset_list object pointer.
 *
 * Return:
 * * 0 when empty
 * * !0 otherwise
 */
inline int sset_list_empty(const struct sset_list *sl)
{
        return sl->head == NULL;
}

/**
 * sset_list_add() - Adds @sset to the tail of the list @sl.
 *
 * @sl: The &struct sset_list object pointer.
 * @sset: The &struct sector_set object pointer to be added to the @sl.
 */
void sset_list_add(struct sset_list *sl, struct sector_set *sset)
{
        sset->next = NULL;
        if (sl->tail)
                sl->tail->next = sset;
        else
                sl->head = sset;
        sl->tail = sset;
}

/**
 * sset_list_pop() - Fetches an element from the head of @sl.
 *
 * @sl: The &struct sset_list object pointer.
 *
 * The returned element is removed from @sl if present.
 *
 * Return: The element at the head of the list, NULL if empty.
 */
struct sector_set *sset_list_pop(struct sset_list *sl)
{
        struct sector_set *sset = sl->head;

        if (sset) {
                sl->head = sl->head->next;
                if (!sl->head)
                        sl->tail = NULL;
                sset->next = NULL;
        }

        return sset;
}

/* srng_init() - Init sectors range list
 *
 * @srlist: List to be initialized
 */

// Initial number of sectors range list entries
#define SRNG_MAX_COUNT (100)

void srng_init(struct sect_rng_list* srlist)
{
    srlist->head = NULL;
    srlist->tail = NULL;
    srlist->count = 0;
    srlist->max_count = SRNG_MAX_COUNT;
    srlist->hits = 0;
    srlist->msec = 0;
    srlist->rec_added = 0;
    srlist->clone_mem_max = 0;
}

/* srng_inc_max_count() - Increase maximum allowed sectors range list entries
 *
 * @srlist: List that will be affected
 */

void srng_inc_max_count(struct sect_rng_list* srlist)
{
    // Why 100?
    srlist->max_count += 100;
}

/* srng_add() - Add element to the list
 *
 * @srlist: List that will be affected
 * @srng: Element to be added
 */

void srng_add(struct sect_rng_list* srlist, struct sect_rng* srng)
{
    if (srlist->tail)
    {
        srlist->tail->next = srng;
    }
    else
    {
        srlist->head = srng;
    }
    srlist->tail = srng;
    srlist->count++;
}

/* srng_del() - Delete oldest element from the list
 *
 * @srlist: List that will be affected
 *
 */

void srng_del(struct sect_rng_list* srlist)
{   
    if (srlist->head)
    {
        srlist->head = srlist->head->next;
        if (srlist->head == NULL)
        {
            srlist->tail = NULL;
        }
        srlist->count--;
    }
}

/* srng_check() - Check if sectors range exist in the list
 *
 * @srlist: List of sector ranges
 * @srng: Range to be checked
 *
 * Return:
 * 1 range found
 * 0 otherwise
 */

int srng_check(struct sect_rng_list* srlist, struct sect_rng* srng)
{
    struct sect_rng* start = srlist->head;
    u64 end_sect_in = srng->sect + srng->size, end_sect;
    int ret = 0;
    unsigned int idx = 0;

    while (start != NULL)
    {
        end_sect = start->sect + start->size;
        if ((srng->sect >= start->sect) && (end_sect_in <= end_sect))
        {
            ret = 1;
            srlist->hits++;
            break;
        }
        start = start->next;
        idx++;
    }

    if (ret)
    {
        if (dattobd_debug == 4)
        {
            // Print hit index value starting from the newest entries
            LOG_DEBUG("Index hit: %d", srlist->count-1-idx);
        }
    }

    return ret;
}

/* srng_count() - Count elements in the list
 *
 * @srlist: List of sector ranges
 *
 * Return:
 * Number of list elements
 */

unsigned int srng_count(struct sect_rng_list* srlist)
{
    struct sect_rng* start = srlist->head;
    unsigned int count = 0;

    while (start != NULL)
    {
        count++;
        start = start->next;
    }
    return count;
}

/* srng_free_all() - Free all elements in the list
 *
 * @srlist: List of sector ranges
 *
 */

void srng_free_all(struct sect_rng_list* srlist)
{
    struct sect_rng* start = srlist->head;
    struct sect_rng* curr;
    unsigned int cnt = 0;

    while (start != NULL)
    {
        curr = start;
        start = start->next;
        kfree(curr);
        cnt++;
    }

    if (dattobd_debug == 7)
    {
        LOG_DEBUG("List elements freed: %u", cnt);
    }
}
