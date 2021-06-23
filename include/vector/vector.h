#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

struct vector {
	size_t n_elems;      /* number of elements currently stored */
	size_t elem_size;    /* size in bytes of each element */
	size_t alloc_size;   /* number of elements allocated */
	size_t alloc_delta;  /* how many elements should be allocated on overflow */
	void *base;
};

struct vector *vec_init(size_t elem_size, size_t alloc_size);
int vec_resize(struct vector *vec, size_t new_alloc_size);
int vec_push_back(struct vector *vec, const void *elem);
int vec_insert(struct vector *vec, const void *elem, size_t idx);
int vec_pop_back(struct vector *vec);
int vec_remove(struct vector *vec, size_t idx);
void *vec_get(const struct vector *vec, size_t idx);
size_t vec_find(const struct vector *vec, const void *elem);
void vec_free(struct vector *vec);

#endif
