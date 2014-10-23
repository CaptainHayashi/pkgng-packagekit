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
#include <stdbool.h>		/* bool, true, false */
#include "../pk-backend.h"
#include "pkg.h"

#include "../actions.h"		/* resolve_thread prototype */
#include "../emit.h"		/* emit_package */
#include "../pkgutils.h"	/* pkgutils_... */
#include "../query.h"		/* query_* */
#include "../utils.h"		/* INTENTIONALLY_IGNORE */

static bool	emit(PkBackend *backend, struct pkgdb *db, struct pkg *pkg);

/*
 * Resolves a package identifier, which may be a full PackageID or a package
 * name, to a package that is promptly emitted.
 */
gboolean
resolve_thread(PkBackend *backend)
{
	bool		success;

	assert(backend != NULL);

	success = query_do_from_backend_names(backend, PKG_LOAD_BASIC, emit);

	(void)pk_backend_finished(backend);
	return success ? TRUE : FALSE;
}

static bool
emit(PkBackend *backend, struct pkgdb *db, struct pkg *pkg)
{

	INTENTIONALLY_IGNORE(db);

	emit_package(pkg, backend, pkgutils_pkg_current_state(pkg));
	return true;
}
