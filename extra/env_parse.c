#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"

/**
 * utils_env_parse
 */
static int utils_env_parse(const char *name, char ***array)
{
	char *delim;
	int len;
	int count;
	char **results;
	char *value;

	if (!name || !array || (*array))
		return 0;

	value = getenv(name);
	if (!value)
		return 0;

	count = 1;
	results = malloc((count+1) * sizeof(*results));
	if (!results)
		return 0;

	results[0] = strdup(name);
	
	while (value && *value) {
		delim = strchr(value, ' ');
		count++;

		if (delim)
			len = delim - value;
		else
			len = strlen(value);

		results = realloc(results, (count+1) * sizeof(*results));
		if (!results)
			return 0;
		results[count-1] = strndup(value, len);

		value += len;
		if (delim)
			value++;
	}

	results[count] = NULL;
	*array = results;

	return count;
}

/**
 * utils_env_free
 */
static void utils_env_free(char **array)
{
	int i;

	if (!array)
		return;

	for (i = 0; array[i]; i++)
		free(array[i]);
	free(array);
}


/**
 * main
 */
int main(void)
{
	int count = 0;
	char **args = NULL;
	int i;

	count = utils_env_parse("NTFS_OPTIONS", &args);

	printf ("count = %d\n", count);

	for (i = 0; i < count; i++)
		printf ("    '%s'\n", args[i]);

	utils_env_free(args);

	return 0;
}


#if 0
/**
 * main_mkntfs
 */
int main_mkntfs(int argc, char *argv[])
{
	int result = 1;
	char *env;
	int count = 0;
	char **split = NULL;

	ntfs_log_set_handler(ntfs_log_handler_outerr);
	utils_set_locale();

	mkntfs_init_options(&opts);			/* Set up the options */

	count = options_parse("NTFS_OPTIONS", &split);

	if (!mkntfs_parse_options(count, split, &opts))
		goto done;
	if (!mkntfs_parse_options(argc, argv, &opts))
		goto done;

	result = mkntfs_redirect(&opts);
done:
	options_free(split);
	return result;
}

#endif

