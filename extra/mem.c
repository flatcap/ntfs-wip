#if 1
static int my_alloc = 0;
static int my_free  = 0;

void * __libc_calloc (size_t nmemb, size_t size);
void * __libc_malloc (size_t size);
void * __libc_realloc (void *ptr, size_t size);
void __libc_free (void *ptr);

static int   mem_size[1024];
static void *mem_list[1024];
static int index = 0;

/**
 * calloc
 */
void * calloc (size_t nmemb, size_t size)
{
	void *ptr;
	ptr = __libc_calloc (nmemb, size);
	if (ptr) {
		if (index >= 0) {
			my_alloc++;
			mem_list[index] = ptr;
			mem_size[index] = nmemb*size;
			index++;
			//printf ("%p calloc (%d,%d)\n", ptr, nmemb, size); fflush (stdout);
		}
	}
	return ptr;
}

/**
 * malloc
 */
void * malloc (size_t size)
{
	void *ptr;
	ptr = __libc_malloc (size);
	if (ptr) {
		if (index >= 0) {
			my_alloc++;
			mem_list[index] = ptr;
			mem_size[index] = size;
			index++;
			//printf ("%p malloc (%d)\n", ptr, size); fflush (stdout);
		}
	}
	return ptr;
}

/**
 * realloc
 */
void * realloc (void *ptr, size_t size)
{
	void *new;

	new = __libc_realloc (ptr, size);

	if (ptr != new) {
		if (ptr) {
			//printf ("%p refree\n", ptr); fflush (stdout);
			my_free++;
		}
		if (new) {
			if (index >= 0) {
				my_alloc++;
				mem_list[index] = ptr;
				mem_size[index] = size;
				index++;
				//printf ("%p realloc (%d)\n", new, size); fflush (stdout);
			}
		}
	}
	return new;
}

/**
 * free
 */
void free (void *ptr)
{
	int i;
	if (!ptr)
		return;
	//printf ("%p free\n", ptr); fflush (stdout);
	my_free++;
	for (i = 0; i < index; i++) {
		if (mem_list[i] == ptr) {
			mem_list[i] = NULL;
			return;
		}
	}
	//printf ("Double free of ptr %p\n", ptr);
	__libc_free (ptr);
}

/**
 * mem_start
 */
void mem_start()
{
	//printf ("mem_start\n");
	index = 0;
	memset (mem_size, 0, sizeof (mem_size));
	memset (mem_list, 0, sizeof (mem_list));
}

/**
 * mem_stop
 */
int mem_stop()
{
	int i = 0;
	//printf ("mem_stop\n");
	if (my_alloc != my_free)
		printf ("mem (%d,%d)\n", my_alloc, my_free);
	else
		printf ("no memory leaks\n");
	/*
	for (i = 0; i < 1024; i++) {
		if (!mem_list[i])
			break;
		printf ("%p %d\n", mem_list[i], mem_size[i]);
		utils_dump_mem (mem_list[i], 0, mem_size[i], TRUE);
	}
	*/
	return i;
}

#endif

