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
 * Gets the group name of a pkgng origin, eg 'ports-mgmt/pkg' ->
 * 'admin-tools'.
 */
#define	USAGE		"usage: %s origin\n"

#include <stdio.h>

#include "pkg.h"
#include "pk-backend.h"

#include "group.h"

int
main(int argc, char **argv)
{
	int		ret;

	ret = 0;
	if (argc != 2) {
		ret = 1;
		fprintf(stderr, USAGE, argv[0]);
	} else
		printf("%s\n",
		    pk_group_enum_to_string(group_from_origin(argv[1])));

	return ret;
}
