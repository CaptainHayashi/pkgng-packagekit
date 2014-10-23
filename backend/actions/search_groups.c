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

#include "../actions.h"		/* search_groups_thread prototype */
#include "../db.h"		/* db_open_remote */
#include "../group.h"		/* group_... */
#include "../search.h"		/* search_... */
#include "../utils.h"		/* INTENTIONALLY_IGNORE */

/*
 * Searches for a package by group.
 */
gboolean
search_groups_thread(PkBackend *backend)
{
	gboolean	success;
	guint		groupc;
	guint		i;
	PkGroupEnum    *groupv;
	struct pkgdb_it *it;
	gchar         **values;
	struct search	search;

	assert(backend != NULL);

	search.db = NULL;
	groupv = NULL;
	it = NULL;
	search.term = NULL;
	success = FALSE;
	values = NULL;

	(void)pk_backend_set_status(backend, PK_STATUS_ENUM_QUERY);
	(void)pk_backend_set_percentage(backend, 0);

	search.db = db_open_remote(backend);
	if (search.db == NULL)
		goto cleanup;

	values = pk_backend_get_strv(backend, "search");
	if (values == NULL) {
		ERR(backend, PK_ERROR_ENUM_INTERNAL_ERROR, "search was NULL");
		goto cleanup;
	}
	groupc = g_strv_length(values);
	if (groupc == 0) {
		ERR(backend, PK_ERROR_ENUM_INTERNAL_ERROR, "no values");
		goto cleanup;
	}
	groupv = calloc(groupc, sizeof(PkGroupEnum));
	if (groupv == NULL) {
		ERR(backend, PK_ERROR_ENUM_OOM, "couldn't alloc groupv");
		goto cleanup;
	}
	for (i = 0; i < groupc; i++) {
		groupv[i] = pk_group_enum_from_string(values[i]);
		if (groupv[i] == PK_GROUP_ENUM_UNKNOWN)
			ERR(backend, PK_ERROR_ENUM_GROUP_NOT_FOUND, values[i]);
	}

	search.term = group_list_to_origin_regex(groupc, groupv);
	if (search.term == NULL || search.term[0] == '\0') {
		ERR(backend, PK_ERROR_ENUM_INTERNAL_ERROR, "no search.term");
		goto cleanup;
	}
	search.type = MATCH_REGEX;
	search.in = FIELD_ORIGIN;
	search.sort_by = FIELD_ORIGIN;
	search.backend = backend;
	search.filters = pk_backend_get_uint(backend, "filters");

	success = search_do(&search);

cleanup:
	free(search.term);
	free(groupv);
	pkgdb_close(search.db);

	(void)pk_backend_set_percentage(backend, 100);
	(void)pk_backend_finished(backend);
	return success;
}
