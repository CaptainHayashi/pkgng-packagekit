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

#include <glib.h>
#include "../pk-backend.h"
#include "pkg.h"

#include "../db.h"		/* db_open_remote */
#include "../emit.h"		/* emit_package */
#include "../pkgutils.h"	/* pkgutils_... */
#include "../utils.h"		/* INTENTIONALLY_IGNORE */

#include "../actions.h"		/* install_files_thread prototype */

static gboolean	do_file(const gchar *path, PkBackend *backend, gboolean simulate);
static gboolean	do_files(PkBackend *backend, gboolean simulate);

/*
 * The thread that performs an InstallFiles operation. Should be invoked by
 * the pk_backend_install_files hook.
 */
gboolean
install_files_thread(PkBackend *backend)
{

	return do_files(backend, FALSE);
}

/*
 * The thread that performs a simulated InstallFiles operation. Should be
 * invoked by the pk_backend_install_files hook.
 */
gboolean
simulate_install_files_thread(PkBackend *backend)
{

	return do_files(backend, TRUE);
}

/*
 * Installs a single package file, or pretends to.
 */
static gboolean
do_file(const gchar *path, PkBackend *backend, gboolean simulate)
{
	gboolean	success;
	struct pkg     *pkg;
	struct pkg_manifest_key *keys;
	struct pkgdb   *db;

	success = FALSE;
	pkg = NULL;
	keys = NULL;
	db = NULL;

	(void)pkg_manifest_keys_new(&keys);

	if (simulate) {
		(void)pk_backend_set_status(backend, PK_STATUS_ENUM_RUNNING);

		if (pkg_open(&pkg, path, keys, PKG_FLAG_NONE) != EPKG_OK) {
			ERR(backend,
			    PK_ERROR_ENUM_PACKAGE_CORRUPT,
			    "could not open file");
			goto cleanup;
		}
		emit_package(pkg, backend, pkgutils_pkg_install_state(pkg));

		success = TRUE;
	} else {
		/* TODO: event hook */

		db = db_open_remote(backend);
		if (db == NULL)
			goto cleanup;

		(void)pk_backend_set_status(backend, PK_STATUS_ENUM_INSTALL);

		if (pkg_add(db, path, PKG_FLAG_NONE, keys) != EPKG_OK) {
			ERR(backend,
			    PK_ERROR_ENUM_PACKAGE_FAILED_TO_INSTALL,
			    "could not install file");
			goto cleanup;
		}
		success = TRUE;
	}

cleanup:
	pkgdb_close(db);
	pkg_free(pkg);
	pkg_manifest_keys_free(keys);

	return success;
}

/*
 * Does a file install operation, or a simulation thereof.
 */
static gboolean
do_files(PkBackend *backend, gboolean simulate)
{
	gboolean	success;
	gchar         **paths;

	success = FALSE;

	paths = pk_backend_get_strv(backend, "full_paths");
	if (paths == NULL)
		ERR(backend,
		    PK_ERROR_ENUM_INTERNAL_ERROR,
		    "could not get path vector");
	else if (*paths == NULL)
		ERR(backend,
		    PK_ERROR_ENUM_INTERNAL_ERROR,
		    "path vector is empty");
	else {
		gchar         **p;

		for (p = paths, success = TRUE;
		    (*p != NULL) && success == TRUE;
		    p++)
			success = do_file(*p, backend, simulate);
	}

	(void)pk_backend_finished(backend);
	return success;
}
