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
#include "pkg.h"		/* pkg... */
#include "../pk-backend.h"	/* pk_..., Pk... */

#include "../actions.h"		/* Prototype */
#include "../pkgutils.h"	/* pkgutils_... */
#include "../query.h"		/* query_..., struct query */
#include "../utils.h"		/* ERR */

static bool	emit(PkBackend *backend, struct pkgdb *db, struct pkg *pkg);

gboolean
get_update_detail_thread(PkBackend *backend)
{
	bool		success;

	success = query_do_from_backend_ids(backend, PKG_LOAD_BASIC, emit);
	(void)pk_backend_finished(backend);
	return success ? TRUE : FALSE;
}

static bool
emit(PkBackend *backend, struct pkgdb *db, struct pkg *pkg)
{
	bool		success;
	const char     *origin;
	struct pkg     *old;

	assert(backend != NULL);
	assert(db != NULL);
	assert(pkg != NULL);

	success = false;

	/* See if there's a version of this package already installed */
	origin = NULL;
	pkg_get(pkg, PKG_ORIGIN, &origin);

	old = NULL;
	pkgutils_add_old_version(db, pkg, &old);
	if (old != NULL && pkg_version_change(pkg) == PKG_UPGRADE) {
		const char     *update_text;
		gchar          *new_id;
		gchar          *old_id;

		new_id = pkgutils_pkg_to_id(pkg);
		old_id = pkgutils_pkg_to_id(old);

		/* TODO: elaborate on this */
		update_text = "new version";

		(void)pk_backend_update_detail(backend, new_id, old_id, NULL,
		    NULL, NULL, NULL, PK_RESTART_ENUM_NONE, update_text, NULL,
		    PK_UPDATE_STATE_ENUM_UNKNOWN, NULL, NULL);
		/* Phew */

		g_free(new_id);
		g_free(old_id);

		success = true;
	}
	pkg_free(old);

	return success;
}
