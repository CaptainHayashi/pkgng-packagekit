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
 *
 * This file contains code from the PKGNG source code (pkg/update.c);
 * its copyright notice is as follows:
 *
 * Copyright (c) 2011-2012 Baptiste Daroussin <bapt@FreeBSD.org>
 * Copyright (c) 2011-2012 Julien Laffaye <jlaffaye@FreeBSD.org>
 * Copyright (c) 2011-2012 Marin Atanasov Nikolov <dnaeon@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer
 * in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>		/* assert */
#include <glib.h>		/* gboolean */
#include <stdbool.h>		/* bool, true, false */
#include "../pk-backend.h"	/* pk..., Pk... */
#include "pkg.h"		/* pkg... */

#include "../actions.h"		/* refresh_cache_thread prototype */
#include "../pkgutils.h"	/* pkgutils_... */
#include "../query.h"		/* query_... */
#include "../utils.h"		/* STATUS, ERR */

static bool	update_repo(struct pkg_repo *repo, bool force);

/*
 * The thread that performs a RefreshCache operation. Should be invoked by
 * the pk_backend_refresh_cache hook.
 */
gboolean
refresh_cache_thread(PkBackend *backend)
{
	bool		success;
	gboolean	force;
	struct pkg_repo *repo;

	assert(backend != NULL);

	STATUS(backend, PK_STATUS_ENUM_QUERY);

	force = pk_backend_get_bool(backend, "force");

	repo = NULL;
	success = true;
	while (success) {
		int		err;

		err = pkg_repos(&repo);
		if (err == EPKG_OK) {
			success = update_repo(repo, force);
		} else {
			/* Did we run out of repos? */
			success = (err == EPKG_END);
			break;
		}
	}

	if (!success) {
		ERR(backend,
		    PK_ERROR_ENUM_REPO_NOT_AVAILABLE,
		    "Repository update failed.");
	}
	pk_backend_finished(backend);
	return success ? TRUE : FALSE;
}

static bool
update_repo(struct pkg_repo *repo, bool force)
{
	bool		success;

	assert(repo != NULL);

	/* Claim success if we don't need to update the repo. */
	success = true;

	/* Don't update disabled repositories. */
	if (pkg_repo_enabled(repo)) {
		int		err;

		err = pkg_update(repo, force);
		success = (err == EPKG_OK || err == EPKG_UPTODATE);
	}
	return success;
}
