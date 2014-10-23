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

/* High-level functions for performing queries. */

#include <assert.h>		/* assert */
#include <stdbool.h>		/* bool, true, false */
#include "pkg.h"		/* pkg_... */
#include "../pk-backend.h"	/* pk_... */

#include "../db.h"		/* db_... */
#include "../utils.h"		/* get_strv_and_length */
#include "do.h"			/* query_do_... */
#include "match.h"		/* query_match_... */
#include "packages.h"		/* query_packages_... */

typedef struct query_id *(*query_id_func_ptr) (gchar **strings, guint count);

static bool	emit_packages(PkBackend *backend, struct pkgdb *db, emit_ptr emitter, struct pkg **packages, unsigned int count);
static bool	query_do_from_backend(PkBackend *backend, unsigned int load_flags, emit_ptr emitter, query_id_func_ptr to_query_ids);
static bool	query_with_db(struct pkgdb *db, PkBackend *backend, unsigned int load_flags, emit_ptr emitter, query_id_func_ptr to_query_ids);

/*
 * Runs a query over the PackageIDs selected in the backend that sends the
 * first match to an emitting function.
 * 
 * This is a wrapper over several lower level query functions.
 */
bool
query_do_from_backend_ids(PkBackend *backend, unsigned int load_flags,
    emit_ptr emitter)
{

	assert(backend != NULL);
	assert(emitter != NULL);

	return query_do_from_backend(backend, load_flags, emitter,
	    query_id_array_from_package_ids);
}


/*
 * Runs a query over the package names selected in the backend that sends the
 * first match to an emitting function.
 * 
 * This is a wrapper over several lower level query functions.
 */
bool
query_do_from_backend_names(PkBackend *backend, unsigned int load_flags,
    emit_ptr emitter)
{

	assert(backend != NULL);
	assert(emitter != NULL);

	return query_do_from_backend(backend, load_flags, emitter,
	    query_id_array_from_names);
}

static bool
query_do_from_backend(PkBackend *backend, unsigned int load_flags,
    emit_ptr emitter, query_id_func_ptr to_query_ids)
{
	bool		success;
	struct pkgdb   *db;

	assert(backend != NULL);
	assert(emitter != NULL);

	success = false;

	db = db_open_remote(backend);
	if (db != NULL) {
		success = query_with_db(db, backend, load_flags, emitter,
		    to_query_ids);

		db_close(&db);
	}
	return success;
}

static bool
query_with_db(struct pkgdb *db, PkBackend *backend, unsigned int load_flags,
    emit_ptr emitter, query_id_func_ptr to_query_ids)
{
	bool		success;
	guint		count;
	struct pkg    **packages;
	struct query_id *query_ids;
	gchar         **package_ids;

	assert(backend != NULL);
	assert(db != NULL);
	assert(emitter != NULL);

	packages = NULL;

	package_ids = get_strv_and_length(backend, "package_ids", &count);
	assert(package_ids != NULL);

	query_ids = to_query_ids(package_ids, count);
	if (query_ids != NULL) {
		enum query_error error;

		error = QUERY_ERROR_OK;
		packages = query_match_ids(query_ids, count, db, load_flags,
		    &error);
		success = query_error_handle(backend, error);
	} else {
		success = query_error_handle(backend, QUERY_ERROR_INVALID_IDS);
	}
	if (packages != NULL) {
		success = emit_packages(backend, db, emitter, packages, count);
		query_packages_array_free(&packages, count);
	}
	query_id_array_free(&query_ids, count);

	return success;
}

static bool
emit_packages(PkBackend *backend, struct pkgdb *db, emit_ptr emitter,
    struct pkg **packages, unsigned int count)
{
	bool		success;
	unsigned int	i;

	assert(backend != NULL);
	assert(db != NULL);
	assert(emitter != NULL);
	assert(packages != NULL);

	success = true;
	for (i = 0; i < count; i++) {
		success = emitter(backend, db, packages[i]);
		if (success == false) {
			break;
		}
	}

	return success;
}
