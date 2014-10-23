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

#include <assert.h>		/* assert */
#include <glib.h>		/* gchar, guint */
#include <stdio.h>		/* asprintf */
#include "pkg.h"		/* pkg_... */
#include "pk-backend.h"		/* pk_* */

#include "namever.h"		/* namever_... */

/*
 * Functions for dealing with "namevers" (name-version strings).
 * 
 * This file CANNOT depend on the PackageKit backend.
 */

/*
 * Joins a name and a version to form a namever.
 * 
 * To be freed using free(3).  Result may be NULL.
 */
char           *
namever_from_name_and_version(const char *name, const char *version)
{
	char           *result;

	assert(name != NULL);
	assert(version != NULL);

	(void)asprintf(&result, "%s-%s", name, version);
	return result;
}

/*
 * Allocates and returns a string of the form "name-version" that identifies
 * the given package's name and version.
 * 
 * To be freed using free(3).  Result may be NULL.
 */
char           *
namever_from_package(struct pkg *package)
{
	char           *result;

	assert(package != NULL);

	result = NULL;
	(void)pkg_asprintf(&result, "%n-%v", package, package);
	return result;
}

/*
 * Allocates and returns a string of the form "name-version" that contains
 * the name and version elements of a PackageID.
 * 
 * To be freed using free(3).
 */
char           *
namever_from_package_id(const gchar *package_id)
{
	char           *result;
	gchar         **id_splits;

	assert(package_id != NULL);

	id_splits = pk_package_id_split(package_id);
	assert(id_splits != NULL);

	result = namever_from_name_and_version(id_splits[PK_PACKAGE_ID_NAME],
	    id_splits[PK_PACKAGE_ID_VERSION]);

	g_strfreev(id_splits);
	return result;
}

char          **
namever_array_from_package_ids(gchar **package_ids, guint count)
{
	char          **namevers;
	unsigned int	i;

	assert(package_ids != NULL);

	namevers = calloc(count, sizeof(char *));
	if (namevers != NULL) {
		for (i = 0; i < count; i++) {
			namevers[i] = namever_from_package_id(package_ids[i]);
			if (namevers[i] == NULL) {
				namever_array_free(&namevers, count);
				break;
			}
		}
	}
	return namevers;
}

/* Frees an array of name-version strings. */
void
namever_array_free(char ***namevers_p, guint count)
{
	unsigned int	i;

	assert(namevers_p != NULL);

	if (*namevers_p != NULL) {
		for (i = 0; i < count; i++) {
			free((*namevers_p)[i]);
		}
		free(*namevers_p);
		*namevers_p = NULL;
	}
	assert(*namevers_p == NULL);
}
