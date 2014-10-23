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

/* Functions for finding a package given its query ID. */

#include <assert.h>		/* assert */
#include <stdbool.h>
#include "pkg.h"

#include "../utils.h"		/* INTENTIONALLY_IGNORE */
#include "find.h"		/* query_find_... */
#include "id.h"			/* struct spec->query_id */

static bool	can_remote_iterate(struct query_id *query_id, unsigned int load_flags);

struct pkgdb_it *
query_find_local(struct query_find_spec *spec)
{

	assert(spec != NULL);
	assert(spec->query_id != NULL);
	assert(spec->db != NULL);
	return pkgdb_query(spec->db, spec->query_id->namever, MATCH_EXACT);
}

struct pkgdb_it *
query_find_remote(struct query_find_spec *spec)
{
	struct pkgdb_it *iterator;

	assert(spec != NULL);
	assert(spec->query_id != NULL);
	assert(spec->db != NULL);
	assert(spec->error_p != NULL);
	iterator = pkgdb_rquery(spec->db, spec->query_id->namever, MATCH_EXACT,
	    spec->query_id->repo);

	/*
	 * Make sure we can use this iterator.  (We only check now so that
	 * the case of there being no remote matches is properly handled.)
	 */
	if (iterator != NULL) {
		if (!can_remote_iterate(spec->query_id, spec->load_flags)) {
			pkgdb_it_free(iterator);
			iterator = NULL;

			*(spec->error_p) = QUERY_ERROR_REMOTE_NOT_ALLOWED;
		}
	} else {
		*(spec->error_p) = QUERY_ERROR_NO_RESULTS;
	}
	return iterator;
}

/*
 * Checks to see if trying to do a remote package iteration with this query
 * could spell disaster.
 */
static bool
can_remote_iterate(struct query_id *query_id, unsigned int load_flags)
{
	bool		sane;

	assert(query_id != NULL);

	/* Innocent until proven guilty */
	sane = true;

	/*
	 * Stop pkg from catching fire if we try to load files from
	 * non-installed packages.
	 */
	if (load_flags & PKG_LOAD_FILES) {
		sane = false;
	}
	return sane;
}
