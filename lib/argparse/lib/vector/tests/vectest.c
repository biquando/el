#include <assert.h>
#include <stdio.h>
#include <vector/vector.h>

int main(void) {
	struct vector v;
	int x;
	v = *vec_init(sizeof(int), 3);
	for (x = 1; x <= 5; x++) {
		assert(vec_push_back(&v, &x));
	}
	assert(v.n_elems == 5);
	assert(v.elem_size == 4);
	assert(v.alloc_size == 6);
	assert(v.alloc_delta == 3);
	for (x = 0; x < 5; x++) {
		int y = x + 1;
		assert(*((int*) (v.base + x * v.elem_size)) == y);
		assert(*((int*) vec_get(&v, x)) == y);
		assert(vec_find(&v, &y) == x);
	}

	assert(vec_pop_back(&v));
	assert(vec_remove(&v, 2));
	x = 2;
	assert((x = vec_find(&v, &x)));
	assert(vec_remove(&v, x));
	assert(*((int*) vec_get(&v, 0)) == 1);
	assert(*((int*) vec_get(&v, 1)) == 4);

	printf("Passed all tests\n");
	return 0;
}
