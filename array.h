#pragma once

#include <stdlib.h>

#define array_t(type) struct { type *a; index_t l, m; }
#define array_init(v)         ((v).l=(v).m=0, (v).a=0)
#define array_free(v)         (free((v).a))
#define array_at(v, i)        ((v).a[(i)])
#define array_capacity(v)     ((v).m)
#define array_size(v)         ((v).l)
#define array_empty(v)        (!(v).l)
#define array_pop(v)          ((v).a[--(v).l])
#define array_top(v)          ((v).a[(v).l-1])
#define array_clear(v)        ((v).l=0)
#define array_reserve(v, n)   (((v).m=( ((n)>=1)? (n) : 1)), (v).a=realloc((v).a, sizeof(*(v).a)*(v).m))
#define array_push(v)         ((((v).l==(v).m)? (((v).m=((v).m? 2*(v).m : 2)), (v).a=realloc((v).a, sizeof(*(v).a)*(v).m)) : 0), ((v).a + ((v).l++)))
#define array_push_item(v, x) (*array_push(v) = (x))
#define array_release(v)      ((((v).m>(v).l)? ((v).a=realloc((v).a, sizeof(*(v).a)*(v).l)) : 0), (v).l=(v).m=0, (v).a)
