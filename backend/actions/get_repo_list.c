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
#include "../pk-backend.h"
#include "pkg.h"

#include "../actions.h"		/* get_repo_list_thread prototype */

/*
 * The thread that performs a GetRepoList operation. Should be invoked by the
 * pk_backend_get_repo_list hook.
 */
gboolean
get_repo_list_thread(PkBackend *backend)
{
	struct pkg_repo *repo;

	assert(backend != NULL);

	(void)pk_backend_set_status(backend, PK_STATUS_ENUM_QUERY);
	(void)pk_backend_set_percentage(backend, 0);

	repo = NULL;
	while (pkg_repos(&repo) == EPKG_OK) {
		assert(backend != NULL);
		(void)pk_backend_repo_detail(backend,
		    pkg_repo_name(repo),
		    pkg_repo_ident(repo),
		    pkg_repo_enabled(repo));
	}

	(void)pk_backend_set_percentage(backend, 100);
	(void)pk_backend_finished(backend);

	return TRUE;
}
