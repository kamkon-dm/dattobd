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

/* srng_init() - Init array with zeros
 *
 * @srng_a: Struct srng_array* to be initialized
 */
void srng_init(struct srng_array* srng_a)
{
    int i;
    srng_a->curr_idx = 0;
    srng_a->cnt = 0;

    for (i = 0; i < SRNG_MAX_COUNT; i++)
    {
        srng_a->rng[i].sect = 0;
        srng_a->rng[i].size = 0;
    }
}

/* srng_add() - Add new range element to the array
 *
 * @srng_a: Struct srng_array* for the new element
 * @srng: Struct srng_range* representing new element
 *
 * Return:
 * 0 success
 * !0 otherwise
 */
int srng_add(struct srng_array* srng_a, struct srng_range* srng)
{
    srng_a->rng[srng_a->curr_idx].sect = srng->sect;
    srng_a->rng[srng_a->curr_idx].size = srng->size;
    srng_a->curr_idx++;
    srng_a->curr_idx %= SRNG_MAX_COUNT;
    if(srng_a->cnt < SRNG_MAX_COUNT)
        srng_a->cnt++;
    return 0;
}

/* srng_check() - Check if range exist in the array
 *
 * @srng_a: Struct srng_array* to search for range
 * @srng: Struct srng_range* representing range to be checked
 *
 * Return:
 * 0 range found
 * 1 otherwise
 */
int srng_check(struct srng_array* srng_a, struct srng_range* srng)
{
    unsigned int idx = srng_a->curr_idx;
    u64 end_sect_in = srng->sect + srng->size;
    u64 end_sect;
    int i, ret = 1;

    for (i = 0; i < srng_a->cnt; i++)
    {
        if (idx == 0)
            idx = SRNG_MAX_COUNT;
        idx--;
        end_sect = srng_a->rng[idx].sect + srng_a->rng[idx].size;
        if ((srng->sect >= srng_a->rng[idx].sect) && (end_sect_in <= end_sect))
        {
            ret = 0;
            break;
        }
    }

    return ret;
}
