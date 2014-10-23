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
 * GNU General Public License for more files.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <assert.h>		/* assert */
#include <glib.h>
#include <stdbool.h>
#include <sys/sbuf.h>
#include "../pk-backend.h"
#include "pkg.h"

#include "../actions.h"		/* get_files_thread prototype */
#include "../pkgutils.h"	/* pkgutils_... */
#include "../query.h"		/* query_... */

static const unsigned int LOAD_FLAGS = PKG_LOAD_BASIC | PKG_LOAD_FILES;

static bool	emit(PkBackend *backend, struct pkgdb *db, struct pkg *pkg);

/*
 * The thread that performs a GetFiles operation. Should be invoked by the
 * pk_backend_get_files hook.
 */
gboolean
get_files_thread(PkBackend *backend)
{
	bool		success;

	assert(backend != NULL);

	(void)pk_backend_set_status(backend, PK_STATUS_ENUM_QUERY);
	success = query_do_from_backend_ids(backend, LOAD_FLAGS, emit);

	(void)pk_backend_finished(backend);
	return success ? TRUE : FALSE;
}

/*
 * Emits the given package's files. To be used as an iterating function.
 */
static bool
emit(PkBackend *backend, struct pkgdb *db, struct pkg *pkg)
{
	bool		success;
	struct pkg_file *file;
	struct sbuf    *sb;

	assert(backend != NULL);
	assert(db != NULL);
	assert(pkg != NULL);

	success = false;
	file = NULL;

	/*
	 * Construct a string of the form ";file1;file2;file3;...". We'll get
	 * rid of the initial ; later.
	 */
	sb = sbuf_new_auto();
	assert(sb != NULL);

	while (pkg_files(pkg, &file) == EPKG_OK)
		sbuf_printf(sb, ";%s", pkg_file_path(file));

	if (sbuf_finish(sb) != 0)
		(void)pk_backend_error_code(backend,
		    PK_ERROR_ENUM_INTERNAL_ERROR,
		    "couldn't construct filename string");
	else {
		char           *filenames;
		gchar          *id;

		filenames = sbuf_data(sb);
		/* Skip over any initial ; */
		if (filenames[0] == ';')
			filenames++;

		id = pkgutils_pkg_to_id(pkg);

		success = (pk_backend_files(backend, id, filenames) ==
		    TRUE);

		g_free(id);
	}

	sbuf_delete(sb);
	return success;
}
