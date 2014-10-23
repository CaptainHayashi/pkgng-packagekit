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

/* Functions for dealing with arrays of package pointers. */
#include <assert.h>		/* assert */
#include "pkg.h"		/* struct pkg, pkg_free */

#include "packages.h"		/* query_packages_... */

/* Allocates an array of size 'package_count' of null pointers to packages. */
struct pkg    **
query_packages_array_alloc(unsigned int package_count)
{

	return calloc(package_count, sizeof(struct pkg *));
}

/* Frees an array of size 'package_count' of pointers to packages. */
void
query_packages_array_free(struct pkg ***packages_p, unsigned int package_count)
{
	unsigned int	i;

	assert(packages_p != NULL);

	if (*packages_p != NULL) {
		for (i = 0; i < package_count; i++) {
			pkg_free((*packages_p)[i]);
		}

		free(*packages_p);
		*packages_p = NULL;
	}
}
