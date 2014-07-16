#include <stdio.h>
#include <unistd.h>

static void *top_level = NULL;

static void dave(void)
{
	printf ("%s: %d %p\n", __FUNCTION__, 0, __builtin_return_address(0));
	if (__builtin_return_address(0) == top_level)
		goto done;
	printf ("%s: %d %p\n", __FUNCTION__, 1, __builtin_return_address(1));
	if (__builtin_return_address(1) == top_level)
		goto done;
	printf ("%s: %d %p\n", __FUNCTION__, 2, __builtin_return_address(2));
	if (__builtin_return_address(2) == top_level)
		goto done;
	printf ("%s: %d %p\n", __FUNCTION__, 3, __builtin_return_address(3));
	if (__builtin_return_address(3) == top_level)
		goto done;
done:

	printf("hello\n");
}

static void bob(void)
{
	printf ("%s: %d %p\n", __FUNCTION__, 0, __builtin_return_address(0));
	if (__builtin_return_address(0) == top_level)
		goto done;
	printf ("%s: %d %p\n", __FUNCTION__, 1, __builtin_return_address(1));
	if (__builtin_return_address(1) == top_level)
		goto done;
	printf ("%s: %d %p\n", __FUNCTION__, 2, __builtin_return_address(2));
	if (__builtin_return_address(2) == top_level)
		goto done;
	printf ("%s: %d %p\n", __FUNCTION__, 3, __builtin_return_address(3));
	if (__builtin_return_address(3) == top_level)
		goto done;
done:

	dave();
}

static void jim(void)
{ 
	printf ("%s: %d %p\n", __FUNCTION__, 0, __builtin_return_address(0));
	if (__builtin_return_address(0) == top_level)
		goto done;
	printf ("%s: %d %p\n", __FUNCTION__, 1, __builtin_return_address(1));
	if (__builtin_return_address(1) == top_level)
		goto done;
	printf ("%s: %d %p\n", __FUNCTION__, 2, __builtin_return_address(2));
	if (__builtin_return_address(2) == top_level)
		goto done;
	printf ("%s: %d %p\n", __FUNCTION__, 3, __builtin_return_address(3));
	if (__builtin_return_address(3) == top_level)
		goto done;
done:

	bob();
}

int main(int argc, char *argv[])
{
	if (argc > 1)
		return 1;
	if (!argv)
		return 1;

	top_level = __builtin_return_address(0);
	printf ("%s: %d %p\n", __FUNCTION__, 0, __builtin_return_address(0));

	jim();
	return 0;
}
