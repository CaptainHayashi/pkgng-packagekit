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
#include <stdbool.h>		/* bool, true, false */
#include "../pk-backend.h"	/* pk..., Pk... */
#include "pkg.h"		/* pkg... */

#include "../actions.h"		/* remove_packages_thread prototype */
#include "../jobs.h"		/* jobs... */
#include "../pkgutils.h"	/* pkgutils_... */

static gboolean	remove_packages(PkBackend *backend, bool simulate);

/*
 * The thread that performs an removePackages operation. Should be invoked by
 * the pk_backend_remove_packages hook.
 */
gboolean
remove_packages_thread(PkBackend *backend)
{

	return remove_packages(backend, false);
}

/*
 * The thread that performs a SimulateRemovePackages operation. Should be
 * invoked by the pk_backend_simulate_remove_packages hook.
 */
gboolean
simulate_remove_packages_thread(PkBackend *backend)
{

	return remove_packages(backend, true);
}

static gboolean
remove_packages(PkBackend *backend, bool simulate)
{
	struct jobs_spec spec;
	bool		success;

	assert(backend != NULL);

	spec.backend = backend;
	spec.info = pkgutils_pkg_remove_state;
	spec.jobs_failed_error = PK_ERROR_ENUM_PACKAGE_FAILED_TO_REMOVE;
	spec.no_jobs_error = PK_ERROR_ENUM_PACKAGE_NOT_INSTALLED;
	spec.reject_non_updates = false;
	spec.simulate = simulate;
	spec.status = PK_STATUS_ENUM_REMOVE;
	spec.type = PKG_JOBS_DEINSTALL;
	spec.use_package_ids = true;

	(void)pk_backend_set_status(backend, PK_STATUS_ENUM_QUERY);
	success = jobs_do(&spec);
	(void)pk_backend_finished(backend);

	return success ? TRUE : FALSE;
}
