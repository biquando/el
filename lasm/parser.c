#include <stdlib.h>
#include "parser.h"

struct parser *par_init()
{
	struct parser *par;
	par = malloc(sizeof *par);

	par->out_buf = vec_init(sizeof(unsigned char), BUF_DEFAULT_SIZE);

	return par;
}

void par_free(struct parser *par)
{
	vec_free(par->out_buf);
	free(par);
}
