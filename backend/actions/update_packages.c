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

#include "../actions.h"		/* update_packages_thread prototype */
#include "../jobs.h"		/* jobs_... */
#include "../pkgutils.h"	/* pkgutils_... */

static gboolean	update_packages(PkBackend *backend, bool simulate);

/*
 * The thread that performs an InstallPackages operation. Should be invoked
 * by the pk_backend_update_packages hook.
 */
gboolean
update_packages_thread(PkBackend *backend)
{

	return update_packages(backend, false);
}

/*
 * The thread that performs a SimulateInstallPackages operation. Should be
 * invoked by the pk_backend_simulate_update_packages hook.
 */
gboolean
simulate_update_packages_thread(PkBackend *backend)
{

	return update_packages(backend, true);
}

static gboolean
update_packages(PkBackend *backend, bool simulate)
{
	struct jobs_spec spec;
	bool		success;

	assert(backend != NULL);

	spec.backend = backend;
	spec.info = pkgutils_pkg_install_state;
	spec.jobs_failed_error = PK_ERROR_ENUM_PACKAGE_FAILED_TO_INSTALL;
	spec.no_jobs_error = PK_ERROR_ENUM_NO_PACKAGES_TO_UPDATE;
	spec.reject_non_updates = true;
	spec.simulate = simulate;
	spec.type = PKG_JOBS_INSTALL;
	spec.use_package_ids = true;

	(void)pk_backend_set_status(backend, PK_STATUS_ENUM_QUERY);
	success = jobs_do(&spec);
	(void)pk_backend_finished(backend);

	return success ? TRUE : FALSE;
}
