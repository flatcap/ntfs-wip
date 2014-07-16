	{
	const char *name = "Rich";
	int num = 42;
	ntfs_log_set_flags (LOG_FLAG_FILENAME);
	ntfs_log_set_flags (LOG_FLAG_LINE);
	ntfs_log_set_flags (LOG_FLAG_FUNCTION);
	log_info ("info %s %d\n", name, num);
	log_warn ("warn %s %d\n", name, num);
	log_error ("error %s %d\n", name, num);
	log_crit ("critical %s %d\n", name, num);
	errno = 42;
	log_perror ("perror %s %d\n", name, num);
	printf ("wibble\n");
	if (1)
		return 0;
	}

	if (strncmp (argv[optind-1], "--log-", 6) == 0) {
		if (!ntfs_log_parse_option (argv[optind-1]))
			err++;
		break;
	}
