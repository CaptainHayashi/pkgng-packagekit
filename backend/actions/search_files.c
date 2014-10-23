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
#include <stdbool.h>		/* bool, TRUE, FALSE */
#include "../pk-backend.h"
#include "pkg.h"

#include "../actions.h"		/* search_files_thread prototype */
#include "../db.h"		/* db_open_remote */
#include "../emit.h"		/* emit_package */
#include "../utils.h"		/* INTENTIONALLY_IGNORE */
#include "../pkgutils.h"	/* pkgutils_... */

/*
 * Searches for packages containing specified files.
 */
gboolean
search_files_thread(PkBackend *backend)
{
	bool		at_least_one;
	gboolean	success;
	guint		filec;
	guint		i;
	struct pkg     *pkg;
	struct pkgdb   *db;
	struct pkgdb_it *it;
	gchar         **filev;

	assert(backend != NULL);
	db = NULL;
	it = NULL;
	pkg = NULL;
	success = FALSE;

	(void)pk_backend_set_status(backend, PK_STATUS_ENUM_QUERY);
	(void)pk_backend_set_percentage(backend, 0);

	db = db_open_remote(backend);
	if (db == NULL)
		goto cleanup;

	filev = pk_backend_get_strv(backend, "search");
	if (filev == NULL) {
		ERR(backend, PK_ERROR_ENUM_INTERNAL_ERROR, "search is NULL");
		goto cleanup;
	}
	filec = g_strv_length(filev);
	if (filec == 0) {
		ERR(backend, PK_ERROR_ENUM_INTERNAL_ERROR, "no files given");
		goto cleanup;
	}
	for (i = 0; i < filec; i++) {
		at_least_one = false;

		it = pkgdb_query_which(db, filev[i], false);
		if (it != NULL) {
			while (pkgdb_it_next(it, &pkg, PKG_LOAD_BASIC)
			    == EPKG_OK) {
				assert(pkg != NULL);

				emit_package(pkg, backend,
				    pkgutils_pkg_current_state(pkg));
				at_least_one = true;
			}
		}
		/* TODO: specifically name file? */
		if (at_least_one == false)
			ERR(backend, PK_ERROR_ENUM_PACKAGE_NOT_FOUND,
			    "no matches for file");

		(void)pk_backend_set_percentage(backend,
		    ((i + 1) * 100) / filec);
	}

	success = TRUE;

cleanup:
	pkg_free(pkg);
	pkgdb_it_free(it);
	pkgdb_close(db);

	(void)pk_backend_finished(backend);
	return success;
}
