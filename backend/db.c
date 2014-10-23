/*-
 *
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
#include "pk-backend.h"
#include "pkg.h"

#include "db.h"			/* prototypes */
#include "utils.h"		/* null_if_empty, split_id */

static const unsigned int ACCESS_MODE = (unsigned int)(PKGDB_MODE_READ |
    PKGDB_MODE_WRITE);
static const unsigned int ACCESS_DB = (unsigned int)(PKGDB_DB_LOCAL |
    PKGDB_DB_REPO);

static bool	has_remote_access(PkBackend *backend);
static struct pkgdb *open_remote_direct(PkBackend *backend);

/*
 * Opens a pkgdb ready for remote operations. This will always return TRUE if
 * and only if a database ready for use is now pointed to by *db, and FALSE
 * if and only if there isn't.
 * 
 * This will also emit PackageKit errors if it fails.
 * 
 * This must be called during the lifetime of "backend", eg after
 * "pk_backend_initialize" and before "pk_backend_destroy".
 */
struct pkgdb   *
db_open_remote(PkBackend *backend)
{
	bool		has_access;
	struct pkgdb   *db;

	assert(backend != NULL);

	db = NULL;

	has_access = has_remote_access(backend);
	if (has_access) {
		db = open_remote_direct(backend);
	}
	return db;
}

void
db_close(struct pkgdb **db_p)
{

	assert(db_p != NULL);

	if (*db_p != NULL) {
		pkgdb_close(*db_p);
		*db_p = NULL;
	}
	assert(*db_p == NULL);
}

static bool
has_remote_access(PkBackend *backend)
{
	int		err;

	assert(backend != NULL);

	err = pkgdb_access(ACCESS_MODE, ACCESS_DB);
	if (err != EPKG_OK) {
		ERR(backend,
		    PK_ERROR_ENUM_NOT_AUTHORIZED,
		    "cannot access database");
	}
	return (err == EPKG_OK);
}

static struct pkgdb *
open_remote_direct(PkBackend *backend)
{
	int		err;
	struct pkgdb   *db;

	assert(backend != NULL);

	db = NULL;
	err = pkgdb_open(&db, PKGDB_REMOTE);
	if (err != EPKG_OK) {
		(void)pk_backend_error_code(backend,
		    PK_ERROR_ENUM_CANNOT_GET_LOCK,
		    "cannot open database");
	}
	assert(err == EPKG_OK || backend == NULL);
	assert(err != EPKG_OK || backend != NULL);

	return db;
}
