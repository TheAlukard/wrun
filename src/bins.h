#pragma once

#include "list.h"
#include "utils.h"
#include <stdlib.h>

LIST_DEF(CstrList, char*);

typedef struct {
    CstrList a;
    CstrList b;
    CstrList c;
    CstrList d;
    CstrList e;
    CstrList f;
    CstrList g;
    CstrList h;
    CstrList i;
    CstrList j;
    CstrList k;
    CstrList l;
    CstrList m;
    CstrList n;
    CstrList o;
    CstrList p;
    CstrList q;
    CstrList r;
    CstrList s;
    CstrList t;
    CstrList u;
    CstrList v;
    CstrList w;
    CstrList x;
    CstrList y;
    CstrList z;
    CstrList _; // other
} Bins;


FORCE_INLINE Bins* bins_alloc(void)
{
    Bins *bins = (Bins*)calloc(1, sizeof(Bins));

    return bins;
}

FORCE_INLINE CstrList* get_strlist(Bins *bins, char c)
{
    switch (to_lower(c)) {
        case 'a': return &bins->a;
        case 'b': return &bins->b;
        case 'c': return &bins->c;
        case 'd': return &bins->d;
        case 'e': return &bins->e;
        case 'f': return &bins->f;
        case 'g': return &bins->g;
        case 'h': return &bins->h;
        case 'i': return &bins->i;
        case 'j': return &bins->j;
        case 'k': return &bins->k;
        case 'l': return &bins->l;
        case 'm': return &bins->m;
        case 'n': return &bins->n;
        case 'o': return &bins->o;
        case 'p': return &bins->p;
        case 'q': return &bins->q;
        case 'r': return &bins->r;
        case 's': return &bins->s;
        case 't': return &bins->t;
        case 'u': return &bins->u;
        case 'v': return &bins->v;
        case 'w': return &bins->w;
        case 'x': return &bins->x;
        case 'y': return &bins->y;
        case 'z': return &bins->z;
        default : return &bins->_;
    }
}
