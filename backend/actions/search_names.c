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
#include "../pk-backend.h"
#include "pkg.h"

#include "../actions.h"		/* search_names_thread prototype */
#include "../db.h"		/* db_open_remote */
#include "../search.h"		/* search_... */
#include "../utils.h"		/* INTENTIONALLY_IGNORE */

/*
 * Searches for one or more packages by name.
 */
gboolean
search_names_thread(PkBackend *backend)
{
	gboolean	success;
	struct search	search;

	assert(backend != NULL);

	success = FALSE;
	(void)pk_backend_set_status(backend, PK_STATUS_ENUM_QUERY);
	(void)pk_backend_set_percentage(backend, 0);

	/* TODO: check search type */
	search.type = MATCH_REGEX;
	search.in = FIELD_NAME;
	search.sort_by = FIELD_NAME;
	search.filters = pk_backend_get_uint(backend, "filters");
	search.term = NULL;
	search.db = NULL;
	search.backend = backend;

	search.db = db_open_remote(backend);
	if (search.db != NULL) {
		guint		i;
		guint		len;
		gchar         **values;

		values = pk_backend_get_strv(backend, "search");
		assert(values != NULL);
		len = g_strv_length(values);

		success = TRUE;
		for (i = 0; i < len && success; i++) {
			search.term = values[i];
			success = search_do(&search);
			(void)pk_backend_set_percentage(backend,
			    ((i + 1) * 100) / len);
		}
	}
	pkgdb_close(search.db);
	(void)pk_backend_set_percentage(backend, 100);
	(void)pk_backend_finished(backend);
	return success;
}
