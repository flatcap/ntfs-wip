#include <stdio.h>
#include <dlfcn.h>

const char * (*ntfs_libntfs_version)(void);

int main(int argc, char *argv[])
{
	void *handle = NULL;
	char *error = NULL;

	if (argc != 2)
		return 1;

	handle = dlopen(argv[1], RTLD_NOW);
	if (!handle) {
		printf ("dlopen failed: %s\n", dlerror());
		return 1;
	}

	ntfs_libntfs_version = dlsym(handle, "ntfs_libntfs_version");
	error = dlerror();
	if (error) {
		printf("dlsym failed: %s\n", error);
		dlclose(handle);
		return 1;
	}

	printf ("%s\n", (*ntfs_libntfs_version)());

	dlclose(handle);
	return 0;
}
