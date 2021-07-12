#include <stdlib.h>
#include <string.h>
#include <vector/vector.h>

struct vector *vec_init(size_t elem_size, size_t alloc_size)
{
	struct vector *vec = malloc(sizeof *vec);
	vec->n_elems = 0;
	vec->elem_size = elem_size;
	vec->alloc_size = alloc_size;
	vec->alloc_delta = alloc_size ? alloc_size : 1;
	vec_resize(vec, alloc_size);
	if (elem_size == 0 || !vec->base) {
		return NULL;
	}

	return vec;
}

int vec_resize(struct vector *vec, size_t new_alloc_size)
{
	void *new_base;

	if (!vec) {
		return 0;
	}

	new_base = realloc(vec->base, new_alloc_size * vec->elem_size);
	if (!new_base) {
		return 0;  /* memory error */
	}

	vec->base = new_base;
	if (new_alloc_size < vec->alloc_size) {
		vec->n_elems = new_alloc_size;
	}
	vec->alloc_size = new_alloc_size;
	return 1;
}

int vec_push_back(struct vector *vec, const void *elem)
{
	if (!vec) {
		return 0;
	}
	return vec_insert(vec, elem, vec->n_elems);
}

int vec_insert(struct vector *vec, const void *elem, size_t idx)
{
	size_t i, s;

	/* Allocate more memory if necessary */
	if (!vec || !elem || idx > vec->n_elems ||
			(vec->n_elems == vec->alloc_size
			&& !vec_resize(vec, vec->alloc_size + vec->alloc_delta))
			) {
		return 0;
	}

	/* Shift elements >= idx forwards */
	s = vec->elem_size;
	for (i = vec->n_elems; i > idx; i--) {
		memcpy((unsigned char *) vec->base + (i + 1) * s,
			(unsigned char *) vec->base + i * s, s);
	}
	vec->n_elems++;

	memcpy((unsigned char *) vec->base + idx * s, elem, s);
	return 1;
}

int vec_pop_back(struct vector *vec)
{
	if (!vec) {
		return 0;
	}
	return vec_remove(vec, vec->n_elems - 1);
}

int vec_remove(struct vector *vec, size_t idx)
{
	size_t i, s;

	if (!vec || idx >= vec->n_elems) {
		return 0;
	}

	/* Shift elements >= idx backwards */
	s = vec->elem_size;
	vec->n_elems--;
	for (i = idx; i < vec->n_elems; i++) {
		memcpy((unsigned char *) vec->base + i * s,
			(unsigned char *) vec->base + (i + 1) * s, s);
	}
	return 1;
}

void *vec_get(const struct vector *vec, size_t idx)
{
	if (!vec || idx >= vec->n_elems) {
		return NULL;
	}
	return (unsigned char *) vec->base + idx * vec->elem_size;
}

size_t vec_find(const struct vector *vec, const void *elem)
{
	size_t i, s;

	if (!vec || !elem) {
		return -1;
	}

	s = vec->elem_size;
	for (i = 0; i < vec->n_elems; i++) {
		if (!memcmp((unsigned char *) vec->base + i * s, elem, s)) {
			return i;
		}
	}

	return -1;  /* no matches found */
}

void vec_free(struct vector *vec)
{
	free(vec->base);
	free(vec);
}
