/*-
 * Copyright (C) 2013 Matt Windsor <mattbw@FreeBSD.org>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * Given the names of PackageKit groups, this programme prints out an
 * Extended Regular Expression matching all dir/name style package origins in
 * those groups.
 */
#define	USAGE "usage: %s group ...\n"

#include <stdlib.h>		/* calloc, free */
#include <stdio.h>		/* printf, fprintf */
#include "pk-backend.h"		/* PkGroupEnum, pk_group_enum_to_string */
#include "group.h"		/* group_list_to_origin_regex */

int
main(int argc, char **argv)
{
	int i;
	int ret;
	unsigned int groupc;
	char *regex;
	PkGroupEnum *groupv;

	ret = 0;

	/* Did we get any groups? */
	if (argc == 1) {
		ret = 1;
		fprintf(stderr, USAGE, argv[0]);
		goto cleanup;
	}

	groupc = (unsigned int)(argc - 1);
	groupv = calloc(groupc, sizeof(PkGroupEnum));
	if (groupv == NULL) {
		ret = 1;
		fprintf(stderr, "out of mem!\n");
		goto cleanup;
	}

	/* Find the group enums from their names */
	for (i = 1; i < argc; i++)
		groupv[i - 1] = pk_group_enum_from_string(argv[i]);

	/* Now build the regex */
	regex = group_list_to_origin_regex(groupc, groupv);
	if (regex == NULL) {
		ret = 1;
		fprintf (stderr, "passed null pointer to regex!\n");
		goto cleanup;
	}

	printf("%s\n", regex);
	free(regex);

cleanup:
	return ret;
}
