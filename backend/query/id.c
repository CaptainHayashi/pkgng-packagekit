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
#include <glib.h>		/* g... */
#include <stdlib.h>		/* calloc */
#include <string.h>		/* strdup */
#include "../pk-backend.h"	/* pk_... */

#include "id.h"			/* query_id_... */
#include "../namever.h"		/* namever_... */


typedef bool    (*id_from_string_ptr) (const char *, struct query_id *);

static char    *strdup_null_if_empty(const char *in);
static struct query_id *query_id_array_alloc(unsigned int count);
static struct query_id *query_id_array_from_strings(char **strings, unsigned int package_count, id_from_string_ptr id_from_string);
static void	query_id_free_contents(struct query_id *query_id);

/* Converts an array of PackageIDs to query IDs. */
struct query_id *
query_id_array_from_package_ids(gchar **package_ids,
    unsigned int package_count)
{

	return query_id_array_from_strings(package_ids, package_count,
	    query_id_from_package_id);
}

/* Converts an array of names (or name-version strings) to query IDs. */
struct query_id *
query_id_array_from_names(gchar **package_names, unsigned int package_count)
{

	return query_id_array_from_strings(package_names, package_count,
	    query_id_from_name);
}

/*
 * Converts an array of strings to query IDs using the given transforming
 * function.
 */
static struct query_id *
query_id_array_from_strings(char **strings, unsigned int package_count,
    id_from_string_ptr id_from_string)
{
	struct query_id *query_ids;

	assert(strings != NULL);
	assert(id_from_string != NULL);

	query_ids = query_id_array_alloc(package_count);
	if (query_ids != NULL) {
		bool		success;
		unsigned int	i;

		success = true;
		for (i = 0; i < package_count; i++) {
			success = id_from_string(strings[i], query_ids + i);
			if (!success) {
				query_id_array_free(&query_ids, package_count);
				break;
			}
		}

		if (!success) {
			query_id_array_free(&query_ids, package_count);
		}
	}
	return query_ids;
}

/*
 * Converts a package name or name-version string to a query ID. Overwrites
 * the contents of *query_id with said ID.
 */
bool
query_id_from_name(const char *name, struct query_id *query_id)
{

	assert(name != NULL);
	assert(query_id != NULL);

	query_id->namever = strdup_null_if_empty(name);
	query_id->arch = NULL;
	query_id->repo = NULL;

	return (query_id->namever != NULL);
}

/*
 * Converts a PackageKit package ID to a query ID. Overwrites the contents of
 * *query_id with said ID.
 */
bool
query_id_from_package_id(const gchar *package_id, struct query_id *query_id)
{
	bool		success;
	gchar         **split_package_id;

	assert(package_id != NULL);
	assert(query_id != NULL);

	success = false;

	split_package_id = pk_package_id_split(package_id);
	if (split_package_id != NULL) {
		bool		have_name;
		bool		have_version;
		gchar          *name;
		gchar          *version;
		gchar          *arch;
		gchar          *repo;

		/* We're not allowed to have an empty name or version. */

		name = split_package_id[PK_PACKAGE_ID_NAME];
		have_name = (name != NULL && name[0] != '\0');

		version = split_package_id[PK_PACKAGE_ID_VERSION];
		have_version = (version != NULL && version[0] != '\0');

		/*
		 * Names and versions are mandatory. Anything else is
		 * optional.
		 */
		if (have_name && have_version) {
			query_id->namever = namever_from_name_and_version(name,
			    version);

			arch = split_package_id[PK_PACKAGE_ID_ARCH];
			query_id->arch = strdup_null_if_empty(arch);

			repo = split_package_id[PK_PACKAGE_ID_DATA];
			query_id->repo = strdup_null_if_empty(repo);

			success = true;
		}
		g_strfreev(split_package_id);
	}
	return success;
}

/* Allocates an array of query IDs. */
static struct query_id *
query_id_array_alloc(unsigned int count)
{

	return calloc(count, sizeof(struct query_id));
}

/*
 * Returns NULL if the given string is NULL or empty, and a duplicate of the
 * string otherwise.
 */
static char    *
strdup_null_if_empty(const char *in)
{

	return (in == NULL || in[0] == '\0') ? NULL : strdup(in);
}

void
query_id_free(struct query_id **query_id_p)
{

	assert(query_id_p != NULL);
	query_id_array_free(query_id_p, 1);
	assert(*query_id_p == NULL);
}

void
query_id_array_free(struct query_id **query_ids_p, unsigned int count)
{
	unsigned int	i;

	assert(query_ids_p != NULL);

	if (*query_ids_p != NULL) {
		for (i = 0; i < count; i++) {
			query_id_free_contents((*query_ids_p) + i);
		}

		free(*query_ids_p);
		*query_ids_p = NULL;
	}
	assert(*query_ids_p == NULL);
}

static void
query_id_free_contents(struct query_id *query_id)
{

	assert(query_id != NULL);
	free(query_id->namever);
	free(query_id->arch);
	free(query_id->repo);
}
