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
#include <glib.h>		/* gboolean */
#include <stdbool.h>		/* bool */
#include "pkg.h"		/* PKG_..., struct pkg, etc. */
#include "../pk-backend.h"	/* PkBackend */

#include "../actions.h"		/* Prototype */
#include "../query.h"		/* query_... */

static const unsigned int LOAD_FLAGS = PKG_LOAD_BASIC | PKG_LOAD_DEPS;

static bool	emit(PkBackend *backend, struct pkgdb *db, struct pkg *pkg);

/*
 * The thread that performs a GetDepends operation. Should be invoked by the
 * pk_backend_get_depends hook.
 */
gboolean
get_depends_thread(PkBackend *backend)
{
	bool		success;

	assert(backend != NULL);

	(void)pk_backend_set_status(backend, PK_STATUS_ENUM_QUERY);
	success = query_do_from_backend_ids(backend, LOAD_FLAGS, emit);

	(void)pk_backend_finished(backend);
	return success ? TRUE : FALSE;
}

static bool
emit(PkBackend *backend, struct pkgdb *db, struct pkg *pkg)
{

	assert(db != NULL);
	assert(pkg != NULL);
	assert(pkg != NULL);

	return query_depends_emit(backend, pkg, pkg_deps, db, LOAD_FLAGS);
}
