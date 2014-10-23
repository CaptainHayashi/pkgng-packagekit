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
#include <glib.h>
#include <stdbool.h>
#include <stdint.h>
#include "../pk-backend.h"
#include "pkg.h"

#include "../actions.h"		/* get_details_thread prototype */
#include "../group.h"		/* group_from_origin */
#include "../licenses.h"	/* license_from_pkg */
#include "../pkgutils.h"	/* pkgutils_... */
#include "../query.h"		/* query_... */

static const unsigned int LOAD_FLAGS = PKG_LOAD_BASIC | PKG_LOAD_LICENSES;

static bool	emit(PkBackend *backend, struct pkgdb *db, struct pkg *pkg);

/*
 * The thread that performs a GetDetails operation. Should be invoked by the
 * pk_backend_get_details hook.
 */
gboolean
get_details_thread(PkBackend *backend)
{
	bool		success;

	assert(backend != NULL);

	success = query_do_from_backend_ids(backend, LOAD_FLAGS, emit);
	(void)pk_backend_finished(backend);
	return success ? TRUE : FALSE;
}

/*
 * Emits the given package's details. To be used as an iterating function.
 */
static bool
emit(PkBackend *backend, struct pkgdb *db, struct pkg *pkg)
{
	bool		success;
	const char     *description;
	const char     *www;
	gchar          *id;
	int64_t		flatsize;
	guint		flatsize_u;

	assert(backend != NULL);
	assert(db != NULL);
	assert(pkg != NULL);

	/* Information not already part of the PackageID */
	description = www = NULL;
	flatsize = 0;
	pkg_get(pkg,
	    PKG_DESC, &description,
	    PKG_FLATSIZE, &flatsize,
	    PKG_WWW, &www);

	flatsize_u = (guint) CLAMP(flatsize, 0, G_MAXUINT);

	id = pkgutils_pkg_to_id(pkg);

	success = pk_backend_details(backend,
	    id,
	    license_name_from_pkg(pkg),
	    group_of_pkg(pkg),
	    description,
	    www,
	    flatsize_u) == TRUE;

	g_free(id);

	return success;
}
