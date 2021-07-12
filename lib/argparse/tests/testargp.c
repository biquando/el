#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector/vector.h>
#include <argparse/argparse.h>

int main(int argc, char *argv[])
{
	struct argparse *ap = argp_init();
	argp_opt(ap, "-h", 0);
	argp_opt(ap, "-q", 0);
	argp_opt(ap, "-a", 0);
	argp_alias(ap, "--quiet", "-q");
	argp_opt(ap, "-n", -1);
	argp_opt(ap, "-o", 1);

	assert(argp_parse(ap, argc, argv));

	char *tmp[10];
	assert(argp_get_opt(ap, "-h", tmp));
	assert(!strcmp(tmp[0], "-h"));
	assert(argp_get_opt(ap, "-q", tmp));
	assert(!strcmp(tmp[0], "--quiet"));
	assert(!argp_get_opt(ap, "-a", tmp));
	assert(!argp_get_opt(ap, "--asdf", tmp));
	assert(3 == argp_get_opt(ap, "-n", tmp));
	assert(!strcmp(tmp[0], "1"));
	assert(!strcmp(tmp[1], "2"));
	assert(!strcmp(tmp[2], "3"));
	assert(2 == argp_get_args(ap, tmp));
	assert(!strcmp(tmp[0], "hello.c"));
	assert(!strcmp(tmp[1], "bye.c"));

	printf("All tests passed\n");
	argp_free(ap);
}
