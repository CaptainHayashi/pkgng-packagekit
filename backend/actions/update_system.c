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

#include "../actions.h"		/* update_system_thread prototype */
#include "../jobs.h"		/* jobs_... */
#include "../pkgutils.h"	/* pkgutils_... */

/*
 * The thread that performs an UpdateSystem operation. Should be invoked by
 * the pk_backend_update_system hook.
 */
gboolean
update_system_thread(PkBackend *backend)
{
	struct jobs_spec spec;
	bool		success;

	assert(backend != NULL);

	spec.backend = backend;
	spec.info = pkgutils_pkg_install_state;
	spec.jobs_failed_error = PK_ERROR_ENUM_PACKAGE_FAILED_TO_INSTALL;
	spec.no_jobs_error = PK_ERROR_ENUM_NO_PACKAGES_TO_UPDATE;
	spec.reject_non_updates = false;
	spec.simulate = false;
	spec.status = PK_STATUS_ENUM_UPDATE;
	spec.type = PKG_JOBS_UPGRADE;
	spec.use_package_ids = false;

	(void)pk_backend_set_status(backend, PK_STATUS_ENUM_QUERY);
	success = jobs_do(&spec);
	(void)pk_backend_finished(backend);

	return success ? TRUE : FALSE;
}
