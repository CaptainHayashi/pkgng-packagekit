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
#include <stdbool.h>
#include <string.h>		/* strcmp */
#include "../pk-backend.h"
#include "pkg.h"		/* struct pkg... */

#include "../namever.h"		/* namever_... */
#include "../repo.h"		/* repo_... */
#include "../utils.h"		/* type_of_repo_name, enum repo_type */
#include "check.h"		/* query_check... */
#include "find.h"		/* query_find_... */
#include "match.h"		/* query_match_... */
#include "packages.h"		/* query_packages_... */

/* Attempts to match a set of QueryIDs into packages. */

static struct pkg *query_match_from(query_find_function function, struct query_find_spec *spec);
static struct pkg *match_iterator(struct pkgdb_it *it, struct query_find_spec *spec);

struct pkg    **
query_match_ids(struct query_id *query_ids, unsigned int count,
    struct pkgdb *db, unsigned int load_flags, enum query_error *error_p)
{
	bool		success;
	unsigned int	i;
	struct pkg    **packages;

	assert(query_ids != NULL);
	assert(db != NULL);
	assert(error_p != NULL);

	packages = query_packages_array_alloc(count);
	success = (packages != NULL);

	for (i = 0; i < count; i++) {
		if (!success) {
			/*
			 * Catch both the packages being NULL and any
			 * failures below.
			 */
			break;
		}
		packages[i] = query_match_id(query_ids + i, db, load_flags,
		    error_p);
		success = (packages[i] != NULL);
	}

	if (!success) {
		query_packages_array_free(&packages, count);
	}
	return packages;
}

/*
 * Takes a QueryID and attempts to retrieve its corresponding package.
 */
struct pkg     *
query_match_id(struct query_id *query_id, struct pkgdb *db,
    unsigned int load_flags, enum query_error *error_p)
{
	bool		try_local;
	bool		try_remote;
	enum repo_type	type;
	struct pkg     *package;
	struct query_find_spec spec;

	assert(query_id != NULL);
	assert(db != NULL);
	assert(error_p != NULL);

	spec.query_id = query_id;
	spec.db = db;
	spec.error_p = error_p;
	spec.load_flags = load_flags;

	/*
	 * If we're not given a specific repository in the PackageID, we want
	 * to try searching locally first and then remotely; otherwise which
	 * database we query depends on the repository we have been given.
	 */
	type = repo_type(spec.query_id->repo);
	try_local = (type != REPO_REMOTE);
	try_remote = (type != REPO_LOCAL);

	package = NULL;
	if (try_local) {
		package = query_match_from(query_find_local, &spec);
	}
	if (package == NULL && try_remote) {
		package = query_match_from(query_find_remote, &spec);
	}
	if (package == NULL) {
		*error_p = QUERY_ERROR_NO_RESULTS;
	}
	return package;
}

/* Takes a querying function and attempts to find a QueryID using it. */
static struct pkg *
query_match_from(query_find_function function, struct query_find_spec *spec)
{
	struct pkg     *result;
	struct pkgdb_it *iterator;

	assert(function != NULL);
	assert(spec != NULL);

	result = NULL;

	iterator = function(spec);
	if (iterator != NULL) {
		result = match_iterator(iterator, spec);
		pkgdb_it_free(iterator);
	}
	return result;
}

/*
 * Tries to find a query-matching package in a database iterator. Returns the
 * package if one matches, or NULL if none do.
 */
static struct pkg *
match_iterator(struct pkgdb_it *it, struct query_find_spec *spec)
{
	bool		success;
	struct pkg     *pkg;

	assert(it != NULL);
	assert(spec != NULL);
	assert(spec->query_id != NULL);

	success = false;

	pkg = NULL;
	while (pkgdb_it_next(it, &pkg, spec->load_flags) == EPKG_OK) {
		success = query_check_package(pkg, spec->query_id);
		/* Did it match? */
		if (success) {
			break;
		}
	}

	if (!success) {
		pkg_free(pkg);
		pkg = NULL;
	}
	/* Package if and only if success. */
	assert(pkg != NULL || !success);
	assert(pkg == NULL || success);

	return pkg;
}
